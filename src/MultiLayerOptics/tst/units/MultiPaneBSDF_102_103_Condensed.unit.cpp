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

// Example on how to create multilayer BSDF from specular layers only

class MultiPaneBSDF_102_103_Condensed : public testing::Test
{
public:
    std::unique_ptr<CMultiPaneBSDF> m_Layer;

protected:
    void SetUp() override
    {
        // Create material from samples
        auto thickness = 3.048e-3;   // [m]
        auto aMaterial_102 = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);
        thickness = 5.715e-3;   // [m]
        auto aMaterial_103 = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_103(), thickness, MaterialType::Monolithic);

        // BSDF definition is needed as well as its material representation
        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);
        auto Layer_102 = CBSDFLayerMaker::getSpecularLayer(aMaterial_102, aBSDF);
        auto Layer_103 = CBSDFLayerMaker::getSpecularLayer(aMaterial_103, aBSDF);

        m_Layer = CMultiPaneBSDF::create({Layer_102, Layer_103});
    }

public:
    CMultiPaneBSDF & getLayer()
    {
        return *m_Layer;
    };
};

TEST_F(MultiPaneBSDF_102_103_Condensed, SolarAndVisibleRange)
{
    CMultiPaneBSDF & aLayer = getLayer();

    // Solar range
    double minLambda = 0.3;
    double maxLambda = 2.5;

    const CalculationProperties input{
      StandardData::solarRadiationASTM_E891_87_Table1(),
      StandardData::condensedSpectrumDefault()};
    aLayer.setCalculationProperties(input);

    double tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySimple::T);
    EXPECT_NEAR(0.560895, tauDiff, 1e-6);

    double rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.225316, rhoDiff, 1e-6);

    double absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.098117, absDiff1, 1e-6);

    double absDiff2 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 2);
    EXPECT_NEAR(0.115671, absDiff2, 1e-6);

    double theta = 0;
    double phi = 0;

    double tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.672294, tauHem, 1e-6);

    double tauDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.672294, tauDir, 1e-6);

    double rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySimple::R, theta, phi);
    EXPECT_NEAR(0.127319, rhoHem, 1e-6);

    double rhoDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySimple::R, theta, phi);
    EXPECT_NEAR(0.127319, rhoDir, 1e-6);

    double abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.084550, abs1, 1e-6);

    double abs2 = aLayer.Abs(minLambda, maxLambda, Side::Front, 2, theta, phi);
    EXPECT_NEAR(0.115837, abs2, 1e-6);

    // Visible range

    minLambda = 0.38;
    maxLambda = 0.78;

    const CalculationProperties inputVisible{StandardData::Photopic::solarRadiation(),
                                             StandardData::condensedSpectrumDefault(),
                                             StandardData::Photopic::detectorData()};
    aLayer.setCalculationProperties(inputVisible);

    tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySimple::T);
    EXPECT_NEAR(0.685857, tauDiff, 1e-6);

    rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.257235, rhoDiff, 1e-6);

    absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.023364, absDiff1, 1e-6);

    absDiff2 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 2);
    EXPECT_NEAR(0.033544, absDiff2, 1e-6);

    theta = 0;
    phi = 0;

    tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.800858, tauHem, 1e-6);

    tauDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.800858, tauDir, 1e-6);

    rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySimple::R, theta, phi);
    EXPECT_NEAR(0.147612, rhoHem, 1e-6);

    rhoDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySimple::R, theta, phi);
    EXPECT_NEAR(0.147612, rhoDir, 1e-6);

    abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.019519, abs1, 1e-6);

    abs2 = aLayer.Abs(minLambda, maxLambda, Side::Front, 2, theta, phi);
    EXPECT_NEAR(0.032011, abs2, 1e-6);
}