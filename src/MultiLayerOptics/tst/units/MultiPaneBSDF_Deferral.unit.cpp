// Characterization + equivalence tests for the lazy wavelength-grid deferral
// refactor of CMultiPaneBSDF / CEquivalentBSDFLayer.
//
// These are written and recorded against the UNMODIFIED (eager) source first, so
// that the deferral refactor (which only reorders WHEN work runs) can be proven
// numerically identical. Comparisons use EXPECT_DOUBLE_EQ where a value must be
// bit-for-bit stable.

#include <array>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <vector>

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

namespace
{
    constexpr double solarMin = 0.3;
    constexpr double solarMax = 2.5;
    constexpr double visMin = 0.38;
    constexpr double visMax = 0.78;

    // A compact but sensitive basket of integrated outputs.
    struct Basket
    {
        double tDiffT;
        double tDiffR;
        double bDiffT;
        double dirHemTF;
        double dirDirTF;
        double absDiff1;
        double absDiff2;
    };

    [[nodiscard]] std::unique_ptr<CMultiPaneBSDF> makeLayer()
    {
        const auto material102 = Material::nBandMaterial(
          SpectralSample::NFRC_102(), 3.048e-3, MaterialType::Monolithic);
        const auto material103 = Material::nBandMaterial(
          SpectralSample::NFRC_103(), 5.715e-3, MaterialType::Monolithic);

        const auto bsdf = BSDFHemisphere::create(BSDFBasis::Quarter);
        const auto layer102 = CBSDFLayerMaker::getSpecularLayer(material102, bsdf);
        const auto layer103 = CBSDFLayerMaker::getSpecularLayer(material103, bsdf);

        return CMultiPaneBSDF::create({layer102, layer103});
    }

    [[nodiscard]] CalculationProperties solarProperties()
    {
        return CalculationProperties{StandardData::solarRadiationASTM_E891_87_Table1(),
                                     StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
    }

    [[nodiscard]] CalculationProperties visibleProperties()
    {
        return CalculationProperties{StandardData::Photopic::solarRadiation(),
                                     StandardData::Photopic::wavelengthSetPhotopic(),
                                     StandardData::Photopic::detectorData()};
    }

    [[nodiscard]] Basket compute(CMultiPaneBSDF & layer, const double minL, const double maxL)
    {
        return Basket{
          .tDiffT = layer.DiffDiff(minL, maxL, Side::Front, PropertySurface::T),
          .tDiffR = layer.DiffDiff(minL, maxL, Side::Front, PropertySurface::R),
          .bDiffT = layer.DiffDiff(minL, maxL, Side::Back, PropertySurface::T),
          .dirHemTF = layer.DirHem(minL, maxL, Side::Front, PropertySurface::T, 0.0, 0.0),
          .dirDirTF = layer.DirDir(minL, maxL, Side::Front, PropertySurface::T, 0.0, 0.0),
          .absDiff1 = layer.AbsDiff(minL, maxL, Side::Front, 1),
          .absDiff2 = layer.AbsDiff(minL, maxL, Side::Front, 2)};
    }

    void expectBitEqual(const Basket & lhs, const Basket & rhs)
    {
        EXPECT_DOUBLE_EQ(lhs.tDiffT, rhs.tDiffT);
        EXPECT_DOUBLE_EQ(lhs.tDiffR, rhs.tDiffR);
        EXPECT_DOUBLE_EQ(lhs.bDiffT, rhs.bDiffT);
        EXPECT_DOUBLE_EQ(lhs.dirHemTF, rhs.dirHemTF);
        EXPECT_DOUBLE_EQ(lhs.dirDirTF, rhs.dirDirTF);
        EXPECT_DOUBLE_EQ(lhs.absDiff1, rhs.absDiff1);
        EXPECT_DOUBLE_EQ(lhs.absDiff2, rhs.absDiff2);
    }
}   // namespace

// Test B - Idempotency: applying the SAME properties twice yields identical results.
TEST(MultiPaneBSDF_Deferral, Idempotency_SameProperties)
{
    auto layer = makeLayer();

    layer->setCalculationProperties(solarProperties());
    const auto first = compute(*layer, solarMin, solarMax);

    layer->setCalculationProperties(solarProperties());
    const auto second = compute(*layer, solarMin, solarMax);

    expectBitEqual(first, second);
}

// Test C - Reading baseline wavelengths BEFORE setCalculationProperties must not
// perturb the committed grid or any result.
TEST(MultiPaneBSDF_Deferral, BaselineReadDoesNotPerturb)
{
    auto plain = makeLayer();
    plain->setCalculationProperties(solarProperties());
    const auto expected = compute(*plain, solarMin, solarMax);

    auto probed = makeLayer();
    // Touch the baseline-grid getters before any properties are set.
    const auto wavelengths = probed->getWavelengths();
    std::ignore = probed->getMinLambda();
    std::ignore = probed->getMaxLambda();
    ASSERT_FALSE(wavelengths.empty());

    probed->setCalculationProperties(solarProperties());
    const auto actual = compute(*probed, solarMin, solarMax);

    expectBitEqual(expected, actual);
}

// Test A - Regression oracle (captured from unmodified eager source). The deferral
// refactor must reproduce these bit-for-bit.
TEST(MultiPaneBSDF_Deferral, RegressionOracle_Solar)
{
    auto layer = makeLayer();
    layer->setCalculationProperties(solarProperties());
    const auto got = compute(*layer, solarMin, solarMax);

    EXPECT_DOUBLE_EQ(0.5423407232935703, got.tDiffT);
    EXPECT_DOUBLE_EQ(0.2212313017711566, got.tDiffR);
    EXPECT_DOUBLE_EQ(0.54239486591382513, got.bDiffT);
    EXPECT_DOUBLE_EQ(0.65229527465721326, got.dirHemTF);
    EXPECT_DOUBLE_EQ(0.65229527465721326, got.dirDirTF);
    EXPECT_DOUBLE_EQ(0.11065150646956451, got.absDiff1);
    EXPECT_DOUBLE_EQ(0.1257764684657087, got.absDiff2);
}

TEST(MultiPaneBSDF_Deferral, RegressionOracle_Visible)
{
    auto layer = makeLayer();
    layer->setCalculationProperties(visibleProperties());
    const auto got = compute(*layer, visMin, visMax);

    EXPECT_DOUBLE_EQ(0.68510758740568278, got.tDiffT);
    EXPECT_DOUBLE_EQ(0.25757120302873093, got.tDiffR);
    EXPECT_DOUBLE_EQ(0.68510764943807201, got.bDiffT);
    EXPECT_DOUBLE_EQ(0.80004021252460955, got.dirHemTF);
    EXPECT_DOUBLE_EQ(0.80004021252460955, got.dirDirTF);
    EXPECT_DOUBLE_EQ(0.023322898494070075, got.absDiff1);
    EXPECT_DOUBLE_EQ(0.033998311071516289, got.absDiff2);
}

// Test D - The baseline grid produced after create() (before setCalculationProperties)
// must equal the eager union grid. Recorded as oracle (size + endpoints + sum).
TEST(MultiPaneBSDF_Deferral, BaselineGridEqualsUnion)
{
    auto layer = makeLayer();
    const auto wavelengths = layer->getWavelengths();
    const double sum = std::accumulate(wavelengths.begin(), wavelengths.end(), 0.0);

    EXPECT_EQ(size_t{111}, wavelengths.size());
    EXPECT_DOUBLE_EQ(0.3, layer->getMinLambda());
    EXPECT_DOUBLE_EQ(2.5, layer->getMaxLambda());
    EXPECT_DOUBLE_EQ(102.90000000000001, sum);
}

// Helper test (not an assertion) - dumps the oracle values at full precision so the
// constants above can be filled in. Run with --gtest_also_run_disabled_tests.
TEST(MultiPaneBSDF_Deferral, DISABLED_DumpOracle)
{
    auto solar = makeLayer();
    solar->setCalculationProperties(solarProperties());
    const auto s = compute(*solar, solarMin, solarMax);

    auto vis = makeLayer();
    vis->setCalculationProperties(visibleProperties());
    const auto v = compute(*vis, visMin, visMax);

    auto grid = makeLayer();
    const auto wavelengths = grid->getWavelengths();
    const double gridSum = std::accumulate(wavelengths.begin(), wavelengths.end(), 0.0);

    std::cout << std::setprecision(17);
    std::cout << "\n=== ORACLE DUMP ===\n";
    std::cout << "solar_tDiffT   " << s.tDiffT << "\n";
    std::cout << "solar_tDiffR   " << s.tDiffR << "\n";
    std::cout << "solar_bDiffT   " << s.bDiffT << "\n";
    std::cout << "solar_dirHemTF " << s.dirHemTF << "\n";
    std::cout << "solar_dirDirTF " << s.dirDirTF << "\n";
    std::cout << "solar_absDiff1 " << s.absDiff1 << "\n";
    std::cout << "solar_absDiff2 " << s.absDiff2 << "\n";
    std::cout << "vis_tDiffT     " << v.tDiffT << "\n";
    std::cout << "vis_tDiffR     " << v.tDiffR << "\n";
    std::cout << "vis_bDiffT     " << v.bDiffT << "\n";
    std::cout << "vis_dirHemTF   " << v.dirHemTF << "\n";
    std::cout << "vis_dirDirTF   " << v.dirDirTF << "\n";
    std::cout << "vis_absDiff1   " << v.absDiff1 << "\n";
    std::cout << "vis_absDiff2   " << v.absDiff2 << "\n";
    std::cout << "grid_size      " << wavelengths.size() << "\n";
    std::cout << "grid_front     " << grid->getMinLambda() << "\n";
    std::cout << "grid_back      " << grid->getMaxLambda() << "\n";
    std::cout << "grid_sum       " << gridSum << "\n";
    std::cout << "=== END ORACLE DUMP ===\n";
}
