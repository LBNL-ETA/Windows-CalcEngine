#include <memory>
#include <gtest/gtest.h>

#include <WCEMultiLayerOptics.hpp>

#include "standardData.hpp"
#include "spectralSampleData.hpp"


using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

// Example on how to create multilayer BSDF from specular and venetian layers

class MultiPaneBSDF_102_VenetianDirectional_n_Band_Material : public testing::Test
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
        auto aMaterialVenetian = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::WhiteVenetianBlindSlat_31100(), thickness, MaterialType::Monolithic);

        // make cell geometry
        const auto slatWidth = 0.05;     // m
        const auto slatSpacing = 0.07;   // m
        const auto slatTiltAngle = 45;
        const auto curvatureRadius = 0.000;
        const size_t numOfSlatSegments = 5;

        std::shared_ptr<CBSDFLayer> Layer_Venetian =
          CBSDFLayerMaker::getVenetianLayer(aMaterialVenetian,
                                            aBSDF,
                                            slatWidth,
                                            slatSpacing,
                                            slatTiltAngle,
                                            curvatureRadius,
                                            numOfSlatSegments,
                                            DistributionMethod::DirectionalDiffuse,
                                            true);

        m_Layer = CMultiPaneBSDF::create({Layer_Venetian, Layer_102});

        const CalculationProperties input{solarRadiation, solarRadiation.getXArray()};
        m_Layer->setCalculationProperties(input);
    }

public:
    CMultiPaneBSDF & getLayer()
    {
        return *m_Layer;
    };
};

TEST_F(MultiPaneBSDF_102_VenetianDirectional_n_Band_Material, TestBSDF1)
{
    SCOPED_TRACE("Begin Test: Specular and venetian directional IGU - BSDF.");

    const double minLambda = 0.3;
    const double maxLambda = 2.5;

    CMultiPaneBSDF & aLayer = getLayer();

    const double tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySimple::T);
    EXPECT_NEAR(0.421760, tauDiff, 1e-6);

    const double rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.305691, rhoDiff, 1e-6);

    const double absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.217515, absDiff1, 1e-6);

    const double absDiff2 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 2);
    EXPECT_NEAR(0.055033, absDiff2, 1e-6);

    const double theta = 0;
    const double phi = 0;

    const double tauHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.475892, tauHem, 1e-6);

    const double tauDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.413809, tauDir, 1e-6);

    const double rhoHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySimple::R, theta, phi);
    EXPECT_NEAR(0.283308, rhoHem, 1e-6);

    const double rhoDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySimple::R, theta, phi);
    EXPECT_NEAR(0.024572, rhoDir, 1e-6);

    const double abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.186032, abs1, 1e-6);

    const double abs2 = aLayer.Abs(minLambda, maxLambda, Side::Front, 2, theta, phi);
    EXPECT_NEAR(0.054768, abs2, 1e-6);
}
