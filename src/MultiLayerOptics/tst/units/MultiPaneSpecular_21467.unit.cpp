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

class EquivalentSpecularLayer_21467 : public testing::Test
{
    std::shared_ptr<CMultiPaneSpecular> m_Layer;

protected:
    void SetUp() override
    {
        double thickness = 3.0e-3;   // [m]
        const auto aMaterial_21467 =
          Material::nBandMaterial(SpectralSample::NFRC_21467(), thickness, MaterialType::Monolithic);

        const auto layer102 = SpecularLayer::createLayer(aMaterial_21467);

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

TEST_F(EquivalentSpecularLayer_21467, TestAngle0)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 0 deg.");

    constexpr double angle = 0;
    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiPaneSpecular aLayer = *getLayer();

    const double T = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.85759475979163102, T, 1e-6);

    const double Rf = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.10882832466003842, Rf, 1e-6);

    const double Rb = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.11455238848477553, Rb, 1e-6);

    const double Abs1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.033577933335637043, Abs1, 1e-6);

    const double Them = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, Side::Front, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.7836942732385721, Them, 1e-6);

    const double Rfhem = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Front, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.16945613901722306, Rfhem, 1e-6);

    const double Rbhem = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, Side::Back, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.17473195757158716, Rbhem, 1e-6);
}

TEST_F(EquivalentSpecularLayer_21467, TestAngleHemispherical10)
{
    SCOPED_TRACE("Begin Test: Hemispherical to hemispherical with ten integration points.");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;
    std::vector<double> aAngles{0, 10, 20, 30, 40, 50, 60, 70, 80, 90};

    CMultiPaneSpecular aLayer = *getLayer();

    double Tfhem =
      aLayer.getHemisphericalProperty(Side::Front, Property::T, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.7836942732385721, Tfhem, 1e-6);

    double Tbhem =
      aLayer.getHemisphericalProperty(Side::Back, Property::T, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.7836942732385721, Tbhem, 1e-6);

    double Rfhem =
      aLayer.getHemisphericalProperty(Side::Front, Property::R, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.16945613901722306, Rfhem, 1e-6);

    double Rbhem =
      aLayer.getHemisphericalProperty(Side::Back, Property::R, aAngles, minLambda, maxLambda);
    EXPECT_NEAR(0.17473195757158716, Rbhem, 1e-6);

    double Abs1 = aLayer.AbsHemispherical(1, aAngles, minLambda, maxLambda, Side::Front);
    EXPECT_NEAR(0.036674993352446866, Abs1, 1e-6);
}
