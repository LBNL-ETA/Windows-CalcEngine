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

// Example on how to create scattered multilayer.

class MultiPaneScattered_102 : public testing::Test
{
    std::unique_ptr<CMultiLayerScattered> m_Layer;

protected:
    void SetUp() override
    {
        // Create material from samples
        constexpr auto thickness = 3.048e-3;   // [m]
        auto aMaterial_102 = Material::nBandMaterial(
          SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);

        CScatteringLayer Layer102 = CScatteringLayer::createSpecularLayer(aMaterial_102);

        // Equivalent BSDF layer
        m_Layer = CMultiLayerScattered::create(Layer102);

        CSeries solarRadiation{StandardData::solarRadiationASTM_E891_87_Table1()};
        m_Layer->setSourceData(solarRadiation);
    }

public:
    [[nodiscard]] CMultiLayerScattered & getLayer() const
    {
        return *m_Layer;
    };
};

TEST_F(MultiPaneScattered_102, TestSpecular1)
{
    SCOPED_TRACE("Begin Test: Specular layer - Scattering model front side (normal incidence).");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiLayerScattered & aLayer = getLayer();

    constexpr Side aSide = Side::Front;
    constexpr double theta = 0;
    constexpr double phi = 0;

    double T_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.833807, T_dir_dir, 1e-6);

    double T_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0, T_dir_dif, 1e-6);

    double T_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.752655, T_dif_dif, 1e-6);

    double R_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.074817, R_dir_dir, 1e-6);

    double R_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0, R_dir_dif, 1e-6);

    double R_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.146042, R_dif_dif, 1e-6);

    double A_dir1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.091376, A_dir1, 1e-6);

    double A_dif1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.101303, A_dif1, 1e-6);
}

TEST_F(MultiPaneScattered_102, TestSpecular2)
{
    SCOPED_TRACE("Begin Test: Specular layer - Scattering model back side (normal incidence).");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiLayerScattered & aLayer = getLayer();

    constexpr Side aSide = Side::Back;
    constexpr double theta = 0;
    constexpr double phi = 0;

    double T_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.833807, T_dir_dir, 1e-6);

    double T_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0, T_dir_dif, 1e-6);

    double T_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.752677, T_dif_dif, 1e-6);

    double R_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.074937, R_dir_dir, 1e-6);

    double R_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0, R_dir_dif, 1e-6);

    double R_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.146165, R_dif_dif, 1e-6);

    double A_dir1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.091256, A_dir1, 1e-6);

    double A_dif1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.101157, A_dif1, 1e-6);
}

TEST_F(MultiPaneScattered_102, TestSpecular3)
{
    SCOPED_TRACE("Begin Test: Specular layer - Scattering model front side (Theta = 40 deg).");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiLayerScattered & aLayer = getLayer();

    constexpr Side aSide = Side::Front;
    constexpr double theta = 40;
    constexpr double phi = 0;

    double T_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.817553, T_dir_dir, 1e-6);

    double T_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0, T_dir_dif, 1e-6);

    double T_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.752655, T_dif_dif, 1e-6);

    double R_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.082508, R_dir_dir, 1e-6);

    double R_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0, R_dir_dif, 1e-6);

    double R_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.146042, R_dif_dif, 1e-6);

    double A_dir1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.099939, A_dir1, 1e-6);

    double A_dif1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.101303, A_dif1, 1e-6);
}
