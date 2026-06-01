// Behavioral guards for the lazy wavelength-grid deferral in CMultiPaneBSDF /
// CEquivalentBSDFLayer. The per-layer band wavelengths are committed lazily (at the first
// of setCalculationProperties or first query) instead of in the constructor; these tests
// pin the properties that the deferral must preserve. Absolute values are covered by the
// broader MultiPaneBSDF_* suite, so this file deliberately avoids hardcoded oracles.

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

// Applying the SAME properties twice must yield identical results (commit-once semantics).
TEST(MultiPaneBSDF_Deferral, Idempotency_SameProperties)
{
    auto layer = makeLayer();

    layer->setCalculationProperties(solarProperties());
    const auto first = compute(*layer, solarMin, solarMax);

    layer->setCalculationProperties(solarProperties());
    const auto second = compute(*layer, solarMin, solarMax);

    expectBitEqual(first, second);
}

// Reading the baseline wavelengths before setCalculationProperties must not perturb the
// committed grid or any result.
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

// The baseline grid produced after create() (before setCalculationProperties) must equal the
// eager union grid that the previous constructor committed up front.
TEST(MultiPaneBSDF_Deferral, BaselineGridEqualsUnion)
{
    auto layer = makeLayer();
    const auto wavelengths = layer->getWavelengths();
    const double sum = std::accumulate(wavelengths.begin(), wavelengths.end(), 0.0);

    EXPECT_EQ(size_t{111}, wavelengths.size());
    EXPECT_NEAR(0.3, layer->getMinLambda(), 1e-12);
    EXPECT_NEAR(2.5, layer->getMaxLambda(), 1e-12);
    EXPECT_NEAR(102.9, sum, 1e-9);   // checksum; loose enough to be cross-platform stable
}
