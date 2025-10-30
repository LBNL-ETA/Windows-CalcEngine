// MultiPaneBSDF_102_Specular_vs_DirDiff.unit.cpp
#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

#include "optical/standardData.hpp"
#include "optical/spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

// -----------------------------------------------------------------------------
// File-local helpers and data types (anonymous namespace = internal linkage)
// -----------------------------------------------------------------------------
namespace
{

    // Readable, file-local assertion helper (no macro needed)
    inline void expectBothNear(
      const std::string & label, double expected, double spec, double dirdif, double tol)
    {
        SCOPED_TRACE(::testing::Message() << label << " | expected=" << expected
                                          << " | spec=" << spec << " | dirdif=" << dirdif);

        EXPECT_NEAR(spec, expected, tol);
        EXPECT_NEAR(dirdif, expected, tol);
        EXPECT_NEAR(spec, dirdif, tol);
    }

    // Named golden metrics
    struct DirGold
    {
        double T_dirHem;   // τ_dir→hem
        double T_dirDir;   // τ_dir→dir
        double R_dirHem;   // ρ_dir→hem
        double R_dirDir;   // ρ_dir→dir
        double Abs1_dir;   // Abs(layer=1) for given θ,φ
    };

    struct Gold
    {
        double T_diffDiff;   // τ_dif→dif
        double R_diffDiff;   // ρ_dif→dif
        double Abs1_diff;    // Abs_dif(layer=1)
        DirGold at_0_0;      // θ=0°, φ=0°
        DirGold at_45_78;    // θ=45°, φ=78°
    };

    struct RangeCfg
    {
        const char * name;
        double minL;
        double maxL;
        CalculationProperties props;
        Gold gold;
    };

}   // namespace

// -----------------------------------------------------------------------------
// Fixture
// -----------------------------------------------------------------------------
class MultiPaneBSDF_102_Specular_vs_DirDiff : public ::testing::Test
{
    static std::unique_ptr<CMultiPaneBSDF> makeSpecular()
    {
        constexpr double thickness = 3.048e-3;
        auto mat =
          Material::nBandMaterial(SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);
        auto bsdf = BSDFHemisphere::create(BSDFBasis::Quarter);
        auto lyr = CBSDFLayerMaker::getSpecularLayer(mat, bsdf);
        return CMultiPaneBSDF::create({lyr});
    }
    static std::unique_ptr<CMultiPaneBSDF> makeDirectDiffuse()
    {
        constexpr double thickness = 3.048e-3;
        auto mat =
          Material::nBandMaterial(SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);
        auto bsdf = BSDFHemisphere::create(BSDFBasis::Quarter);
        auto lyr = CBSDFLayerMaker::getDirDifLayer(mat, bsdf);
        return CMultiPaneBSDF::create({lyr});
    }

public:
    static CMultiPaneBSDF & specular()
    {
        static auto L = makeSpecular();
        return *L;
    }
    static CMultiPaneBSDF & directdiffuse()
    {
        static auto L = makeDirectDiffuse();
        return *L;
    }
};

// -----------------------------------------------------------------------------
// Test
// -----------------------------------------------------------------------------
TEST_F(MultiPaneBSDF_102_Specular_vs_DirDiff, ModelsMatchAndHitGoldenNumbers)
{
    constexpr double tol = 1e-6;
    const auto side = Side::Front;

    // Build CalculationProperties used below
    const CalculationProperties solarProps{
      StandardData::solarRadiationASTM_E891_87_Table1(),
      StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
    const CalculationProperties visProps{StandardData::Photopic::solarRadiation(),
                                         StandardData::Photopic::wavelengthSetPhotopic(),
                                         StandardData::Photopic::detectorData()};

    // Ranges (C++20 designated initializers; follow declaration order)
    RangeCfg ranges[] = {RangeCfg{.name = "Solar",
                                  .minL = 0.30,
                                  .maxL = 2.50,
                                  .props = solarProps,
                                  .gold = Gold{.T_diffDiff = 0.745404,
                                               .R_diffDiff = 0.152995,
                                               .Abs1_diff = 0.101601,
                                               .at_0_0 = DirGold{.T_dirHem = 0.833843,
                                                                 .T_dirDir = 0.833843,
                                                                 .R_dirHem = 0.074761,
                                                                 .R_dirDir = 0.074761,
                                                                 .Abs1_dir = 0.091396},
                                               .at_45_78 = DirGold{.T_dirHem = 0.822262,
                                                                   .T_dirDir = 0.822262,
                                                                   .R_dirHem = 0.079326,
                                                                   .R_dirDir = 0.079326,
                                                                   .Abs1_dir = 0.098413}}},
                         RangeCfg{.name = "Visible",
                                  .minL = 0.38,
                                  .maxL = 0.78,
                                  .props = visProps,
                                  .gold = Gold{.T_diffDiff = 0.813555,
                                               .R_diffDiff = 0.165793,
                                               .Abs1_diff = 0.020652,
                                               .at_0_0 = DirGold{.T_dirHem = 0.899260,
                                                                 .T_dirDir = 0.899260,
                                                                 .R_dirHem = 0.082563,
                                                                 .R_dirDir = 0.082563,
                                                                 .Abs1_dir = 0.018177},
                                               .at_45_78 = DirGold{.T_dirHem = 0.892556,
                                                                   .T_dirDir = 0.892556,
                                                                   .R_dirHem = 0.087775,
                                                                   .R_dirDir = 0.087775,
                                                                   .Abs1_dir = 0.019669}}}};

    for(const auto & r : ranges)
    {
        auto & A = specular();
        auto & B = directdiffuse();
        A.setCalculationProperties(r.props);
        B.setCalculationProperties(r.props);

        // ---- Diffuse–diffuse (angle-independent) -----------------------------------
        {
            const double tA = A.DiffDiff(r.minL, r.maxL, side, PropertySurface::T);
            const double tB = B.DiffDiff(r.minL, r.maxL, side, PropertySurface::T);
            expectBothNear(std::string(r.name) + " τ_dif→dif", r.gold.T_diffDiff, tA, tB, tol);

            const double rA = A.DiffDiff(r.minL, r.maxL, side, PropertySurface::R);
            const double rB = B.DiffDiff(r.minL, r.maxL, side, PropertySurface::R);
            expectBothNear(std::string(r.name) + " ρ_dif→dif", r.gold.R_diffDiff, rA, rB, tol);

            const double aA = A.AbsDiff(r.minL, r.maxL, side, /*layer*/ 1);
            const double aB = B.AbsDiff(r.minL, r.maxL, side, /*layer*/ 1);
            expectBothNear(
              std::string(r.name) + " Abs_dif(layer=1)", r.gold.Abs1_diff, aA, aB, tol);
        }

        // ---- θ = 0°, φ = 0° --------------------------------------------------------
        {
            const double th = 0.0, ph = 0.0;

            double vA = A.DirHem(r.minL, r.maxL, side, PropertySurface::T, th, ph);
            double vB = B.DirHem(r.minL, r.maxL, side, PropertySurface::T, th, ph);
            expectBothNear(
              std::string(r.name) + " θ=0 φ=0 τ_dir→hem", r.gold.at_0_0.T_dirHem, vA, vB, tol);

            vA = A.DirDir(r.minL, r.maxL, side, PropertySurface::T, th, ph);
            vB = B.DirDir(r.minL, r.maxL, side, PropertySurface::T, th, ph);
            expectBothNear(
              std::string(r.name) + " θ=0 φ=0 τ_dir→dir", r.gold.at_0_0.T_dirDir, vA, vB, tol);

            vA = A.DirHem(r.minL, r.maxL, side, PropertySurface::R, th, ph);
            vB = B.DirHem(r.minL, r.maxL, side, PropertySurface::R, th, ph);
            expectBothNear(
              std::string(r.name) + " θ=0 φ=0 ρ_dir→hem", r.gold.at_0_0.R_dirHem, vA, vB, tol);

            vA = A.DirDir(r.minL, r.maxL, side, PropertySurface::R, th, ph);
            vB = B.DirDir(r.minL, r.maxL, side, PropertySurface::R, th, ph);
            expectBothNear(
              std::string(r.name) + " θ=0 φ=0 ρ_dir→dir", r.gold.at_0_0.R_dirDir, vA, vB, tol);

            vA = A.Abs(r.minL, r.maxL, side, /*layer*/ 1, th, ph);
            vB = B.Abs(r.minL, r.maxL, side, /*layer*/ 1, th, ph);
            expectBothNear(std::string(r.name) + " θ=0 φ=0 Abs_dir(layer=1)",
                           r.gold.at_0_0.Abs1_dir,
                           vA,
                           vB,
                           tol);
        }

        // ---- θ = 45°, φ = 78° ------------------------------------------------------
        {
            const double th = 45.0, ph = 78.0;

            double vA = A.DirHem(r.minL, r.maxL, side, PropertySurface::T, th, ph);
            double vB = B.DirHem(r.minL, r.maxL, side, PropertySurface::T, th, ph);
            expectBothNear(
              std::string(r.name) + " θ=45 φ=78 τ_dir→hem", r.gold.at_45_78.T_dirHem, vA, vB, tol);

            vA = A.DirHem(r.minL, r.maxL, side, PropertySurface::R, th, ph);
            vB = B.DirHem(r.minL, r.maxL, side, PropertySurface::R, th, ph);
            expectBothNear(
              std::string(r.name) + " θ=45 φ=78 ρ_dir→hem", r.gold.at_45_78.R_dirHem, vA, vB, tol);

            vA = A.Abs(r.minL, r.maxL, side, /*layer*/ 1, th, ph);
            vB = B.Abs(r.minL, r.maxL, side, /*layer*/ 1, th, ph);
            expectBothNear(std::string(r.name) + " θ=45 φ=78 Abs_dir(layer=1)",
                           r.gold.at_45_78.Abs1_dir,
                           vA,
                           vB,
                           tol);
        }
    }
}
