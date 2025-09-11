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

class MultiPaneSpecular_6046 : public testing::Test
{
    std::shared_ptr<CMultiPaneSpecular> m_Layer;

protected:
    void SetUp() override
    {
        double thickness = 5.66e-3;   // [m]
        const auto aMaterial_6046 =
          Material::nBandMaterial(SpectralSample::NFRC_6046(), thickness, MaterialType::Monolithic);

        const auto layer102 = SpecularLayer::createLayer(aMaterial_6046);

        m_Layer = CMultiPaneSpecular::create({layer102});

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

TEST_F(MultiPaneSpecular_6046, TestAngle0)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 0 deg.");

    constexpr double angle = 0;

    CMultiPaneSpecular aLayer = *getLayer();

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    const double T = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.38299706431771591, T, 1e-6);

    const double Rf = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.28616972912819455, Rf, 1e-6);

    const double Rb = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.44953481057588385, Rb, 1e-6);

    const double Abs1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.33083320655408976, Abs1, 1e-6);

    const double Them = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.34930792564766677, Them, 1e-6);

    const double Rfhem = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.30355611406159649, Rfhem, 1e-6);

    const double Rbhem = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.45861965268430771, Rbhem, 1e-6);
}

TEST_F(MultiPaneSpecular_6046, TestAngle10)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 10 deg.");

    constexpr double angle = 10;

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiPaneSpecular aLayer = *getLayer();

    const double T = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.38250500788015518, T, 1e-6);

    const double Rf = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.28612567799481614, Rf, 1e-6);

    const double Rb = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.44949838906463818, Rb, 1e-6);

    const double Abs1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.33136931412502829, Abs1, 1e-6);
}

TEST_F(MultiPaneSpecular_6046, TestAngleHemispherical10)
{
    SCOPED_TRACE("Begin Test: Hemispherical to hemispherical with ten integration points.");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;
    std::vector<double> aAngles{0, 10, 20, 30, 40, 50, 60, 70, 80, 90};

    CMultiPaneSpecular aLayer = *getLayer();

    double Tfhem =
      aLayer.getHemisphericalProperty(Side::Front, Property::T, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.34930792564766677, Tfhem, 1e-6);

    double Tbhem =
      aLayer.getHemisphericalProperty(Side::Back, Property::T, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.34930792564766677, Tbhem, 1e-6);

    double Rfhem =
      aLayer.getHemisphericalProperty(Side::Front, Property::R, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.30355611406159649, Rfhem, 1e-6);

    double Rbhem =
      aLayer.getHemisphericalProperty(Side::Back, Property::R, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.45861965268430771, Rbhem, 1e-6);

    double Abs1 = aLayer.AbsHemispherical(1, aAngles, minLambda, maxLambda, Side::Front);
    EXPECT_NEAR(0.33696136589897857, Abs1, 1e-6);
}
