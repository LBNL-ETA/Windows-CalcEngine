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

class MultiPaneBSDF_102_VenetianDirectional_n_Band_Material_Condensed : public testing::Test
{
private:
    std::unique_ptr<CMultiPaneBSDF> m_Layer;

protected:
    virtual void SetUp()
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
          SpectralSample::NFRC_31100(), thickness, MaterialType::Monolithic);


        // make cell geometry
        const auto slatWidth = 0.016;     // m
        const auto slatSpacing = 0.012;   // m
        const auto slatTiltAngle = 45;
        const auto curvatureRadius = 0.0;
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

        const CalculationProperties input{solarRadiation, StandardData::condensedSpectrumDefault()};
        m_Layer->setCalculationProperties(input);
    }

public:
    CMultiPaneBSDF & getLayer()
    {
        return *m_Layer;
    };
};

TEST_F(MultiPaneBSDF_102_VenetianDirectional_n_Band_Material_Condensed, TestBSDF1)
{
    SCOPED_TRACE("Begin Test: Specular and venetian directional IGU - BSDF.");

    constexpr auto minLambda{0.3};
    constexpr auto maxLambda{2.5};

    CMultiPaneBSDF & aLayer = getLayer();

    const double tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::T);
    EXPECT_NEAR(0.258586, tauDiff, 1e-6);

    const double rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::R);
    EXPECT_NEAR(0.364627, rhoDiff, 1e-6);

    const double absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.347440, absDiff1, 1e-6);

    const double absDiff2 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 2);
    EXPECT_NEAR(0.029348, absDiff2, 1e-6);

    constexpr auto theta{0.0};
    constexpr auto phi{0.0};

    const double tauHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.165435, tauHem, 1e-6);

    const double tauDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.051814, tauDir, 1e-6);

    const double rhoHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.406686, rhoHem, 1e-6);

    const double rhoDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.012897, rhoDir, 1e-6);

    const double abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.408806, abs1, 1e-6);

    const double abs2 = aLayer.Abs(minLambda, maxLambda, Side::Front, 2, theta, phi);
    EXPECT_NEAR(0.019073, abs2, 1e-6);
}
