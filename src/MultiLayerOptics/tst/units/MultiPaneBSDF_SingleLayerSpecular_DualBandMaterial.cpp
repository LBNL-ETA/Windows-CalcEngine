#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

#include "optical/standardData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

// Example on how to create multilayer BSDF from specular layers only

#include <fstream>


class MultiPaneBSDF_SingleLayerSpecular_DualBandMaterial : public testing::Test
{
    std::unique_ptr<CMultiPaneBSDF> m_Layer;


protected:
    void SetUp() override
    {
        constexpr auto tsol_f = 0.1;
        constexpr auto tsol_b = 0.11;
        constexpr auto rsol_f = 0.71;
        constexpr auto rsol_b = 0.72;
        constexpr auto tvis_f = 0.2;
        constexpr auto tvis_b = 0.21;
        constexpr auto rvis_b = 0.61;
        constexpr auto rvis_f = 0.62;
        const auto aBSDFMaterial = Material::dualBandMaterial(
          tsol_f, tsol_b, rsol_f, rsol_b, tvis_f, tvis_b, rvis_f, rvis_b);

        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);
        auto Layer_BSDF = CBSDFLayerMaker::getSpecularLayer(aBSDFMaterial, aBSDF);

        m_Layer = CMultiPaneBSDF::create({Layer_BSDF});
    }

public:
    CMultiPaneBSDF & getLayer()
    {
        return *m_Layer;
    }
};


TEST_F(MultiPaneBSDF_SingleLayerSpecular_DualBandMaterial, TestEquivalentProperties)
{
    SCOPED_TRACE("Begin Test: Specular layer - BSDF.");

    double minLambda;
    double maxLambda;
    double theta;
    double phi;
    double tauHem;
    double tauDir;
    double rhoFrontHem;
    double rhoBackHem;
    double rhoDir;
    double abs1;
    double rhoHem;
    double tauDiff;
    double rhoDiff;
    double absDiff1;

    CMultiPaneBSDF & aLayer = getLayer();

    // --- Solar range ---
    minLambda = 0.3;
    maxLambda = 2.5;
    theta = 0;
    phi = 0;

    const CalculationProperties input{StandardData::solarRadiationASTM_E891_87_Table1(),
                                      StandardData::condensedSpectrumDefault()};
    aLayer.setCalculationProperties(input);

    tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.107847, tauHem, 1e-6);

    tauDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.107847, tauDir, 1e-6);

    rhoFrontHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.702938, rhoFrontHem, 1e-6);

    rhoBackHem = aLayer.DirHem(minLambda, maxLambda, Side::Back, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.711368, rhoBackHem, 1e-6);

    rhoDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.702938, rhoDir, 1e-6);

    abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.189215, abs1, 1e-6);

    theta = 45;
    phi = 78;

    tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.107847, tauHem, 1e-6);

    rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.702938, rhoHem, 1e-6);

    abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.189215, abs1, 1e-6);

    // Diffuse values
    tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::T);
    EXPECT_NEAR(0.107847, tauDiff, 1e-6);

    rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::R);
    EXPECT_NEAR(0.702938, rhoDiff, 1e-6);

    absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.189215, absDiff1, 1e-6);

    // --- Visible range ---
    minLambda = 0.38;
    maxLambda = 0.78;
    theta = 0;
    phi = 0;

    const CalculationProperties inputVisible{StandardData::Photopic::solarRadiation(),
                                             StandardData::condensedSpectrumDefault(),
                                             StandardData::Photopic::detectorData()};
    aLayer.setCalculationProperties(inputVisible);

    tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.200000, tauHem, 1e-6);

    tauDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.200000, tauDir, 1e-6);

    rhoFrontHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.620000, rhoFrontHem, 1e-6);

    rhoBackHem = aLayer.DirHem(minLambda, maxLambda, Side::Back, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.610000, rhoBackHem, 1e-6);

    rhoDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.620000, rhoDir, 1e-6);

    abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.180000, abs1, 1e-6);

    theta = 45;
    phi = 78;

    tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.200000, tauHem, 1e-6);

    rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.620000, rhoHem, 1e-6);

    abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.180000, abs1, 1e-6);

    // Diffuse values
    tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::T);
    EXPECT_NEAR(0.200000, tauDiff, 1e-6);

    rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::R);
    EXPECT_NEAR(0.620000, rhoDiff, 1e-6);

    absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.180000, absDiff1, 1e-6);
}
