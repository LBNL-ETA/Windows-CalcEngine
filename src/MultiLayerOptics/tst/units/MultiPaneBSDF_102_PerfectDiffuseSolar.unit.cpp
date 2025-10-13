#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>

#include "optical/standardData.hpp"
#include "optical/spectralSampleData.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

// Example on how to create multilayer BSDF from specular and venetian layers

class MultiPaneBSDF_102_PerfectDiffuseSolar : public testing::Test
{
private:
    std::unique_ptr<CMultiPaneBSDF> m_Layer;

protected:
    virtual void SetUp()
    {
        const auto commonWL{StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
        auto thickness = 3.048e-3;   // [m]
        auto aMaterial_102 = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);

        aMaterial_102->setBandWavelengths(commonWL);

        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Small);

        auto Layer_102 = CBSDFLayerMaker::getSpecularLayer(aMaterial_102, aBSDF);

        // Layer_102->setBandWavelengths(commonWL);

        // Solar range
        const auto Tsol = 0.5;
        const auto Rfsol = 0.3;
        const auto Rbsol = 0.3;

        // Visible range
        const auto Tvis = 0.4;
        const auto Rfvis = 0.2;
        const auto Rbvis = 0.2;

        auto aMaterial = SingleLayerOptics::Material::dualBandMaterial(
          Tsol, Tsol, Rfsol, Rbsol, Tvis, Tvis, Rfvis, Rbvis);
        aMaterial->setBandWavelengths(commonWL);

        const auto diffuseLayer = CBSDFLayerMaker::getPerfectlyDiffuseLayer(aMaterial, aBSDF);

        m_Layer = CMultiPaneBSDF::create({Layer_102, diffuseLayer});

        const CalculationProperties input{
          StandardData::solarRadiationASTM_E891_87_Table1(),
          StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
        m_Layer->setCalculationProperties(input);
    }

public:
    CMultiPaneBSDF & getLayer()
    {
        return *m_Layer;
    }
};

TEST_F(MultiPaneBSDF_102_PerfectDiffuseSolar, TestPerfectDiffuseBSDFSolar)
{
    SCOPED_TRACE("Begin Test: Perfectly diffusing IGU - BSDF.");

    const double minLambda = 0.3;
    const double maxLambda = 2.5;

    CMultiPaneBSDF & aLayer = getLayer();

    double tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::T);
    EXPECT_NEAR(0.386373, tauDiff, 1e-6);

    double rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::R);
    EXPECT_NEAR(0.318043, rhoDiff, 1e-6);

    double absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.127644, absDiff1, 1e-6);

    double absDiff2 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 2);
    EXPECT_NEAR(0.167941, absDiff2, 1e-6);

    double theta = 0;
    double phi = 0;

    double tauHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.430167, tauHem, 1e-6);

    double tauDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.005512, tauDir, 1e-6);

    double tauDirDiff =
      aLayer.DirDiff(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.424654, tauDirDiff, 1e-6);

    double rhoHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.263848, rhoHem, 1e-6);

    double rhoDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.077460, rhoDir, 1e-6);

    double abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.120879, abs1, 1e-6);

    double abs2 = aLayer.Abs(minLambda, maxLambda, Side::Front, 2, theta, phi);
    EXPECT_NEAR(0.185106, abs2, 1e-6);

    theta = 45;
    phi = 78;

    tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.421933, tauHem, 1e-6);

    tauDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.092840, tauDir, 1e-6);

    rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.266965, rhoHem, 1e-6);

    rhoDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.126101, rhoDir, 1e-6);

    abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.128341, abs1, 1e-6);

    abs2 = aLayer.Abs(minLambda, maxLambda, Side::Front, 2, theta, phi);
    EXPECT_NEAR(0.182761, abs2, 1e-6);
}
