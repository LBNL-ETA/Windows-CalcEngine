#include <cstdlib>
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <gtest/gtest.h>

#include "WCECommon.hpp"
#include "WCESingleLayerOptics.hpp"

#include "csvHandlers.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;

// Venetian blind BSDFs against an independent ray-traced reference.
//
// The reference matrices were produced with Radiance (genfmtx, Klems full basis, 20000
// rays per patch, 12 ambient bounces) for the same geometry (curved slats as a 12-facet
// polygon strip); see docs/validation/venetian_radiance.md for the setup, the conventions
// and how the reference files are regenerated. Each reference CSV has 145 rows (incoming
// patch, this library's Klems-full ordering) and 13 columns: patch index, then for Tf, Tb,
// Rf, Rb the directional-hemispherical value, its direct-direct part (diagonal times
// lambda) and the diffuse remainder.
//
// What is asserted, and why these tolerances:
// - the diffuse part (dir-hem minus the direct-direct term) of the hemispherical value and
//   of the dir-hem at normal incidence, within kIntegratedTolerance: the Radiance noise
//   floor is about 0.001 and the model difference between a 2D radiosity cell with five
//   segments and a ray-traced blind is up to 0.005 over the cases (Radiance slightly
//   higher in T, lower in R);
// - the diffuse part of the dir-hem per incoming patch for theta <= 45 degrees within
//   kDiffuseTolerance and for 45 to 65 degrees within kDiffuseTolerance_Oblique. Tilted
//   slats stay within 0.010 / 0.025; horizontal slats reach 0.017 / 0.034 because their lit
//   fraction changes fastest within a patch, so the patch-centre beam irradiance (the same
//   approximation as below) also shows in the diffuse part;
// - the totals (with the direct-direct term) only loosely, kTotalTolerance and
//   kTotalTolerance_Normal: the engine evaluates the beam cut-off at the patch-centre
//   direction while Radiance averages over the patch. Near the cut-off profile angle the
//   two differ by design: up to 0.2 at 70 degrees for the 45-degree blind, and 0.040 at
//   normal incidence for horizontal slats (direct transmittance 1.0 at the centre of the
//   0-5 degree patch against 0.96 averaged over it). That is a known resolution limit of
//   the method, shared with the legacy engine and documented in the notes.
//
// Setting the environment variable WCE_VENETIAN_DUMP_DIR makes every test also write the
// engine's own columns in the reference layout to <dir>/wce_<reference file>, which
// docs/validation/scripts/summarize.py uses for the comparison tables.
namespace
{
    struct Blind
    {
        double Tmat;
        double Rfmat;
        double Rbmat;
        double slatSpacing{0.012};     // m
        double slatTiltAngle{45.0};    // deg, positive lifts the interior edge
        double curvatureRadius{0.0};   // m, positive is crown up
    };

    constexpr std::pair<Side, PropertySurface> kProperties[] = {{Side::Front, PropertySurface::T},
                                                                {Side::Back, PropertySurface::T},
                                                                {Side::Front, PropertySurface::R},
                                                                {Side::Back, PropertySurface::R}};
}   // namespace

class TestVenetianRadianceReference : public testing::Test
{
protected:
    static constexpr double kIntegratedTolerance = 0.008;         // diffuse part, hemispherical
    static constexpr double kNormalDiffuseTolerance = 0.012;      // diffuse part at normal incidence: for horizontal
                                                                  // slats nothing is lit at exactly 0 deg while Radiance
                                                                  // averages the 0-5 deg patch (observed 0.008)
    static constexpr double kTotalTolerance = 0.020;              // total hemispherical, includes the direct term
    static constexpr double kTotalTolerance_Normal = 0.050;       // total at normal incidence (horizontal slats: 0.040)
    static constexpr double kDiffuseTolerance = 0.020;            // observed up to 0.017 (horizontal slats)
    static constexpr double kDiffuseTolerance_Oblique = 0.040;    // observed up to 0.034 (horizontal slats)
    static constexpr size_t kLastPatchWithin45 = 68;    // rings 0..4 of the Klems full basis
    static constexpr size_t kLastPatchWithin65 = 116;   // rings 5..6

    static std::shared_ptr<CBSDFLayer> makeShade(const Blind & blind)
    {
        const auto aMaterial = Material::singleBandMaterial(blind.Tmat, blind.Tmat, blind.Rfmat, blind.Rbmat);
        const auto slatWidth = 0.016;   // m
        const size_t numOfSlatSegments = 5;
        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Full);
        return CBSDFLayerMaker::getVenetianLayer(aMaterial,
                                                 aBSDF,
                                                 slatWidth,
                                                 blind.slatSpacing,
                                                 blind.slatTiltAngle,
                                                 blind.curvatureRadius,
                                                 numOfSlatSegments,
                                                 DistributionMethod::DirectionalDiffuse,
                                                 true);
    }

    struct Columns
    {
        size_t dirHem;
        size_t direct;
        size_t diffuse;
    };

    static Columns columnsFor(const Side side, const PropertySurface property)
    {
        // order in the file: tf, tb, rf, rb
        const size_t block = (property == PropertySurface::T ? 0 : 2) + (side == Side::Front ? 0 : 1);
        const size_t first = 1 + 3 * block;
        return {.dirHem = first, .direct = first + 1, .diffuse = first + 2};
    }

    static void checkProperty(BSDFIntegrator & results,
                              const std::vector<std::vector<double>> & reference,
                              const Side side,
                              const PropertySurface property,
                              const std::string & label)
    {
        const auto cols = columnsFor(side, property);
        const auto lambda = results.lambdaVector();
        const auto dirHem = results.DirHem(side, property);
        const auto & matrix = results.getMatrix(side, property).getMatrix();
        ASSERT_EQ(reference.size(), dirHem.size());

        // Hemispherical values, total and diffuse part only. The diffuse part is what the
        // radiosity model controls; the direct-direct part carries the patch-centre
        // evaluation of the beam cut-off, whose error depends on the geometry (0.013 in
        // hemispherical T for horizontal slats, where the cut-off gradient is largest at
        // normal incidence), so the total gets the loose bound.
        double referenceTotal = 0.0;
        double referenceDiffuse = 0.0;
        double engineDiffuse = 0.0;
        for(size_t patch = 0; patch < reference.size(); ++patch)
        {
            referenceTotal += reference[patch][cols.dirHem] * lambda[patch];
            referenceDiffuse += reference[patch][cols.diffuse] * lambda[patch];
            engineDiffuse += (dirHem[patch] - matrix[patch][patch] * lambda[patch]) * lambda[patch];
        }
        EXPECT_NEAR(referenceTotal / WCE_PI, results.DiffDiff(side, property), kTotalTolerance)
          << label << " hemispherical, total";
        EXPECT_NEAR(referenceDiffuse / WCE_PI, engineDiffuse / WCE_PI, kIntegratedTolerance)
          << label << " hemispherical, diffuse part";

        // Normal incidence: diffuse part tightly, total loosely (same reason).
        const double normalDiffuse = dirHem[0] - matrix[0][0] * lambda[0];
        EXPECT_NEAR(reference[0][cols.diffuse], normalDiffuse, kNormalDiffuseTolerance)
          << label << " normal incidence, diffuse part";
        EXPECT_NEAR(reference[0][cols.dirHem], dirHem[0], kTotalTolerance_Normal)
          << label << " normal incidence, total";

        for(size_t patch = 0; patch <= kLastPatchWithin65; ++patch)
        {
            const double diffuse = dirHem[patch] - matrix[patch][patch] * lambda[patch];
            const double tolerance =
              patch <= kLastPatchWithin45 ? kDiffuseTolerance : kDiffuseTolerance_Oblique;
            EXPECT_NEAR(reference[patch][cols.diffuse], diffuse, tolerance)
              << label << " diffuse part, incoming patch " << patch;
        }
    }

    //! Engine columns in the reference layout, for the comparison tables of the notes.
    static void dumpEngineColumns(BSDFIntegrator & results, const std::string & referenceFile)
    {
        const char * dir = std::getenv("WCE_VENETIAN_DUMP_DIR");
        if(dir == nullptr)
        {
            return;
        }
        const auto lambda = results.lambdaVector();
        std::ofstream out(std::string(dir) + "/wce_" + referenceFile);
        out.precision(6);
        out << std::fixed;
        for(size_t patch = 0; patch < lambda.size(); ++patch)
        {
            out << patch;
            for(const auto & [side, property] : kProperties)
            {
                const double total = results.DirHem(side, property)[patch];
                const double direct = results.getMatrix(side, property).getMatrix()[patch][patch] * lambda[patch];
                out << "," << total << "," << direct << "," << (total - direct);
            }
            out << ",\n";
        }
    }

    static void checkAgainstRadiance(const Blind & blind, const std::string & referenceFile)
    {
        auto results = makeShade(blind)->getResults();
        dumpEngineColumns(results, referenceFile);
        const auto reference = Helper::readMatrixFromCSV(
          std::string(TEST_DATA_DIR_SINGLE_LAYER_OPTICS) + "/data/radiance/" + referenceFile);
        checkProperty(results, reference, Side::Front, PropertySurface::T, "Tf");
        checkProperty(results, reference, Side::Back, PropertySurface::T, "Tb");
        checkProperty(results, reference, Side::Front, PropertySurface::R, "Rf");
        checkProperty(results, reference, Side::Back, PropertySurface::R, "Rb");
    }
};

// Base set: flat slats 16 mm wide, 12 mm spacing, 45 degrees

TEST_F(TestVenetianRadianceReference, OpaqueSymmetricSlats)
{
    checkAgainstRadiance({.Tmat = 0.0, .Rfmat = 0.5, .Rbmat = 0.5}, "venetian_flat45_R0.5.csv");
}

TEST_F(TestVenetianRadianceReference, OpaqueAsymmetricSlats)
{
    checkAgainstRadiance({.Tmat = 0.0, .Rfmat = 0.8, .Rbmat = 0.2}, "venetian_flat45_Rf0.8_Rb0.2.csv");
}

TEST_F(TestVenetianRadianceReference, OpaqueAsymmetricSlatsMirrored)
{
    checkAgainstRadiance({.Tmat = 0.0, .Rfmat = 0.2, .Rbmat = 0.8}, "venetian_flat45_Rf0.2_Rb0.8.csv");
}

TEST_F(TestVenetianRadianceReference, TranslucentSymmetricSlats)
{
    checkAgainstRadiance({.Tmat = 0.2, .Rfmat = 0.5, .Rbmat = 0.5}, "venetian_flat45_T0.2_R0.5.csv");
}

TEST_F(TestVenetianRadianceReference, TranslucentAsymmetricSlats)
{
    checkAgainstRadiance({.Tmat = 0.2, .Rfmat = 0.7, .Rbmat = 0.2}, "venetian_flat45_T0.2_Rf0.7_Rb0.2.csv");
}

TEST_F(TestVenetianRadianceReference, CurvedOpaqueSymmetricSlats)
{
    // rise 1 mm on a 16 mm chord: radius 32.5 mm
    checkAgainstRadiance({.Tmat = 0.0, .Rfmat = 0.5, .Rbmat = 0.5, .curvatureRadius = 0.0325},
                         "venetian_curved45_rise1mm_R0.5.csv");
}

// Extended set: other tilts, spacing and curvature

TEST_F(TestVenetianRadianceReference, HorizontalSlats)
{
    checkAgainstRadiance({.Tmat = 0.0, .Rfmat = 0.5, .Rbmat = 0.5, .slatTiltAngle = 0.0},
                         "venetian_flat0_R0.5.csv");
}

TEST_F(TestVenetianRadianceReference, NegativeTiltAsymmetricSlats)
{
    checkAgainstRadiance({.Tmat = 0.0, .Rfmat = 0.8, .Rbmat = 0.2, .slatTiltAngle = -45.0},
                         "venetian_flatm45_Rf0.8_Rb0.2.csv");
}

TEST_F(TestVenetianRadianceReference, NearlyClosedSlats)
{
    checkAgainstRadiance({.Tmat = 0.0, .Rfmat = 0.5, .Rbmat = 0.5, .slatTiltAngle = 80.0},
                         "venetian_flat80_R0.5.csv");
}

TEST_F(TestVenetianRadianceReference, WideSpacingSlats)
{
    checkAgainstRadiance({.Tmat = 0.0, .Rfmat = 0.5, .Rbmat = 0.5, .slatSpacing = 0.016},
                         "venetian_flat45_s16_R0.5.csv");
}

TEST_F(TestVenetianRadianceReference, StronglyCurvedAsymmetricSlats)
{
    // rise 3 mm on a 16 mm chord: radius 12.17 mm
    checkAgainstRadiance({.Tmat = 0.0, .Rfmat = 0.8, .Rbmat = 0.2, .curvatureRadius = 0.0121667},
                         "venetian_curved45_rise3mm_Rf0.8_Rb0.2.csv");
}

TEST_F(TestVenetianRadianceReference, HorizontalTranslucentAsymmetricSlats)
{
    checkAgainstRadiance({.Tmat = 0.2, .Rfmat = 0.7, .Rbmat = 0.2, .slatTiltAngle = 0.0},
                         "venetian_flat0_T0.2_Rf0.7_Rb0.2.csv");
}
