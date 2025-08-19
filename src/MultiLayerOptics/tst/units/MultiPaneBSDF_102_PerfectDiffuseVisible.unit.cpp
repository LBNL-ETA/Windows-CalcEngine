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

// Example on how to create multilayer BSDF from specular and venetian layers

class MultiPaneBSDF_102_PerfectDiffuse_Visible : public testing::Test
{
    std::unique_ptr<CMultiPaneBSDF> m_Layer;

protected:
    virtual void SetUp()
    {
        constexpr auto thickness = 3.048e-3;   // [m]
        auto aMaterial_102 =
          Material::nBandMaterial(SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);

        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);

        auto Layer_102 = CBSDFLayerMaker::getSpecularLayer(aMaterial_102, aBSDF);

        // Solar range
        constexpr auto Tsol = 0.5;
        constexpr auto Rfsol = 0.3;
        constexpr auto Rbsol = 0.3;

        // Visible range
        constexpr auto Tvis = 0.4;
        constexpr auto Rfvis = 0.2;
        constexpr auto Rbvis = 0.2;

        auto aMaterial = SingleLayerOptics::Material::dualBandMaterial(
          Tsol, Tsol, Rfsol, Rbsol, Tvis, Tvis, Rfvis, Rbvis);

        auto diffuseLayer = CBSDFLayerMaker::getPerfectlyDiffuseLayer(aMaterial, aBSDF);

        m_Layer = CMultiPaneBSDF::create({Layer_102, diffuseLayer});

        const CalculationProperties input{StandardData::Photopic::solarRadiation(),
                                          StandardData::Photopic::solarRadiation().getXArray(),
                                          StandardData::Photopic::detectorData()};
        m_Layer->setCalculationProperties(input);
    }

public:
    [[nodiscard]] CMultiPaneBSDF & getLayer() const
    {
        return *m_Layer;
    }
};

TEST_F(MultiPaneBSDF_102_PerfectDiffuse_Visible, TestPerfectDiffuseBSDFVisible)
{
    SCOPED_TRACE("Begin Test: Perfectly diffusing IGU - BSDF.");

    constexpr double minLambda = 0.38;
    constexpr double maxLambda = 0.78;

    CMultiPaneBSDF & aLayer = getLayer();

    double tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySimple::T);
    EXPECT_NEAR(0.33657875345349131, tauDiff, 1e-6);

    double rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.30271422929099751, rhoDiff, 1e-6);

    double absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.024128263802016579, absDiff1, 1e-6);

    double absDiff2 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 2);
    EXPECT_NEAR(0.33657875345349303, absDiff2, 1e-6);

    double theta = 0;
    double phi = 0;

    double tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.37203670879226813, tauHem, 1e-6);

    double tauDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.0091045055680517902, tauDir, 1e-6);

    double tauDirDiff =
      aLayer.DirDiff(minLambda, maxLambda, Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.36293232249507862, tauDirDiff, 1e-6);

    double rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySimple::R, theta, phi);
    EXPECT_NEAR(0.23390789068239645, rhoHem, 1e-6);

    double rhoDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySimple::R, theta, phi);
    EXPECT_NEAR(0.08665605444634436, rhoDir, 1e-6);

    double abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.022018691733066909, abs1, 1e-6);

    double abs2 = aLayer.Abs(minLambda, maxLambda, Side::Front, 2, theta, phi);
    EXPECT_NEAR(0.3720367087922683, abs2, 1e-6);
}
