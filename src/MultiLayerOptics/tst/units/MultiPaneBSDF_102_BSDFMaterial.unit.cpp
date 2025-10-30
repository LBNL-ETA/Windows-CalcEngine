#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

#include "optical/standardData.hpp"
#include "optical/spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

// Example on how to create multilayer BSDF from specular layers only

#include <fstream>


class MultiPaneBSDF_102_BSDFMaterial : public testing::Test
{
    std::unique_ptr<CMultiPaneBSDF> m_Layer;

    std::vector<std::vector<double>> loadTf()
    {
        std::vector<std::vector<double>> data{
          {2.033760, 0.022174, 0.022174, 0.022174, 0.022174, 0.022174, 0.022174},
          {0.022223, 0.022223, 0.022223, 0.022223, 0.022223, 0.022223, 0.022223},
          {0.022223, 0.022223, 0.022223, 0.022223, 0.022223, 0.022223, 0.022223},
          {0.022461, 0.022461, 0.022461, 0.022461, 0.022461, 0.022461, 0.022461},
          {0.022461, 0.022461, 0.022461, 0.022461, 0.022461, 0.022461, 0.022461},
          {0.023551, 0.023551, 0.023551, 0.023551, 0.023551, 0.023551, 0.023551},
          {0.023551, 0.023551, 0.023551, 0.023551, 0.023551, 0.023551, 0.023551}};

        return data;
    }

    std::vector<std::vector<double>> loadRf()
    {
        std::vector<std::vector<double>> data{
          {0.148154, 0.148805, 0.148805, 0.148805, 0.148805, 0.148805, 0.148805},
          {0.150762, 0.150762, 0.150762, 0.150762, 0.150762, 0.150762, 0.150762},
          {0.150762, 0.150762, 0.150762, 0.150762, 0.150762, 0.150762, 0.150762},
          {0.154041, 0.154041, 0.154041, 0.154041, 0.154041, 0.154041, 0.154041},
          {0.154041, 0.154041, 0.154041, 0.154041, 0.154041, 0.154041, 0.154041},
          {0.158675, 0.158675, 0.158675, 0.158675, 0.158675, 0.158675, 0.158675},
          {0.158675, 0.158675, 0.158675, 0.158675, 0.158675, 0.158675, 0.158675}};

        return data;
    }
    std::vector<std::vector<double>> loadRb()
    {
        std::vector<std::vector<double>> data{
          {0.167364, 0.167990, 0.167990, 0.167990, 0.167990, 0.167990, 0.167990},
          {0.169873, 0.169873, 0.169873, 0.169873, 0.169873, 0.169873, 0.169873},
          {0.169873, 0.169873, 0.169873, 0.169873, 0.169873, 0.169873, 0.169873},
          {0.173027, 0.173027, 0.173027, 0.173027, 0.173027, 0.173027, 0.173027},
          {0.173027, 0.173027, 0.173027, 0.173027, 0.173027, 0.173027, 0.173027},
          {0.177485, 0.177485, 0.177485, 0.177485, 0.177485, 0.177485, 0.177485},
          {0.177485, 0.177485, 0.177485, 0.177485, 0.177485, 0.177485, 0.177485}};

        return data;
    }


protected:
    void SetUp() override
    {
        // Create material from samples
        auto thickness = 3.048e-3;   // [m]
        auto aMaterial_102 = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);
        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Small);
        auto tf = loadTf();
        auto tb = loadTf();
        auto rf = loadRf();
        auto rb = loadRb();
        const auto aBSDFMaterial = Material::dualBandBSDFMaterial(
          tf, tb, rf, rb, tf, tb, rf, rb, aBSDF, StandardData::solarRadiationASTM_E891_87_Table1());

        auto Layer_102 = CBSDFLayerMaker::getSpecularLayer(aMaterial_102, aBSDF);
        auto Layer_BSDF = CBSDFLayerMaker::getPreLoadedBSDFLayer(aBSDFMaterial, aBSDF);

        m_Layer = CMultiPaneBSDF::create({Layer_102, Layer_BSDF});

        const CalculationProperties input{
          StandardData::solarRadiationASTM_E891_87_Table1(),
          StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
        m_Layer->setCalculationProperties(input);
    }

public:
    [[nodiscard]] CMultiPaneBSDF & getLayer() const
    {
        return *m_Layer;
    }
};


TEST_F(MultiPaneBSDF_102_BSDFMaterial, TestBSDFMatrixAsInput)
{
    SCOPED_TRACE("Begin Test: Specular layer - BSDF.");

    const double minLambda = 0.3;
    const double maxLambda = 2.5;

    CMultiPaneBSDF & aLayer = getLayer();

    double tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::T);
    EXPECT_NEAR(0.05860854, tauDiff, 1e-6);

    double rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::R);
    EXPECT_NEAR(0.44433841, rhoDiff, 1e-6);

    double absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.13856463, absDiff1, 1e-6);

    double absDiff2 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 2);
    EXPECT_NEAR(0.35848842, absDiff2, 1e-6);

    double theta = 0;
    double phi = 0;

    double tauHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.13169170, tauHem, 1e-6);

    double tauDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.06836489, tauDir, 1e-6);

    double rhoFrontHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.40345532, rhoFrontHem, 1e-6);

    double rhoBackHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Back, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.54623205, rhoBackHem, 1e-6);

    double rhoDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.07926371, rhoDir, 1e-6);

    double abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.13305454, abs1, 1e-6);

    double abs2 = aLayer.Abs(minLambda, maxLambda, Side::Front, 2, theta, phi);
    EXPECT_NEAR(0.33179845, abs2, 1e-6);

    theta = 45;
    phi = 78;

    tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.06301503, tauHem, 1e-6);

    double rhoHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.40454995, rhoHem, 1e-6);

    abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.14027296, abs1, 1e-6);

    abs2 = aLayer.Abs(minLambda, maxLambda, Side::Front, 2, theta, phi);
    EXPECT_NEAR(0.39216205, abs2, 1e-6);
}
