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

// Example/test case on multilayer specular
// Difference from BSDF layer is that properties can be calculated at any custom angle

class MultiPaneSpecular_2191_103_FullSpectrum : public testing::Test
{
    std::shared_ptr<CMultiPaneSpecular> m_Layer;

protected:
    void SetUp() override
    {
        double thickness = 3.048e-3;   // [m]
        const auto aMaterial_102 =
          Material::nBandMaterial(SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);

        thickness = 3e-3;   // [m]
        const auto aMaterial_2191 =
          Material::nBandMaterial(SpectralSample::NFRC_2191(), thickness, MaterialType::Monolithic);


        auto layer102 = SpecularLayer::createLayer(aMaterial_102);
        auto layer2191 = SpecularLayer::createLayer(aMaterial_2191);
        layer2191->Flipped(true);

        m_Layer = CMultiPaneSpecular::create({layer2191, layer102});

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

TEST_F(MultiPaneSpecular_2191_103_FullSpectrum, TestAngle0)
{
    SCOPED_TRACE("Begin Test: Specular MultiLayerOptics layer - angle = 0 deg.");

    constexpr double angle = 0;

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiPaneSpecular aLayer = *getLayer();

    const double T = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.5862113966042467, T, 1e-6);

    const double Rf = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, Side::Front, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.23011267434056742, Rf, 1e-6);

    const double Rb = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, Side::Back, Scattering::DirectDirect, angle, 0);
    EXPECT_NEAR(0.1903911215406871, Rb, 1e-6);

    const std::vector<double> AbsorptanceFront{aLayer.getAbsorptanceLayers(
      minLambda, maxLambda, Side::Front, ScatteringSimple::Direct, angle, 0)};

    const double AbsFront1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.12766749504232652, AbsFront1, 1e-6);
    EXPECT_NEAR(0.12766749504232652, AbsorptanceFront[0], 1e-6);

    const double AbsFront2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, Side::Front, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.056008434012859425, AbsFront2, 1e-6);
    EXPECT_NEAR(0.056008434012859425, AbsorptanceFront[1], 1e-6);

    const std::vector<double> AbsorptanceBack{aLayer.getAbsorptanceLayers(
      minLambda, maxLambda, Side::Back, ScatteringSimple::Direct, angle, 0)};

    const double AbsBack1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, Side::Back, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.10474173607311184, AbsBack1, 1e-6);
    EXPECT_NEAR(0.10474173607311184, AbsorptanceBack[0], 1e-6);

    const double AbsBack2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, Side::Back, ScatteringSimple::Direct, angle, 0);
    EXPECT_NEAR(0.1186557457819544, AbsBack2, 1e-6);
    EXPECT_NEAR(0.1186557457819544, AbsorptanceBack[1], 1e-6);

    const double Them = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, Side::Front, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.5092302931977486, Them, 1e-6);

    const double Rfhem = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, Side::Front, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.28196889594945301, Rfhem, 1e-6);

    const double Rbhem = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, Side::Back, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.25153667149679543, Rbhem, 1e-6);
}
