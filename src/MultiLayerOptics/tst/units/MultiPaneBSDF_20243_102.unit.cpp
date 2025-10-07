#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

#include "../../../helper/optical/standardData.hpp"
#include "../../../helper/optical/spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

// Example on how to create multilayer BSDF from specular layers only

class MultiPaneBSDF_20243_103 : public testing::Test
{
    std::unique_ptr<CMultiPaneBSDF> m_Layer;

protected:
    void SetUp() override
    {
        // Create material from samples
        auto thickness = 3.048e-3;   // [m]
        auto aMaterial_102 =
          Material::nBandMaterial(SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);
        thickness = 5.715e-3;   // [m]
        auto aMaterial_20243 = Material::nBandMaterial(
          SpectralSample::NFRC_20243(), thickness, MaterialType::Monolithic);

        // BSDF definition is needed as well as its material representation
        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);
        auto Layer_102 = CBSDFLayerMaker::getSpecularLayer(aMaterial_102, aBSDF);
        auto Layer_20243 = CBSDFLayerMaker::getSpecularLayer(aMaterial_20243, aBSDF);

        m_Layer = CMultiPaneBSDF::create({Layer_20243, Layer_102});

        const CalculationProperties input{
          StandardData::solarRadiationASTM_E891_87_Table1(),
          StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
        m_Layer->setCalculationProperties(input);
    }

public:
    CMultiPaneBSDF & getLayer() const
    {
        return *m_Layer;
    };
};

TEST_F(MultiPaneBSDF_20243_103, TestSpecular1)
{
    SCOPED_TRACE("Begin Test: Specular layer - BSDF.");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiPaneBSDF & aLayer = getLayer();

    double tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::T);
    EXPECT_NEAR(0.00021851943605472038, tauDiff, 1e-6);

    double rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::R);
    EXPECT_NEAR(0.17938334436247161, rhoDiff, 1e-6);

    double absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.82037742417864912, absDiff1, 1e-6);

    double absDiff2 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 2);
    EXPECT_NEAR(0.0000207, absDiff2, 1e-6);

    double theta = 0;
    double phi = 0;

    double tauHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.00043396730130541584, tauHem, 1e-6);

    double tauDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.00043396730130541584, tauDir, 1e-6);

    double rhoHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.13176923275670094, rhoHem, 1e-6);

    double rhoDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.13176923275670094, rhoDir, 1e-6);

    double abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.86776218584464693, abs1, 1e-6);

    double abs2 = aLayer.Abs(minLambda, maxLambda, Side::Front, 2, theta, phi);
    EXPECT_NEAR(0.000035, abs2, 1e-6);

    theta = 45;
    phi = 78;

    tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.00027458805047742208, tauHem, 1e-6);

    rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.13464311997341968, rhoHem, 1e-6);

    abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.86505762485949012, abs1, 1e-6);

    abs2 = aLayer.Abs(minLambda, maxLambda, Side::Front, 2, theta, phi);
    EXPECT_NEAR(0.000025, abs2, 1e-6);
}
