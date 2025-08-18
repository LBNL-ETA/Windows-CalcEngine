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

class MultiPaneSpecular_1042_103_FirstLayerFlipped : public testing::Test
{
    std::shared_ptr<CMultiPaneSpecular> m_Layer;

protected:
    void SetUp() override
    {
        double thickness = 3.18e-3;   // [m]
        const auto aMaterial_1042 =
          Material::nBandMaterial(SpectralSample::NFRC_1042(), thickness, MaterialType::Coated);

        thickness = 5.715e-3;   // [m]
        const auto aMaterial_103 =
          Material::nBandMaterial(SpectralSample::NFRC_103(), thickness, MaterialType::Monolithic);

        auto layer1042 = SpecularLayer::createLayer(aMaterial_1042);
        layer1042->Flipped(true);
        const auto layer103 = SpecularLayer::createLayer(aMaterial_103);

        m_Layer = CMultiPaneSpecular::create({layer1042, layer103});

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

TEST_F(MultiPaneSpecular_1042_103_FirstLayerFlipped, TestAngle0)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 0 deg.");

    constexpr double angle = 0;

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiPaneSpecular aLayer = *getLayer();

    const double T = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.3719583755488084, T, 1e-6);

    const double Rf = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.41715457981977144, Rf, 1e-6);

    const double Rb = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.28037294048520067, Rb, 1e-6);

    const double Abs1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.15627839622413103, Abs1, 1e-6);

    const double Abs2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.054608648407289261, Abs2, 1e-6);

    const double Them = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, Side::Front, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.30817022451966275, Them, 1e-6);

    const double Rfhem = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, Side::Front, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.44942446269438591, Rfhem, 1e-6);

    const double Rbhem = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, Side::Back, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.31592238140198325, Rbhem, 1e-6);
}

TEST_F(MultiPaneSpecular_1042_103_FirstLayerFlipped, TestAngle40)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 40 deg.");

    constexpr double angle = 40;

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiPaneSpecular aLayer = *getLayer();

    const double T = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.34978043617677002, T, 1e-6);

    const double Rf = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.42031579031787092, Rf, 1e-6);

    const double Rb = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.27778429648957981, Rb, 1e-6);

    const double Abs1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.1726805764239672, Abs1, 1e-6);

    const double Abs2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.057223197081391772, Abs2, 1e-6);
}
