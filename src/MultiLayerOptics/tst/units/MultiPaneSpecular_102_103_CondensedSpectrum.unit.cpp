#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>

#include "standardData.hpp"
#include "spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

// Example/test case on multlayer specular
// Difference from BSDF layer is that properties can be calculated at any custom angle

class MultiPaneSpecular_102_103_CondensedSpectrum : public testing::Test
{
    std::shared_ptr<CMultiPaneSpecular> m_Layer;

protected:
    void SetUp() override
    {
        constexpr double thickness1 = 3.048e-3;   // [m]
        const auto aMaterial_102 =
          Material::nBandMaterial(SpectralSample::NFRC_102(), thickness1, MaterialType::Monolithic);

        constexpr auto thickness2 = 5.715e-3;   // [m]
        const auto aMaterial_103 =
          Material::nBandMaterial(SpectralSample::NFRC_103(), thickness2, MaterialType::Monolithic);

        auto layer102 = SpecularLayer::createLayer(aMaterial_102);
        auto layer103 = SpecularLayer::createLayer(aMaterial_103);

        m_Layer = CMultiPaneSpecular::create({layer102, layer103}, StandardData::condensedSpectrumDefault());

        const CalculationProperties input{
          StandardData::solarRadiationASTM_E891_87_Table1(),
          StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
        m_Layer->setCalculationProperties(input);
    }

public:
    [[nodiscard]] std::shared_ptr<CMultiPaneSpecular> getLayer() const
    {
        return m_Layer;
    }
};

TEST_F(MultiPaneSpecular_102_103_CondensedSpectrum, TestAngle0)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 0 deg.");

    constexpr double angle = 0;

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiPaneSpecular aLayer = *getLayer();

    const double T = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.65096737306210839, T, 1e-6);

    const double Rf = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.12437233463533325, Rf, 1e-6);

    const double Rb = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.11621576724154123, Rb, 1e-6);

    const std::vector AbsorptanceFront{aLayer.getAbsorptanceLayers(
      minLambda, maxLambda, Side::Front, ScatteringSimple::Direct, angle, 0)};

    const double AbsFront1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.09886828346187998, AbsFront1, 1e-6);
    EXPECT_NEAR(0.09886828346187998, AbsorptanceFront[0], 1e-6);

    const double AbsFront2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.12579200884067865, AbsFront2, 1e-6);
    EXPECT_NEAR(0.12579200884067865, AbsorptanceFront[1], 1e-6);

    const std::vector AbsorptanceBack{aLayer.getAbsorptanceLayers(
      minLambda, maxLambda, Side::Back, ScatteringSimple::Direct, angle, 0)};

    const double AbsBack1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Back, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.16896549433713057, AbsBack1, 1e-6);
    EXPECT_NEAR(0.16896549433713057, AbsorptanceBack[0], 1e-6);

    const double AbsBack2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, Side::Back, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.063851365359220133, AbsBack2, 1e-6);
    EXPECT_NEAR(0.063851365359220133, AbsorptanceBack[1], 1e-6);

    const double Them = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.55134962796067644, Them, 1e-6);

    const double Rfhem = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.20101014299215508, Rfhem, 1e-6);

    const double Rbhem = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.18703361935727306, Rbhem, 1e-6);
}
