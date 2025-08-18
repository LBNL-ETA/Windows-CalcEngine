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

class MultiPaneBSDF_102_Woven_n_BandMaterial : public testing::Test
{
    std::unique_ptr<CMultiPaneBSDF> m_Layer;

protected:
    void SetUp() override
    {
        auto thickness = 3.048e-3;   // [m]
        const auto aMaterial_102 = Material::nBandMaterial(
          SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);

        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);
        auto Layer_102 = CBSDFLayerMaker::getSpecularLayer(aMaterial_102, aBSDF);

        // Woven material
        thickness = 0.1;   // [m]
        const auto aWovenMaterial = Material::nBandMaterial(
          SpectralSample::NFRC_31100(), thickness, MaterialType::Monolithic);

        // make cell geometry
        constexpr auto diameter = 0.001;   // m
        constexpr auto spacing = 0.002;    // m

        // Perforated layer is created here
        auto LayerWoven = CBSDFLayerMaker::getWovenLayer(aWovenMaterial, aBSDF, diameter, spacing);

        m_Layer = CMultiPaneBSDF::create({LayerWoven, Layer_102});

        const CalculationProperties input{StandardData::solarRadiationASTM_E891_87_Table1(),
                                          StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
        m_Layer->setCalculationProperties(input);
    }

public:
    [[nodiscard]] CMultiPaneBSDF & getLayer() const
    {
        return *m_Layer;
    }
};

TEST_F(MultiPaneBSDF_102_Woven_n_BandMaterial, TestWovenShade)
{
    SCOPED_TRACE("Begin Test: Woven shade - BSDF n-band material.");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiPaneBSDF & aLayer = getLayer();

    const double tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySimple::T);
    EXPECT_NEAR(0.13484825298678549, tauDiff, 1e-6);

    const double rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySimple::R);
    EXPECT_NEAR(0.56618660131592768, rhoDiff, 1e-6);

    const double absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.28263551018831601, absDiff1, 1e-6);

    const double absDiff2 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 2);
    EXPECT_NEAR(0.016329675082156797, absDiff2, 1e-6);

    constexpr double theta = 0;
    constexpr double phi = 0;

    const double tauHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.22102289948987869, tauHem, 1e-6);

    const double tauDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySimple::T, theta, phi);
    EXPECT_NEAR(0.20880416888774572, tauDir, 1e-6);

    const double rhoHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySimple::R, theta, phi);
    EXPECT_NEAR(0.51172408586557705, rhoHem, 1e-6);

    const double rhoDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySimple::R, theta, phi);
    EXPECT_NEAR(0.017084421624025157, rhoDir, 1e-6);

    const double abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.24285700879406391, abs1, 1e-6);

    const double abs2 = aLayer.Abs(minLambda, maxLambda, Side::Front, 2, theta, phi);
    EXPECT_NEAR(0.024396023333432437, abs2, 1e-6);
}
