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

// Example on how to create multilayer BSDF with combination of specular and perforated layer

class MultiPaneBSDF_102_PerforatedCircular_NFRC18000 : public testing::Test
{
    std::unique_ptr<CMultiPaneBSDF> m_Layer;

protected:
    virtual void SetUp()
    {
        const auto wl{StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};

        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);

        constexpr auto thickness_102 = 3.048e-3;   // [m]

        auto aMaterial_102 = Material::nBandMaterial(
          SpectralSample::NFRC_102(), thickness_102, MaterialType::Monolithic);
        aMaterial_102->setBandWavelengths(wl);

        auto Layer_102 = CBSDFLayerMaker::getSpecularLayer(aMaterial_102, aBSDF);

        constexpr auto thickness_31111{0.00023};
        auto aMaterial_31111 = Material::nBandMaterial(
          SpectralSample::NFRC_31111(), thickness_31111, MaterialType::Monolithic);
        aMaterial_31111->setBandWavelengths(wl);

        // make cell geometry
        constexpr auto x = 0.00169;        // m
        constexpr auto y = 0.00169;        // m
        constexpr auto radius = 0.00058;   // m

        // Perforated layer is created here
        const auto perforated = CBSDFLayerMaker::getCircularPerforatedLayer(
          aMaterial_31111, aBSDF, x, y, thickness_31111, radius);

        m_Layer = CMultiPaneBSDF::create({perforated, Layer_102});

        const CalculationProperties input{
          StandardData::solarRadiationASTM_E891_87_Table1(),
          StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
        m_Layer->setCalculationProperties(input);
    }

public:
    CMultiPaneBSDF & getLayer() const
    {
        return *m_Layer;
    }
};

TEST_F(MultiPaneBSDF_102_PerforatedCircular_NFRC18000, Test102PerofratedCircular)
{
    SCOPED_TRACE("Begin Test: Specular layer - BSDF.");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiPaneBSDF & aLayer = getLayer();

    double tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::T);
    EXPECT_NEAR(0.21249128345672524, tauDiff, 1e-6);

    double rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::R);
    EXPECT_NEAR(0.63539424553486545, rhoDiff, 1e-6);

    double absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.1254558414581273, absDiff1, 1e-6);

    double absDiff2 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 2);
    EXPECT_NEAR(0.02665862955027334, absDiff2, 1e-6);

    double theta = 0;
    double phi = 0;

    double tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.31523300551099365, tauHem, 1e-6);

    double tauDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.30872668074190435, tauDir, 1e-6);

    double rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.54336046822785034, rhoHem, 1e-6);

    double rhoDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.023282859797643073, rhoDir, 1e-6);

    double abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.10672566908222603, abs1, 1e-6);

    double abs2 = aLayer.Abs(minLambda, maxLambda, Side::Front, 2, theta, phi);
    EXPECT_NEAR(0.034680857178930609, abs2, 1e-6);
}
