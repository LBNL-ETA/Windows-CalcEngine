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

class MultiPaneScattered_102_VenetianUniform : public testing::Test
{
    std::unique_ptr<CMultiLayerScattered> m_Layer;

protected:
    void SetUp() override
    {
        // Create material from samples
        constexpr auto thickness = 3.048e-3;   // [m]
        auto aMaterial_102 = Material::nBandMaterial(
          SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);

        // Venetian blind material
        // Solar range
        constexpr auto Tsol = 0.1;
        constexpr auto Rfsol = 0.7;
        constexpr auto Rbsol = 0.7;

        // Visible range
        constexpr auto Tvis = 0.2;
        constexpr auto Rfvis = 0.6;
        constexpr auto Rbvis = 0.6;

        const auto aMaterialVenetian = Material::dualBandMaterial(
          Tsol, Tsol, Rfsol, Rbsol, Tvis, Tvis, Rfvis, Rbvis);

        // make cell geometry
        constexpr auto slatWidth = 0.016;     // m
        constexpr auto slatSpacing = 0.012;   // m
        constexpr auto slatTiltAngle = 45;
        constexpr auto curvatureRadius = 0.0;
        constexpr size_t numOfSlatSegments = 5;

        CScatteringLayer Layer102 = CScatteringLayer::createSpecularLayer(aMaterial_102);
        CScatteringLayer LayerVenetian =
          CScatteringLayer::createVenetianLayer(aMaterialVenetian,
                                                slatWidth,
                                                slatSpacing,
                                                slatTiltAngle,
                                                curvatureRadius,
                                                numOfSlatSegments,
                                                DistributionMethod::UniformDiffuse);


        // Equivalent BSDF layer
        m_Layer = CMultiLayerScattered::create(Layer102);
        m_Layer->addLayer(LayerVenetian);

        m_Layer->setSourceData(StandardData::solarRadiationASTM_E891_87_Table1());
    }

public:
    [[nodiscard]] CMultiLayerScattered & getLayer() const
    {
        return *m_Layer;
    }
};

TEST_F(MultiPaneScattered_102_VenetianUniform, TestVenetianUniformDirectBeam)
{
    SCOPED_TRACE(
      "Begin Test: Venetian (uniform) layer - Scattering model front side (normal incidence).");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    auto & aLayer = getLayer();

    constexpr auto aSide = Side::Front;
    constexpr double theta = 0;
    constexpr double phi = 0;

    auto T_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.047686, T_dir_dir, 1e-6);

    auto T_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.199672, T_dir_dif, 1e-6);

    auto T_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.302710, T_dif_dif, 1e-6);

    auto R_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.074817, R_dir_dir, 1e-6);

    auto R_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.292574, R_dir_dif, 1e-6);

    auto R_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.379244, R_dif_dif, 1e-6);

    auto A_dir1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.130697, A_dir1, 1e-6);

    auto A_dir2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.254553, A_dir2, 1e-6);

    auto A_dif1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.132645, A_dif1, 1e-6);

    auto A_dif2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.185401, A_dif2, 1e-6);
}

TEST_F(MultiPaneScattered_102_VenetianUniform, TestVenetianUniformAngledBeam25)
{
    SCOPED_TRACE(
      "Begin Test: Venetian (uniform) layer - Scattering model back side (normal incidence).");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    auto & aLayer = getLayer();

    constexpr auto aSide = Side::Front;
    constexpr double theta = 25;
    constexpr double phi = 0;

    auto T_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.047446, T_dir_dir, 1e-6);

    auto T_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.199672, T_dir_dif, 1e-6);

    auto T_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.302710, T_dif_dif, 1e-6);

    auto R_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.075583, R_dir_dir, 1e-6);

    auto R_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.292574, R_dir_dif, 1e-6);

    auto R_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.379244, R_dif_dif, 1e-6);

    auto A_dir1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.132257, A_dir1, 1e-6);

    auto A_dir2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.253701, A_dir2, 1e-6);

    auto A_dif1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.132645, A_dif1, 1e-6);

    auto A_dif2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.185401, A_dif2, 1e-6);
}

TEST_F(MultiPaneScattered_102_VenetianUniform, TestVenetianUniformAngleBeam50)
{
    SCOPED_TRACE(
      "Begin Test: Venetian (uniform) layer - Scattering model front side (Theta = 50 deg).");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    auto & aLayer = getLayer();

    constexpr auto aSide = Side::Front;
    constexpr double theta = 50;
    constexpr double phi = 0;

    auto T_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.045559, T_dir_dir, 1e-6);

    auto T_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.199672, T_dir_dif, 1e-6);

    auto T_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::T, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.302710, T_dif_dif, 1e-6);

    auto R_dir_dir = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.099211, R_dir_dir, 1e-6);

    auto R_dir_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0.292574, R_dir_dif, 1e-6);

    auto R_dif_dif = aLayer.getPropertySimple(
      minLambda, maxLambda, PropertySimple::R, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.379244, R_dif_dif, 1e-6);

    auto A_dir1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.141740, A_dir1, 1e-6);

    auto A_dir2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.243199, A_dir2, 1e-6);

    auto A_dif1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.132645, A_dif1, 1e-6);

    auto A_dif2 = aLayer.getAbsorptanceLayer(2, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.185401, A_dif2, 1e-6);
}
