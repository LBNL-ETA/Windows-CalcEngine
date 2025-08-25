#include <memory>
#include <gtest/gtest.h>

#include <WCESingleLayerOptics.hpp>
#include <WCESpectralAveraging.hpp>
#include <WCECommon.hpp>

#include "standardData.hpp"
#include "spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace SpectralAveraging;
using namespace FenestrationCommon;

class TestNFRC102ScatteringLayer1 : public testing::Test
{
    CScatteringLayer m_Layer;

protected:
    void SetUp() override
    {
        constexpr auto thickness = 3.048e-3;   // [m]
        const auto aMaterial =
          Material::nBandMaterial(SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);

        m_Layer = CScatteringLayer::createSpecularLayer(aMaterial);
        m_Layer.setSourceData(StandardData::solarRadiationASTM_E891_87_Table1());
    }

public:
    CScatteringLayer & getLayer()
    {
        return m_Layer;
    }
};

TEST_F(TestNFRC102ScatteringLayer1, Test1)
{
    SCOPED_TRACE("Begin Test: NFRC 102 scattering layer - 0 deg incident.");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    auto aLayer = getLayer();

    Side aSide = Side::Front;

    double T_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDirect);
    EXPECT_NEAR(0.833807, T_dir_dir, 1e-6);

    double R_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDirect);
    EXPECT_NEAR(0.074816, R_dir_dir, 1e-6);

    double T_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDiffuse);
    EXPECT_NEAR(0, T_dir_dif, 1e-6);

    double R_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDiffuse);
    EXPECT_NEAR(0, R_dir_dif, 1e-6);

    double T_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.752655, T_dif_dif, 1e-6);

    double R_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.146041, R_dif_dif, 1e-6);

    double A_dir = aLayer.getAbsorptance(aSide, ScatteringSimple::Direct);
    EXPECT_NEAR(0.091376, A_dir, 1e-6);

    double A_dif = aLayer.getAbsorptance(aSide, ScatteringSimple::Diffuse);
    EXPECT_NEAR(0.101303, A_dif, 1e-6);
}

TEST_F(TestNFRC102ScatteringLayer1, Test2)
{
    SCOPED_TRACE("Begin Test: NFRC 102 scattering layer - Theta = 45 deg, Phi = 90 incident.");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    auto aLayer = getLayer();

    constexpr Side aSide = Side::Front;
    constexpr double Theta = 45;
    constexpr double Phi = 90;

    double T_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDirect, Theta, Phi);
    EXPECT_NEAR(0.809175, T_dir_dir, 1e-6);

    double R_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDirect, Theta, Phi);
    EXPECT_NEAR(0.088811, R_dir_dir, 1e-6);

    double T_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDiffuse, Theta, Phi);
    EXPECT_NEAR(0.008378, T_dir_dif, 1e-6);

    double R_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDiffuse, Theta, Phi);
    EXPECT_NEAR(0, R_dir_dif, 1e-6);

    double T_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DiffuseDiffuse, Theta, Phi);
    EXPECT_NEAR(0.752655, T_dif_dif, 1e-6);

    double R_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DiffuseDiffuse, Theta, Phi);
    EXPECT_NEAR(0.146042, R_dif_dif, 1e-6);

    double A_dir = aLayer.getAbsorptance(aSide, ScatteringSimple::Direct, Theta, Phi);
    EXPECT_NEAR(0.093637, A_dir, 1e-6);

    double A_dif = aLayer.getAbsorptance(aSide, ScatteringSimple::Diffuse, Theta, Phi);
    EXPECT_NEAR(0.101303, A_dif, 1e-6);
}
