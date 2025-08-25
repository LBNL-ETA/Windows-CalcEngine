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

// This test is created simply to keep comparison between two different models that are using same
// measurement sample

class MultiPaneBSDF_102_Specular_vs_DirDiff : public testing::Test
{
    static std::unique_ptr<CMultiPaneBSDF> createSpecularLayer()
    {
        auto thickness = 3.048e-3;
        auto aMaterial_102 = Material::nBandMaterial(
            SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);
        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);
        auto Layer_102_Specular = CBSDFLayerMaker::getSpecularLayer(aMaterial_102, aBSDF);
        return CMultiPaneBSDF::create({Layer_102_Specular});
    }

    static std::unique_ptr<CMultiPaneBSDF> createDirDifLayer()
    {
        auto thickness = 3.048e-3;
        auto aMaterial_102 = Material::nBandMaterial(
            SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);
        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);
        auto Layer_102_DirDif = CBSDFLayerMaker::getDirDifLayer(aMaterial_102, aBSDF);
        return CMultiPaneBSDF::create({Layer_102_DirDif});
    }

public:
    static CMultiPaneBSDF & getLayerSpecular()
    {
        static std::unique_ptr<CMultiPaneBSDF> layer = createSpecularLayer();
        return *layer;
    }

    static CMultiPaneBSDF & getLayerDirDif()
    {
        static std::unique_ptr<CMultiPaneBSDF> layer = createDirDifLayer();
        return *layer;
    }
};

TEST_F(MultiPaneBSDF_102_Specular_vs_DirDiff, SolarAndVisibleRange_Specular)
{
    CMultiPaneBSDF & aLayer = getLayerSpecular();

    // Solar range
    double minLambda = 0.3;
    double maxLambda = 2.5;

    const CalculationProperties input{
      StandardData::solarRadiationASTM_E891_87_Table1(),
      StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
    aLayer.setCalculationProperties(input);

    auto T = aLayer.getMatrix(minLambda, maxLambda, Side::Front, PropertySurface::T);

    double tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::T);
    EXPECT_NEAR(0.745404, tauDiff, 1e-6);

    double rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::R);
    EXPECT_NEAR(0.152995, rhoDiff, 1e-6);

    double absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.101601, absDiff1, 1e-6);

    double theta = 0;
    double phi = 0;

    double tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.833843, tauHem, 1e-6);

    double tauDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.833843, tauDir, 1e-6);

    double rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.074761, rhoHem, 1e-6);

    double rhoDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.074761, rhoDir, 1e-6);

    double abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.091396, abs1, 1e-6);

    theta = 45;
    phi = 78;

    tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.822262, tauHem, 1e-6);

    rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.079326, rhoHem, 1e-6);

    abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.098413, abs1, 1e-6);

    // Visible range

    minLambda = 0.38;
    maxLambda = 0.78;

    const CalculationProperties inputVisible{StandardData::Photopic::solarRadiation(),
                                             StandardData::Photopic::wavelengthSetPhotopic(),
                                             StandardData::Photopic::detectorData()};
    aLayer.setCalculationProperties(inputVisible);

    tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::T);
    EXPECT_NEAR(0.813555, tauDiff, 1e-6);

    rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::R);
    EXPECT_NEAR(0.165793, rhoDiff, 1e-6);

    absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.020652, absDiff1, 1e-6);

    theta = 0;
    phi = 0;

    tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.899260, tauHem, 1e-6);

    tauDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.899260, tauDir, 1e-6);

    rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.082563, rhoHem, 1e-6);

    rhoDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.082563, rhoDir, 1e-6);

    abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.018177, abs1, 1e-6);

    theta = 45;
    phi = 78;

    tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.892556, tauHem, 1e-6);

    rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.087775, rhoHem, 1e-6);

    abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.019669, abs1, 1e-6);
}

TEST_F(MultiPaneBSDF_102_Specular_vs_DirDiff, SolarAndVisibleRange_DirDif)
{
    CMultiPaneBSDF & aLayer = getLayerDirDif();

    // Solar range
    double minLambda = 0.3;
    double maxLambda = 2.5;

    const CalculationProperties input{
      StandardData::solarRadiationASTM_E891_87_Table1(),
      StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
    aLayer.setCalculationProperties(input);

    auto T = aLayer.getMatrix(minLambda, maxLambda, Side::Front, PropertySurface::T);

    double tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::T);
    EXPECT_NEAR(0.745404, tauDiff, 1e-6);

    double rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::R);
    EXPECT_NEAR(0.152995, rhoDiff, 1e-6);

    double absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.101601, absDiff1, 1e-6);

    double theta = 0;
    double phi = 0;

    double tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.833843, tauHem, 1e-6);

    double tauDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.833843, tauDir, 1e-6);

    double rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.074761, rhoHem, 1e-6);

    double rhoDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.074761, rhoDir, 1e-6);

    double abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.091396, abs1, 1e-6);

    theta = 45;
    phi = 78;

    tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.822262, tauHem, 1e-6);

    rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.079326, rhoHem, 1e-6);

    abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.098413, abs1, 1e-6);

    // Visible range

    minLambda = 0.38;
    maxLambda = 0.78;

    const CalculationProperties inputVisible{StandardData::Photopic::solarRadiation(),
                                             StandardData::Photopic::wavelengthSetPhotopic(),
                                             StandardData::Photopic::detectorData()};
    aLayer.setCalculationProperties(inputVisible);

    tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::T);
    EXPECT_NEAR(0.813555, tauDiff, 1e-6);

    rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::R);
    EXPECT_NEAR(0.165793, rhoDiff, 1e-6);

    absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.020652, absDiff1, 1e-6);

    theta = 0;
    phi = 0;

    tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.899260, tauHem, 1e-6);

    tauDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.899260, tauDir, 1e-6);

    rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.082563, rhoHem, 1e-6);

    rhoDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.082563, rhoDir, 1e-6);

    abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.018177, abs1, 1e-6);

    theta = 45;
    phi = 78;

    tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.892556, tauHem, 1e-6);

    rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.087775, rhoHem, 1e-6);

    abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.019669, abs1, 1e-6);
}