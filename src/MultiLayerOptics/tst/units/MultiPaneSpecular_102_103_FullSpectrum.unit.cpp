#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>

#include "optical/standardData.hpp"
#include "optical/spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

// Example/test case on multlayer specular
// Difference from BSDF layer is that properties can be calculated at any custom angle

class MultiPaneSpecular_102_103_FullSpectrum : public testing::Test
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


        auto layer102 = SpecularLayer::createLayer(aMaterial_102);
        auto layer103 = SpecularLayer::createLayer(aMaterial_103);

        m_Layer = CMultiPaneSpecular::create({layer102, layer103});

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

TEST_F(MultiPaneSpecular_102_103_FullSpectrum, TestAngle0)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 0 deg.");

    constexpr double angle = 0;

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiPaneSpecular aLayer = *getLayer();

    const double T = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.65229527465721349, T, 1e-6);

    const double Rf = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.12473355299988206, Rf, 1e-6);

    const double Rb = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.11661434024905955, Rb, 1e-6);

    const std::vector<double> AbsorptanceFront{aLayer.getAbsorptanceLayers(
      minLambda, maxLambda, Side::Front, ScatteringSimple::Direct, angle, 0)};

    const double AbsFront1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.096067402665126034, AbsFront1, 1e-6);
    EXPECT_NEAR(0.096067402665126034, AbsorptanceFront[0], 1e-6);

    const double AbsFront2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.12690376967777861, AbsFront2, 1e-6);
    EXPECT_NEAR(0.12690376967777861, AbsorptanceFront[1], 1e-6);

    const std::vector<double> AbsorptanceBack{aLayer.getAbsorptanceLayers(
      minLambda, maxLambda, Side::Back, ScatteringSimple::Direct, angle, 0)};

    const double AbsBack1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Back, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.16714188545079209, AbsBack1, 1e-6);
    EXPECT_NEAR(0.16714188545079209, AbsorptanceBack[0], 1e-6);

    const double AbsBack2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, Side::Back, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.063948499642935183, AbsBack2, 1e-6);
    EXPECT_NEAR(0.063948499642935183, AbsorptanceBack[1], 1e-6);

    const double Them = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.55254043812883435, Them, 1e-6);

    const double Rfhem = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.20148222722498713, Rfhem, 1e-6);

    const double Rbhem = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.1875316037471744, Rbhem, 1e-6);
}

TEST_F(MultiPaneSpecular_102_103_FullSpectrum, TestAngle10)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 10 deg.");

    constexpr double angle = 10;

    CMultiPaneSpecular aLayer = *getLayer();

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    const double T = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.6512009408137408, T, 1e-6);

    const double Rf = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.12464131190038928, Rf, 1e-6);

    const double Rb = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.11648460372395195, Rb, 1e-6);

    const double Abs1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.096643889655207982, Abs1, 1e-6);

    const double Abs2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.12751385763066228, Abs2, 1e-6);
}

TEST_F(MultiPaneSpecular_102_103_FullSpectrum, TestAngle20)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 20 deg.");

    constexpr double angle = 20;

    CMultiPaneSpecular aLayer = *getLayer();

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    const double T = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.64750874044040407, T, 1e-6);

    const double Rf = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.12483246115747181, Rf, 1e-6);

    const double Rb = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.11653428886495588, Rb, 1e-6);

    const double Abs1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.098381037477157776, Abs1, 1e-6);

    const double Abs2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.12927776092496646, Abs2, 1e-6);
}

TEST_F(MultiPaneSpecular_102_103_FullSpectrum, TestAngle30)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 30 deg.");

    constexpr double angle = 30;

    CMultiPaneSpecular aLayer = *getLayer();

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    const double T = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.6397093404230586, T, 1e-6);

    const double Rf = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.12704776228943535, Rf, 1e-6);

    const double Rb = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.11839109688423785, Rb, 1e-6);

    const double Abs1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.10130303643061224, Abs1, 1e-6);

    const double Abs2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.13193986085689391, Abs2, 1e-6);
}

TEST_F(MultiPaneSpecular_102_103_FullSpectrum, TestAngle40)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 40 deg.");

    constexpr double angle = 40;

    CMultiPaneSpecular aLayer = *getLayer();

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    const double T = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.62414048337519745, T, 1e-6);

    const double Rf = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.13555493484076114, Rf, 1e-6);

    const double Rb = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.12602697935816962, Rb, 1e-6);

    const double Abs1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.10545945033456472, Abs1, 1e-6);

    const double Abs2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.13484513144947669, Abs2, 1e-6);
}

TEST_F(MultiPaneSpecular_102_103_FullSpectrum, TestAngleHemispherical10)
{
    SCOPED_TRACE("Begin Test: Hemispherical to hemispherical with ten integration points.");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;
    std::vector aAngles{0.0, 10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0};

    CMultiPaneSpecular aLayer = *getLayer();

    double Tfhem =
      aLayer.getHemisphericalProperty(Side::Front, Property::T, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.55254043812883435, Tfhem, 1e-6);

    double Tbhem =
      aLayer.getHemisphericalProperty(Side::Back, Property::T, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.55254043812883435, Tbhem, 1e-6);

    double Rfhem =
      aLayer.getHemisphericalProperty(Side::Front, Property::R, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.20148222722498713, Rfhem, 1e-6);

    double Rbhem =
      aLayer.getHemisphericalProperty(Side::Back, Property::R, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.1875316037471744, Rbhem, 1e-6);

    double Abs1 = aLayer.AbsHemispherical(1, aAngles, minLambda, maxLambda, Side::Front);
    EXPECT_NEAR(0.10892818665399433, Abs1, 1e-6);

    double Abs2 = aLayer.AbsHemispherical(2, aAngles, minLambda, maxLambda, Side::Front);
    EXPECT_NEAR(0.12687455360042596, Abs2, 1e-6);
}

TEST_F(MultiPaneSpecular_102_103_FullSpectrum, TestAngleHemispherical19)
{
    SCOPED_TRACE("Begin Test: Hemispherical to hemispherical with nineteen integration points.");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;
    std::vector<double> aAngles{
      0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90};

    CMultiPaneSpecular aLayer = *getLayer();

    double Tfhem =
      aLayer.getHemisphericalProperty(Side::Front, Property::T, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.55491389662002732, Tfhem, 1e-6);

    double Tbhem =
      aLayer.getHemisphericalProperty(Side::Back, Property::T, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.55491389662002732, Tbhem, 1e-6);

    double Rfhem =
      aLayer.getHemisphericalProperty(Side::Front, Property::R, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.20557297581589185, Rfhem, 1e-6);

    double Rbhem =
      aLayer.getHemisphericalProperty(Side::Back, Property::R, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.19154036259734128, Rbhem, 1e-6);

    const auto AbsFront1 = aLayer.AbsHemispherical(1, aAngles, minLambda, maxLambda, Side::Front);
    EXPECT_NEAR(0.10959216916044692, AbsFront1, 1e-6);

    const auto AbsFront2 = aLayer.AbsHemispherical(2, aAngles, minLambda, maxLambda, Side::Front);
    EXPECT_NEAR(0.12738119019535987, AbsFront2, 1e-6);

    const auto AbsBack1 = aLayer.AbsHemispherical(1, aAngles, minLambda, maxLambda, Side::Back);
    EXPECT_NEAR(0.18707512921364519, AbsBack1, 1e-6);

    const auto AbsBack2 = aLayer.AbsHemispherical(2, aAngles, minLambda, maxLambda, Side::Back);
    EXPECT_NEAR(0.063930843360712114, AbsBack2, 1e-6);
}

TEST_F(MultiPaneSpecular_102_103_FullSpectrum, TestHemispherical)
{
    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    auto aLayer = *getLayer();

    const std::vector absorptances{
      aLayer.getAbsorptanceLayers(minLambda, maxLambda, Side::Front, ScatteringSimple::Diffuse)};

    double Abs1 =
      aLayer.getAbsorptanceLayer(minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Diffuse);
    EXPECT_NEAR(0.10892818665399433, Abs1, 1e-6);
    EXPECT_NEAR(0.10892818665399433, absorptances[0], 1e-6);

    double Abs2 =
      aLayer.getAbsorptanceLayer(minLambda, maxLambda, 2, Side::Front, ScatteringSimple::Diffuse);
    EXPECT_NEAR(0.12687455360042596, Abs2, 1e-6);
    EXPECT_NEAR(0.12687455360042596, absorptances[1], 1e-6);
}
