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

class MultiPaneScattered_102_PerfectDiffuse : public testing::Test
{
    std::unique_ptr<CMultiLayerScattered> m_Layer;

protected:
    void SetUp() override
    {
        // Create material from samples
        constexpr auto thickness = 3.048e-3;   // [m]
        auto aMaterial_102 = Material::nBandMaterial(
          SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);

        // Solar range
        constexpr auto Tsol = 0.0;
        constexpr auto Rfsol = 0.2;
        constexpr auto Rbsol = 0.2;

        // Visible range
        constexpr auto Tvis = 0.0;
        constexpr auto Rfvis = 0.1;
        constexpr auto Rbvis = 0.1;

        auto aMaterial = Material::dualBandMaterial(
          Tsol, Tsol, Rfsol, Rbsol, Tvis, Tvis, Rfvis, Rbvis);

        CScatteringLayer Layer102 = CScatteringLayer::createSpecularLayer(aMaterial_102);
        CScatteringLayer LayerDiffuse = CScatteringLayer::createPerfectlyDiffusingLayer(aMaterial);


        // Equivalent BSDF layer
        m_Layer = CMultiLayerScattered::create(Layer102);
        m_Layer->addLayer(LayerDiffuse);

        m_Layer->setSourceData(StandardData::solarRadiationASTM_E891_87_Table1());
    }

public:
    [[nodiscard]] CMultiLayerScattered & getLayer() const
    {
        return *m_Layer;
    }
};

TEST_F(MultiPaneScattered_102_PerfectDiffuse, TestPerfectDiffuseDirectBeam)
{
    SCOPED_TRACE(
      "Begin Test: Perfectly diffusing layer - Scattering model front side (normal incidence).");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    auto & aLayer = getLayer();

    constexpr auto aSide = Side::Front;
    constexpr double theta = 0;
    constexpr double phi = 0;

    auto T_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.0, T_dir_dir, 1e-6);

    auto T_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.0, T_dir_dif, 1e-6);

    auto T_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.0, T_dif_dif, 1e-6);

    auto R_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.074817, R_dir_dir, 1e-6);

    auto R_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.129297, R_dir_dif, 1e-6);

    auto R_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.262755, R_dif_dif, 1e-6);

    auto A_dir1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.108753, A_dir1, 1e-6);

    auto A_dir2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.687132, A_dir2, 1e-6);

    auto A_dif1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.116989, A_dif1, 1e-6);

    auto A_dif2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.620256, A_dif2, 1e-6);
}

TEST_F(MultiPaneScattered_102_PerfectDiffuse, TestPerfectDiffuseAngledBeam25)
{
    SCOPED_TRACE(
      "Begin Test: Perfectly diffusing layer - Scattering model back side (Theta = 25 deg).");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    auto & aLayer = getLayer();

    constexpr auto aSide = Side::Front;
    constexpr double theta = 25;
    constexpr double phi = 0;

    auto T_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.0, T_dir_dir, 1e-6);

    auto T_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.0, T_dir_dif, 1e-6);

    auto T_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.0, T_dif_dif, 1e-6);

    auto R_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.0755832, R_dir_dir, 1e-6);

    auto R_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.129297, R_dir_dif, 1e-6);

    auto R_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.262755, R_dif_dif, 1e-6);

    auto A_dir1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.110387, A_dir1, 1e-6);

    auto A_dir2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.685113, A_dir2, 1e-6);

    auto A_dif1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.116989, A_dif1, 1e-6);

    auto A_dif2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.620256, A_dif2, 1e-6);
}

TEST_F(MultiPaneScattered_102_PerfectDiffuse, TestPerfectDiffuseAngleBeam50)
{
    SCOPED_TRACE(
      "Begin Test: Perfectly diffusing layer - Scattering model front side (Theta = 50 deg).");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    auto & aLayer = getLayer();

    constexpr auto aSide = Side::Front;
    constexpr double theta = 50;
    constexpr double phi = 0;

    auto T_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.0, T_dir_dir, 1e-6);

    auto T_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.0, T_dir_dif, 1e-6);

    auto T_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.0, T_dif_dif, 1e-6);

    auto R_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.099211, R_dir_dir, 1e-6);

    auto R_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.129297, R_dir_dif, 1e-6);

    auto R_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.262755, R_dif_dif, 1e-6);

    auto A_dir1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.120774, A_dir1, 1e-6);

    auto A_dir2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.656484, A_dir2, 1e-6);

    auto A_dif1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.116989, A_dif1, 1e-6);

    auto A_dif2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.620256, A_dif2, 1e-6);
}
