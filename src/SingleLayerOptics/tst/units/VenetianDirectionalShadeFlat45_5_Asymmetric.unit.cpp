#include <memory>
#include <gtest/gtest.h>

#include "WCECommon.hpp"
#include "WCESingleLayerOptics.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;

// Directional-diffuse venetian with slats whose two faces differ (Rf != Rb) and/or transmit.
// The expected values come from an independent radiosity solution of the ISO 15099 slat
// enclosure (WINDOW Technical Documentation 11.2.2) with the same 5 segments per slat, flat
// slats 16 mm wide, 12 mm apart, tilted 45 degrees (rising toward the interior). The slat
// material's front side is the up-facing slat surface. Tolerances reflect the Klems
// discretisation, measured on the symmetric opaque case where the two methods agree.
class TestVenetianDirectionalShadeFlat45_5_Asymmetric : public testing::Test
{
protected:
    static std::shared_ptr<CBSDFLayer>
      makeShade(const double Tmat, const double Rfmat, const double Rbmat)
    {
        const auto aMaterial = Material::singleBandMaterial(Tmat, Tmat, Rfmat, Rbmat);

        const auto slatWidth = 0.016;     // m
        const auto slatSpacing = 0.012;   // m
        const auto slatTiltAngle = 45;
        const auto curvatureRadius = 0;
        const size_t numOfSlatSegments = 5;

        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Full);

        return CBSDFLayerMaker::getVenetianLayer(aMaterial,
                                                 aBSDF,
                                                 slatWidth,
                                                 slatSpacing,
                                                 slatTiltAngle,
                                                 curvatureRadius,
                                                 numOfSlatSegments,
                                                 DistributionMethod::DirectionalDiffuse,
                                                 true);
    }

    struct Expected
    {
        double TfDiff;
        double RfDiff;
        double RbDiff;
        double TfNormal;
        double RfNormal;
        double TbNormal;
        double RbNormal;
    };

    static void checkAgainstReference(BSDFIntegrator & results, const Expected & expected)
    {
        constexpr double diffuseTolerance = 6e-3;
        constexpr double normalTolerance = 3e-3;

        // A passive layer transmits the same diffuse flux in both directions
        const double TfDiff = results.DiffDiff(Side::Front, PropertySurface::T);
        const double TbDiff = results.DiffDiff(Side::Back, PropertySurface::T);
        EXPECT_NEAR(TfDiff, TbDiff, 1e-3);

        EXPECT_NEAR(expected.TfDiff, TfDiff, diffuseTolerance);
        EXPECT_NEAR(expected.RfDiff, results.DiffDiff(Side::Front, PropertySurface::R), diffuseTolerance);
        EXPECT_NEAR(expected.RbDiff, results.DiffDiff(Side::Back, PropertySurface::R), diffuseTolerance);

        EXPECT_NEAR(expected.TfNormal, results.DirHem(Side::Front, PropertySurface::T, 0, 0), normalTolerance);
        EXPECT_NEAR(expected.RfNormal, results.DirHem(Side::Front, PropertySurface::R, 0, 0), normalTolerance);
        EXPECT_NEAR(expected.TbNormal, results.DirHem(Side::Back, PropertySurface::T, 0, 0), normalTolerance);
        EXPECT_NEAR(expected.RbNormal, results.DirHem(Side::Back, PropertySurface::R, 0, 0), normalTolerance);
    }
};

TEST_F(TestVenetianDirectionalShadeFlat45_5_Asymmetric, OpaqueAsymmetricSlats)
{
    SCOPED_TRACE("Begin Test: Venetian directional diffuse, opaque slats Rf 0.8 / Rb 0.2.");

    auto results = makeShade(0.0, 0.8, 0.2)->getResults();

    checkAgainstReference(results,
                          {.TfDiff = 0.2730,
                           .RfDiff = 0.3549,
                           .RbDiff = 0.0989,
                           .TfNormal = 0.1358,
                           .RfNormal = 0.4092,
                           .TbNormal = 0.0974,
                           .RbNormal = 0.1061});
}

TEST_F(TestVenetianDirectionalShadeFlat45_5_Asymmetric, TranslucentSymmetricSlats)
{
    SCOPED_TRACE("Begin Test: Venetian directional diffuse, translucent slats T 0.2, R 0.5.");

    auto results = makeShade(0.2, 0.5, 0.5)->getResults();

    checkAgainstReference(results,
                          {.TfDiff = 0.3746,
                           .RfDiff = 0.2979,
                           .RbDiff = 0.2979,
                           .TfNormal = 0.2656,
                           .RfNormal = 0.3319,
                           .TbNormal = 0.2656,
                           .RbNormal = 0.3319});
}

TEST_F(TestVenetianDirectionalShadeFlat45_5_Asymmetric, TranslucentAsymmetricSlats)
{
    SCOPED_TRACE("Begin Test: Venetian directional diffuse, slats T 0.2, Rf 0.7 / Rb 0.2.");

    auto results = makeShade(0.2, 0.7, 0.2)->getResults();

    checkAgainstReference(results,
                          {.TfDiff = 0.3510,
                           .RfDiff = 0.3852,
                           .RbDiff = 0.1371,
                           .TfNormal = 0.2517,
                           .RfNormal = 0.4398,
                           .TbNormal = 0.2138,
                           .RbNormal = 0.1391});
}
