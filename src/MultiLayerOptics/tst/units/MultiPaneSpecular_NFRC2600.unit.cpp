#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

#include "standardData.hpp"
#include "spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

class MultiPaneSpecular_NFRC2600 : public testing::Test
{
    std::shared_ptr<CMultiPaneSpecular> m_Layer;

protected:
    void SetUp() override
    {
        double thickness = 2.4892e-3;   // [m]
        const auto aMaterial_2600 =
          Material::nBandMaterial(SpectralSample::NFRC_2600(), thickness, MaterialType::Monolithic);

        const auto layer2600 = SpecularLayer::createLayer(aMaterial_2600);

        m_Layer = CMultiPaneSpecular::create({layer2600});

        const CalculationProperties input{
          StandardData::solarRadiationASTM_E891_87_Table1(),
          StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
        m_Layer->setCalculationProperties(input);
    }

public:
    [[nodiscard]] std::shared_ptr<CMultiPaneSpecular> getLayer() const
    {
        return m_Layer;
    };
};

TEST_F(MultiPaneSpecular_NFRC2600, TestAngle0)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 0 deg.");

    constexpr double angle = 0;

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiPaneSpecular aLayer = *getLayer();

    const double T = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.85364295496596676, T, 1e-6);

    const double Rf = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.070590408730950835, Rf, 1e-6);

    const double Rb = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.070317441387705334, Rb, 1e-6);

    const double Abs1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.075766636303082432, Abs1, 1e-6);
}

TEST_F(MultiPaneSpecular_NFRC2600, TestAngle10)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 10 deg.");

    constexpr double angle = 10;

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiPaneSpecular aLayer = *getLayer();

    const double T = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.85339696288847755, T, 1e-6);

    const double Rf = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.070603265452464065, Rf, 1e-6);

    const double Rb = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.070330211466021975, Rb, 1e-6);

    const double Abs1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.075999771659058385, Abs1, 1e-6);
}

TEST_F(MultiPaneSpecular_NFRC2600, TestAngle20)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 20 deg.");

    constexpr double angle = 20;

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiPaneSpecular aLayer = *getLayer();

    const double T = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.852378, T, 1e-6);

    const double Rf = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.070942533307403133, Rf, 1e-6);

    const double Rb = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.070668784173020249, Rb, 1e-6);

    const double Abs1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.076680058161091746, Abs1, 1e-6);
}

TEST_F(MultiPaneSpecular_NFRC2600, TestAngleHemispherical10)
{
    SCOPED_TRACE("Begin Test: Hemispherical to hemispherical with ten integration points.");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;
    const std::vector<double> aAngles{0, 10, 20, 30, 40, 50, 60, 70, 80, 90};

    CMultiPaneSpecular aLayer = *getLayer();

    double Tfhem =
      aLayer.getHemisphericalProperty(Side::Front, Property::T, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.77845552949593655, Tfhem, 1e-6);

    double Tbhem =
      aLayer.getHemisphericalProperty(Side::Back, Property::T, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.77845552949593655, Tbhem, 1e-6);

    double Rfhem =
      aLayer.getHemisphericalProperty(Side::Front, Property::R, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.13474572707890642, Rfhem, 1e-6);

    double Rbhem =
      aLayer.getHemisphericalProperty(Side::Back, Property::R, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.13445032501853083, Rbhem, 1e-6);

    double Abs1 = aLayer.AbsHemispherical(1, aAngles, minLambda, maxLambda, Side::Front);
    EXPECT_NEAR(0.076624149033398592, Abs1, 1e-6);
}
