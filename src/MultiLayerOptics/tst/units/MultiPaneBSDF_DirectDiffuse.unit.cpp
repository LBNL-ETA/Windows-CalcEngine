#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

#include "../../../helper/optical/standardData.hpp"
#include "../../../helper/optical/spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

// Example on how to create multilayer BSDF from specular layers only

class MultiPaneBSDF_DirectDiffuseSingleBand : public testing::Test
{
public:
    std::unique_ptr<CMultiPaneBSDF> m_Layer;

protected:
    void SetUp() override
    {
        constexpr auto Tfmat = 0.08;
        constexpr auto Tbmat = 0.08;
        constexpr auto Rfmat = 0.9;
        constexpr auto Rbmat = 0.9;
        const auto aMaterial = Material::singleBandMaterial(Tfmat, Tbmat, Rfmat, Rbmat);

        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Small);
        auto Layer_102 = CBSDFLayerMaker::getDirDifLayer(aMaterial, aBSDF);

        m_Layer = CMultiPaneBSDF::create({Layer_102});
    }

public:
    CMultiPaneBSDF & getLayer() const
    {
        return *m_Layer;
    };
};

TEST_F(MultiPaneBSDF_DirectDiffuseSingleBand, Solar)
{
    CMultiPaneBSDF & aLayer = getLayer();

    // Solar range
    double minLambda = 0.3;
    double maxLambda = 2.5;

    const CalculationProperties input{
      StandardData::solarRadiationASTM_E891_87_Table1(),
      StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
    aLayer.setCalculationProperties(input);

    auto T = aLayer.getMatrix(minLambda, maxLambda, Side::Front, PropertySurface::T);

    double tauDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::T);
    EXPECT_NEAR(0.080000, tauDiff, 1e-6);

    double rhoDiff = aLayer.DiffDiff(minLambda, maxLambda, Side::Front, PropertySurface::R);
    EXPECT_NEAR(0.900000, rhoDiff, 1e-6);

    double absDiff1 = aLayer.AbsDiff(minLambda, maxLambda, Side::Front, 1);
    EXPECT_NEAR(0.020000, absDiff1, 1e-6);

    double theta = 0;
    double phi = 0;

    double tauHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.080000, tauHem, 1e-6);

    double tauDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.080000, tauDir, 1e-6);

    double rhoHem =
      aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.900000, rhoHem, 1e-6);

    double rhoDir =
      aLayer.DirDir(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.900000, rhoDir, 1e-6);

    double abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.020000, abs1, 1e-6);

    theta = 45;
    phi = 78;

    tauHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::T, theta, phi);
    EXPECT_NEAR(0.080000, tauHem, 1e-6);

    rhoHem = aLayer.DirHem(minLambda, maxLambda, Side::Front, PropertySurface::R, theta, phi);
    EXPECT_NEAR(0.900000, rhoHem, 1e-6);

    abs1 = aLayer.Abs(minLambda, maxLambda, Side::Front, 1, theta, phi);
    EXPECT_NEAR(0.020000, abs1, 1e-6);
}