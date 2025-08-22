#include <memory>
#include <gtest/gtest.h>

#include <WCEMultiLayerOptics.hpp>

#include "standardData.hpp"
#include "spectralSampleData.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

class MultiPaneBSDF_102_Homogeneous_31100_n_Band_Material : public testing::Test
{
private:
    std::unique_ptr<CMultiPaneBSDF> m_Layer;

protected:
    void SetUp() override
    {
        const auto solarRadiation{StandardData::solarRadiationASTM_E891_87_Table1()};

        double thickness = 3.048e-3;   // [m]
        auto aMaterial_102 = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);

        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);

        auto Layer_102 = CBSDFLayerMaker::getSpecularLayer(aMaterial_102, aBSDF);

        // Venetian blind material
        thickness = 0.0001;   // [m]
        auto aMaterial = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_31100(), thickness, MaterialType::Monolithic);

        std::shared_ptr<CBSDFLayer> Layer_Homogeneous =
          CBSDFLayerMaker::getHomogeneousDiffuseLayer(aMaterial, aBSDF);

        m_Layer = CMultiPaneBSDF::create({Layer_102, Layer_Homogeneous});

        const CalculationProperties input{solarRadiation, solarRadiation.getXArray()};
        m_Layer->setCalculationProperties(input);
    }

public:
    CMultiPaneBSDF & getLayer()
    {
        return *m_Layer;
    }
};

TEST_F(MultiPaneBSDF_102_Homogeneous_31100_n_Band_Material, TestBSDF1)
{
    SCOPED_TRACE("Begin Test: Specular and homogeneous diffuse IGU - BSDF.");

    const double minLambda = 0.3;
    const double maxLambda = 2.5;

    CMultiPaneBSDF & aLayer = getLayer();

    const double tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::T);
    EXPECT_NEAR(0.0, tauDiff, 1e-6);

    const double rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::R);
    EXPECT_NEAR(0.575221, rhoDiff, 1e-6);   // was 0.575213

    const double absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.153284, absDiff1, 1e-6);   // was 0.153292

    const double absDiff2 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 2);
    EXPECT_NEAR(0.271495, absDiff2, 1e-6);

    const double theta = 0;
    const double phi = 0;

    const double tauHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.0, tauHem, 1e-6);

    const double tauDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.0, tauDir, 1e-6);

    const double rhoHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.564570, rhoHem, 1e-6);   // was 0.564561

    const double rhoDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.088154, rhoDir, 1e-6);

    const double abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.151848, abs1, 1e-6);   // was 0.151857

    const double abs2 = aLayer.Abs(minLambda, maxLambda, Side::Front, 2, theta, phi);
    EXPECT_NEAR(0.283582, abs2, 1e-6);
}
