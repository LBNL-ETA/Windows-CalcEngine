#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>

#include "../../../helper/optical/standardData.hpp"
#include "../../../helper/optical/spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

class MultiPaneBSDF_102_Perforated : public testing::Test
{
private:
    std::unique_ptr<CMultiPaneBSDF> m_Layer;

protected:
    void SetUp() override
    {
        auto thickness = 3.048e-3;   // [m]

        const auto aMaterial_102 = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);

        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Quarter);
        auto Layer_102 = CBSDFLayerMaker::getSpecularLayer(aMaterial_102, aBSDF);

        // Dual band material properties
        const auto Tsol = 0.1;
        const auto Rfsol = 0.7;
        const auto Rbsol = 0.7;

        const auto Tvis = 0.2;
        const auto Rfvis = 0.6;
        const auto Rbvis = 0.6;

        const auto aMaterialPerforated = SingleLayerOptics::Material::dualBandMaterial(
          Tsol, Tsol, Rfsol, Rbsol, Tvis, Tvis, Rfvis, Rbvis);

        // make cell geometry
        const auto x = 0.01905;         // m
        const auto y = 0.01905;         // m
        thickness = 0.005;              // m
        const auto radius = 0.003175;   // m

        // Perforated layer is created here
        const auto perforated = CBSDFLayerMaker::getCircularPerforatedLayer(
          aMaterialPerforated, aBSDF, x, y, thickness, radius);

        m_Layer = CMultiPaneBSDF::create({Layer_102, perforated});

        const CalculationProperties input{
          StandardData::solarRadiationASTM_E891_87_Table1(),   // Solar radiation
          StandardData::condensedSpectrumDefault()             // Common wavelengths
        };
        m_Layer->setCalculationProperties(input);
    }

public:
    [[nodiscard]] CMultiPaneBSDF & getLayer() const
    {
        return *m_Layer;
    }
};

TEST_F(MultiPaneBSDF_102_Perforated, Test102PerofratedCircular)
{
    SCOPED_TRACE("Begin Test: Specular layer - BSDF.");

    const double minLambda = 0.3;
    const double maxLambda = 2.5;

    CMultiPaneBSDF & aLayer = getLayer();

    const double tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::T);
    EXPECT_NEAR(0.113886, tauDiff, 1e-6);

    const double rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::R);
    EXPECT_NEAR(0.579920, rhoDiff, 1e-6);

    const double absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.142015, absDiff1, 1e-6);

    const double absDiff2 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 2);
    EXPECT_NEAR(0.164179, absDiff2, 1e-6);

    double theta = 0;
    double phi = 0;

    double tauHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.170518, tauHem, 1e-6);

    double tauDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.076108, tauDir, 1e-6);

    double rhoHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.521843, rhoHem, 1e-6);

    double rhoDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.087788, rhoDir, 1e-6);

    double abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.135516, abs1, 1e-6);

    double abs2 = aLayer.Abs(minLambda, maxLambda, Side::Front, 2, theta, phi);
    EXPECT_NEAR(0.172123, abs2, 1e-6);

    theta = 45;
    phi = 78;

    tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.132296, tauHem, 1e-6);

    tauDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.033610, tauDir, 1e-6);

    rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.544655, rhoHem, 1e-6);

    rhoDir = aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.092768, rhoDir, 1e-6);

    abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.143872, abs1, 1e-6);

    abs2 = aLayer.Abs(minLambda, maxLambda, Side::Front, 2, theta, phi);
    EXPECT_NEAR(0.179177, abs2, 1e-6);
}
