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

// Example on how to create scattered multilayer.

class MultiPaneScattered_102_PerforatedRectangular : public testing::Test
{
    std::unique_ptr<CMultiLayerScattered> m_Layer;

protected:
    void SetUp() override
    {
        // Create samples from measurements and solar radiation
        auto aSample_102 = std::make_shared<CSpectralSample>(SpectralSample::NFRC_102());

        // Create material from samples
        constexpr auto thickness1 = 3.048e-3;   // [m]
        auto aMaterial_102 =
          Material::nBandMaterial(SpectralSample::NFRC_102(), thickness1, MaterialType::Monolithic);

        // Setting rectangular perforated shade with double range material
        constexpr auto Tsol = 0.1;
        constexpr auto Rfsol = 0.7;
        constexpr auto Rbsol = 0.7;

        // Visible range
        constexpr auto Tvis = 0.2;
        constexpr auto Rfvis = 0.6;
        constexpr auto Rbvis = 0.6;

        auto aMaterialPerforated = SingleLayerOptics::Material::dualBandMaterial(
          Tsol, Tsol, Rfsol, Rbsol, Tvis, Tvis, Rfvis, Rbvis);

        // make cell geometry
        constexpr auto x = 0.01905;          // m
        constexpr auto y = 0.01905;          // m
        constexpr auto thickness2 = 0.005;   // m
        constexpr auto xHole = 0.005;        // m
        constexpr auto yHole = 0.005;        // m

        CScatteringLayer Layer102 = CScatteringLayer::createSpecularLayer(aMaterial_102);
        CScatteringLayer LayerPerforated = CScatteringLayer::createPerforatedRectangularLayer(
          aMaterialPerforated, x, y, thickness2, xHole, yHole);

        // Equivalent BSDF layer
        m_Layer = CMultiLayerScattered::create(Layer102);
        m_Layer->addLayer(LayerPerforated);

        m_Layer->setSourceData(StandardData::solarRadiationASTM_E891_87_Table1());
    }

public:
    [[nodiscard]] CMultiLayerScattered & getLayer() const
    {
        return *m_Layer;
    }
};

TEST_F(MultiPaneScattered_102_PerforatedRectangular, TestPerforatedRectangularDirectBeam)
{
    SCOPED_TRACE(
      "Begin Test: Perforated rectangular layer - Scattering model front side (normal incidence).");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    auto & aLayer = getLayer();

    constexpr auto aSide = Side::Front;
    constexpr double theta = 0;
    constexpr double phi = 0;

    auto T_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.057440, T_dir_dir, 1e-6);

    auto T_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.087586, T_dir_dif, 1e-6);

    auto T_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.094272, T_dif_dif, 1e-6);

    auto R_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.074817, R_dir_dir, 1e-6);

    auto R_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.454943, R_dir_dif, 1e-6);

    auto R_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.580906, R_dif_dif, 1e-6);

    auto A_dir1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.152519, A_dir1, 1e-6);

    auto A_dir2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.172695, A_dir2, 1e-6);

    auto A_dif1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.159748, A_dif1, 1e-6);

    auto A_dif2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.165073, A_dif2, 1e-6);
}

TEST_F(MultiPaneScattered_102_PerforatedRectangular, TestPerforatedRectangularAngledBeam25)
{
    SCOPED_TRACE(
      "Begin Test: Perforated rectangular layer - Scattering model back side (normal incidence).");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    auto & aLayer = getLayer();

    constexpr auto aSide = Side::Front;
    constexpr double theta = 25;
    constexpr double phi = 0;

    auto T_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.030501, T_dir_dir, 1e-6);

    auto T_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.095823, T_dir_dif, 1e-6);

    auto T_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.094272, T_dif_dif, 1e-6);

    auto R_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.075583, R_dir_dir, 1e-6);

    auto R_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.467194, R_dir_dif, 1e-6);

    auto R_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.580906, R_dif_dif, 1e-6);

    auto A_dir1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.155670, A_dir1, 1e-6);

    auto A_dir2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.176836, A_dir2, 1e-6);

    auto A_dif1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.159748, A_dif1, 1e-6);

    auto A_dif2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.165073, A_dif2, 1e-6);
}


TEST_F(MultiPaneScattered_102_PerforatedRectangular, TestPerforatedRectangularAngleBeam50)
{
    SCOPED_TRACE(
      "Begin Test: Perforated rectangular layer - Scattering model front side (Theta = 50 deg).");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    auto & aLayer = getLayer();

    constexpr auto aSide = Side::Front;
    constexpr double theta = 50;
    constexpr double phi = 0;

    auto T_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.000000, T_dir_dir, 1e-6);

    auto T_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.094066, T_dir_dif, 1e-6);

    auto T_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.094272, T_dif_dif, 1e-6);

    auto R_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.099211, R_dir_dir, 1e-6);

    auto R_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.488602, R_dir_dif, 1e-6);

    auto R_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.580906, R_dif_dif, 1e-6);

    auto A_dir1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.166910, A_dir1, 1e-6);

    auto A_dir2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.177199, A_dir2, 1e-6);

    auto A_dif1 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 1, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.159748, A_dif1, 1e-6);

    auto A_dif2 = aLayer.getAbsorptanceLayer(
      minLambda, maxLambda, 2, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.165073, A_dif2, 1e-6);
}
