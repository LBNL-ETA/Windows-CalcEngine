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

class MultiPaneScattered_102_Woven : public testing::Test
{
    std::unique_ptr<CMultiLayerScattered> m_Layer;

protected:
    void SetUp() override
    {
        // Create material from samples
        constexpr auto thickness = 3.048e-3;   // [m]
        const auto aMaterial_102 =
          Material::nBandMaterial(SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);

        // Solar
        constexpr auto Tsol = 0.0;
        constexpr auto Rfsol = 0.2;
        constexpr auto Rbsol = 0.2;

        // Visible range
        constexpr auto Tvis = 0.0;
        constexpr auto Rfvis = 0.1;
        constexpr auto Rbvis = 0.1;

        const auto aMaterialWoven = SingleLayerOptics::Material::dualBandMaterial(
          Tsol, Tsol, Rfsol, Rbsol, Tvis, Tvis, Rfvis, Rbvis);

        // make cell geometry
        constexpr auto diameter = 0.1;   // m
        constexpr auto spacing = 0.3;    // m

        const auto Layer102 = CScatteringLayer::createSpecularLayer(aMaterial_102);
        const auto LayerWoven =
          CScatteringLayer::createWovenLayer(aMaterialWoven, diameter, spacing);


        // Equivalent BSDF layer
        m_Layer = CMultiLayerScattered::create(Layer102);
        m_Layer->addLayer(LayerWoven);

        m_Layer->setSourceData(StandardData::solarRadiationASTM_E891_87_Table1());
    }

public:
    [[nodiscard]] CMultiLayerScattered & getLayer() const
    {
        return *m_Layer;
    }
};

TEST_F(MultiPaneScattered_102_Woven, TestWovenDirectBeam)
{
    SCOPED_TRACE("Begin Test: Woven layer - Scattering model front side (normal incidence).");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    auto & aLayer = getLayer();

    constexpr auto aSide = Side::Front;
    constexpr double theta = 0;
    constexpr double phi = 0;

    const auto T_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.37058090121615606, T_dir_dir, 1e-6);

    const auto T_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.0048808741902768172, T_dir_dif, 1e-6);

    const auto T_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.22901317354759129, T_dif_dif, 1e-6);

    const auto R_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.074816596463456245, R_dir_dir, 1e-6);

    const auto R_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.0704549, R_dir_dif, 1e-6);

    const auto R_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.220159, R_dif_dif, 1e-6);

    const auto A_dir1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.100845, A_dir1, 1e-6);

    const auto A_dir2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.37842143036663228, A_dir2, 1e-6);

    const auto A_dif1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.111265, A_dif1, 1e-6);

    const auto A_dif2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.43956297346634654, A_dif2, 1e-6);
}

TEST_F(MultiPaneScattered_102_Woven, TestWovenAngledBeam25)
{
    SCOPED_TRACE("Begin Test: Woven layer - Scattering model back side (normal incidence).");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    auto & aLayer = getLayer();

    constexpr auto aSide = Side::Front;
    constexpr double theta = 25;
    constexpr double phi = 0;

    const auto T_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.34965232450210415, T_dir_dir, 1e-6);

    const auto T_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.01275180630292648, T_dir_dif, 1e-6);

    const auto T_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.22901317354759129, T_dif_dif, 1e-6);

    const auto R_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.075583231565284098, R_dir_dir, 1e-6);

    const auto R_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.071876, R_dir_dif, 1e-6);

    const auto R_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.220159, R_dif_dif, 1e-6);

    const auto A_dir1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.102695, A_dir1, 1e-6);

    const auto A_dir2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.38716028280323356, A_dir2, 1e-6);

    const auto A_dif1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.111265, A_dif1, 1e-6);

    const auto A_dif2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.43956297346634654, A_dif2, 1e-6);
}

TEST_F(MultiPaneScattered_102_Woven, TestWovenAngleBeam50)
{
    SCOPED_TRACE("Begin Test: Woven layer - Scattering model front side (Theta = 50 deg).");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    auto & aLayer = getLayer();

    constexpr auto aSide = Side::Front;
    constexpr double theta = 50;
    constexpr double phi = 0;

    auto T_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.2556741646920998, T_dir_dir, 1e-6);

    auto T_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.020920852348678964, T_dir_dif, 1e-6);

    auto T_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.22901317354759129, T_dif_dif, 1e-6);

    auto R_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.099211435499337183, R_dir_dir, 1e-6);

    auto R_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.0741108, R_dir_dif, 1e-6);

    auto R_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.220159, R_dif_dif, 1e-6);

    auto A_dir1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.113688, A_dir1, 1e-6);

    auto A_dir2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.44063310775720743, A_dir2, 1e-6);

    auto A_dif1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.111265, A_dif1, 1e-6);

    auto A_dif2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.43956297346634654, A_dif2, 1e-6);
}
