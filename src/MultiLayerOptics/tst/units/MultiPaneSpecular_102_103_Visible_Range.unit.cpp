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

// Example/test case on multlayer specular
// Difference from BSDF layer is that properties can be calculated at any custom angle

class EquivalentSpecularLayer_102_103_Visible_Range : public testing::Test
{
    std::shared_ptr<CMultiPaneSpecular> m_Layer;

protected:
    void SetUp() override
    {
        double thickness = 3.048e-3;   // [m]
        const auto aMaterial_102 =
          Material::nBandMaterial(SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);

        thickness = 5.715e-3;   // [m]
        const auto aMaterial_103 =
          Material::nBandMaterial(SpectralSample::NFRC_103(), thickness, MaterialType::Monolithic);

        const auto layer102 = SpecularLayer::createLayer(aMaterial_102);
        const auto layer103 = SpecularLayer::createLayer(aMaterial_103);

        m_Layer = CMultiPaneSpecular::create({layer102, layer103});

        const CalculationProperties input{StandardData::Photopic::solarRadiation(),
                                          StandardData::Photopic::wavelengthSetPhotopic(),
                                          StandardData::Photopic::detectorData()};
        m_Layer->setCalculationProperties(input);
    }

public:
    [[nodiscard]] std::shared_ptr<CMultiPaneSpecular> getLayer() const
    {
        return m_Layer;
    }
};
TEST_F(EquivalentSpecularLayer_102_103_Visible_Range, TestAngle0)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 0 deg.");

    constexpr double angle = 0;

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiPaneSpecular aLayer = *getLayer();

    const double T = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.80004116239060297, T, 1e-6);

    const double Rf = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.148036, Rf, 1e-6);

    const double Rb = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.145341, Rb, 1e-6);

    const double Abs1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.019482417027709444, Abs1, 1e-6);

    const double Abs2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.032440156277910373, Abs2, 1e-6);

    const double Them = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.69728960814264751, Them, 1e-6);

    const double Rfhem = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.235641, Rfhem, 1e-6);

    const double Rbhem = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.230712, Rbhem, 1e-6);
}

TEST_F(EquivalentSpecularLayer_102_103_Visible_Range, TestAngle10)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 10 deg.");

    constexpr double angle = 10;
    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiPaneSpecular aLayer = *getLayer();

    const double T = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.79971340390661316, T, 1e-6);

    const double Rf = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.148037, Rf, 1e-6);

    const double Rb = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.145324, Rb, 1e-6);

    const double Abs1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.019607658230143151, Abs1, 1e-6);

    const double Abs2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.032641877809576232, Abs2, 1e-6);
}

TEST_F(EquivalentSpecularLayer_102_103_Visible_Range, TestAngleHemispherical10)
{
    SCOPED_TRACE("Begin Test: Hemispherical to hemispherical with ten integration points.");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;
    std::vector<double> aAngles{0, 10, 20, 30, 40, 50, 60, 70, 80, 90};

    CMultiPaneSpecular aLayer = *getLayer();

    double Tfhem =
      aLayer.getHemisphericalProperty(Side::Front, Property::T, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.69728960814264751, Tfhem, 1e-6);

    double Tbhem =
      aLayer.getHemisphericalProperty(Side::Back, Property::T, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.69728960814264751, Tbhem, 1e-6);

    double Rfhem =
      aLayer.getHemisphericalProperty(Side::Front, Property::R, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.235641, Rfhem, 1e-6);

    double Rbhem =
      aLayer.getHemisphericalProperty(Side::Back, Property::R, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.230712, Rbhem, 1e-6);

    double Abs1 = aLayer.AbsHemispherical(1, aAngles, minLambda, maxLambda, Side::Front);
    EXPECT_NEAR(0.022818163375237097, Abs1, 1e-6);

    double Abs2 = aLayer.AbsHemispherical(2, aAngles, minLambda, maxLambda, Side::Front);
    EXPECT_NEAR(0.034076425843029154, Abs2, 1e-6);
}
