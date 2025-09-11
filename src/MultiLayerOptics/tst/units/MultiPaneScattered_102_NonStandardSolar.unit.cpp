#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

#include "spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

// Example on how to create scattered multilayer.

class MultiPaneScattered_102_NonStandardSolar : public testing::Test
{
private:
    std::unique_ptr<CMultiLayerScattered> m_Layer;

    // This solar radiation input is not standard. Do not replace it with one in StandardData
    static CSeries loadSolarRadiationFile()
    {
        CSeries aSolarRadiation;

        aSolarRadiation.addProperty(0.3000, 0);
        aSolarRadiation.addProperty(0.3050, 9.5);
        aSolarRadiation.addProperty(0.3100, 42.3);
        aSolarRadiation.addProperty(0.3150, 107.8);
        aSolarRadiation.addProperty(0.3200, 181);
        aSolarRadiation.addProperty(0.3250, 246);
        aSolarRadiation.addProperty(0.3300, 395.3);
        aSolarRadiation.addProperty(0.3350, 390.1);
        aSolarRadiation.addProperty(0.3400, 435.3);
        aSolarRadiation.addProperty(0.3450, 438.9);
        aSolarRadiation.addProperty(0.3500, 483.7);
        aSolarRadiation.addProperty(0.3600, 520.3);
        aSolarRadiation.addProperty(0.3700, 666.2);
        aSolarRadiation.addProperty(0.3800, 712.5);
        aSolarRadiation.addProperty(0.3900, 720.7);
        aSolarRadiation.addProperty(0.4000, 1013.1);
        aSolarRadiation.addProperty(0.4100, 1158.2);
        aSolarRadiation.addProperty(0.4200, 1184);
        aSolarRadiation.addProperty(0.4300, 1071.9);
        aSolarRadiation.addProperty(0.4400, 1302);
        aSolarRadiation.addProperty(0.4500, 1526);
        aSolarRadiation.addProperty(0.4600, 1599.6);
        aSolarRadiation.addProperty(0.4700, 1581);
        aSolarRadiation.addProperty(0.4800, 1628.3);
        aSolarRadiation.addProperty(0.4900, 1539.2);
        aSolarRadiation.addProperty(0.5000, 1548.7);
        aSolarRadiation.addProperty(0.5100, 1586.5);
        aSolarRadiation.addProperty(0.5200, 1484.9);
        aSolarRadiation.addProperty(0.5300, 1572.4);
        aSolarRadiation.addProperty(0.5400, 1550.7);
        aSolarRadiation.addProperty(0.5500, 1561.5);
        aSolarRadiation.addProperty(0.5700, 1501.5);
        aSolarRadiation.addProperty(0.5900, 1395.5);
        aSolarRadiation.addProperty(0.6100, 1485.3);
        aSolarRadiation.addProperty(0.6300, 1434.1);
        aSolarRadiation.addProperty(0.6500, 1419.9);
        aSolarRadiation.addProperty(0.6700, 1392.3);
        aSolarRadiation.addProperty(0.6900, 1130);
        aSolarRadiation.addProperty(0.7100, 1316.7);
        aSolarRadiation.addProperty(0.7180, 1010.3);
        aSolarRadiation.addProperty(0.7244, 1043.2);
        aSolarRadiation.addProperty(0.7400, 1211.2);
        aSolarRadiation.addProperty(0.7525, 1193.9);
        aSolarRadiation.addProperty(0.7575, 1175.5);
        aSolarRadiation.addProperty(0.7625, 643.1);
        aSolarRadiation.addProperty(0.7675, 1030.7);
        aSolarRadiation.addProperty(0.7800, 1131.1);
        aSolarRadiation.addProperty(0.8000, 1081.6);
        aSolarRadiation.addProperty(0.8160, 849.2);
        aSolarRadiation.addProperty(0.8237, 785);
        aSolarRadiation.addProperty(0.8315, 916.4);
        aSolarRadiation.addProperty(0.8400, 959.9);
        aSolarRadiation.addProperty(0.8600, 978.9);
        aSolarRadiation.addProperty(0.8800, 933.2);
        aSolarRadiation.addProperty(0.9050, 748.5);
        aSolarRadiation.addProperty(0.9150, 667.5);
        aSolarRadiation.addProperty(0.9250, 690.3);
        aSolarRadiation.addProperty(0.9300, 403.6);
        aSolarRadiation.addProperty(0.9370, 258.3);
        aSolarRadiation.addProperty(0.9480, 313.6);
        aSolarRadiation.addProperty(0.9650, 526.8);
        aSolarRadiation.addProperty(0.9800, 646.4);
        aSolarRadiation.addProperty(0.9935, 746.8);
        aSolarRadiation.addProperty(1.0400, 690.5);
        aSolarRadiation.addProperty(1.0700, 637.5);
        aSolarRadiation.addProperty(1.1000, 412.6);
        aSolarRadiation.addProperty(1.1200, 108.9);
        aSolarRadiation.addProperty(1.1300, 189.1);
        aSolarRadiation.addProperty(1.1370, 132.2);
        aSolarRadiation.addProperty(1.1610, 339);
        aSolarRadiation.addProperty(1.1800, 460);
        aSolarRadiation.addProperty(1.2000, 423.6);
        aSolarRadiation.addProperty(1.2350, 480.5);
        aSolarRadiation.addProperty(1.2900, 413.1);
        aSolarRadiation.addProperty(1.3200, 250.2);
        aSolarRadiation.addProperty(1.3500, 32.5);
        aSolarRadiation.addProperty(1.3950, 1.6);
        aSolarRadiation.addProperty(1.4425, 55.7);
        aSolarRadiation.addProperty(1.4625, 105.1);
        aSolarRadiation.addProperty(1.4770, 105.5);
        aSolarRadiation.addProperty(1.4970, 182.1);
        aSolarRadiation.addProperty(1.5200, 262.2);
        aSolarRadiation.addProperty(1.5390, 274.2);
        aSolarRadiation.addProperty(1.5580, 275);
        aSolarRadiation.addProperty(1.5780, 244.6);
        aSolarRadiation.addProperty(1.5920, 247.4);
        aSolarRadiation.addProperty(1.6100, 228.7);
        aSolarRadiation.addProperty(1.6300, 244.5);
        aSolarRadiation.addProperty(1.6460, 234.8);
        aSolarRadiation.addProperty(1.6780, 220.5);
        aSolarRadiation.addProperty(1.7400, 171.5);
        aSolarRadiation.addProperty(1.8000, 30.7);
        aSolarRadiation.addProperty(1.8600, 2);
        aSolarRadiation.addProperty(1.9200, 1.2);
        aSolarRadiation.addProperty(1.9600, 21.2);
        aSolarRadiation.addProperty(1.9850, 91.1);
        aSolarRadiation.addProperty(2.0050, 26.8);
        aSolarRadiation.addProperty(2.0350, 99.5);
        aSolarRadiation.addProperty(2.0650, 60.4);
        aSolarRadiation.addProperty(2.1000, 89.1);
        aSolarRadiation.addProperty(2.1480, 82.2);
        aSolarRadiation.addProperty(2.1980, 71.5);
        aSolarRadiation.addProperty(2.2700, 70.2);
        aSolarRadiation.addProperty(2.3600, 62);
        aSolarRadiation.addProperty(2.4500, 21.2);
        aSolarRadiation.addProperty(2.4940, 18.5);
        aSolarRadiation.addProperty(2.5370, 3.2);

        return aSolarRadiation;
    }

protected:
    void SetUp() override
    {
        // Create material from samples
        constexpr auto thickness = 3.048e-3;   // [m]
        auto aMaterial_102 =
          Material::nBandMaterial(SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);

        CScatteringLayer Layer102 = CScatteringLayer::createSpecularLayer(aMaterial_102);

        // Equivalent BSDF layer
        m_Layer = CMultiLayerScattered::create(Layer102);

        m_Layer->setSourceData(loadSolarRadiationFile());
    }

public:
    [[nodiscard]] CMultiLayerScattered & getLayer() const
    {
        return *m_Layer;
    }
};

TEST_F(MultiPaneScattered_102_NonStandardSolar, TestSpecular1)
{
    SCOPED_TRACE("Begin Test: Specular layer - Scattering model front side (normal incidence).");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiLayerScattered & aLayer = getLayer();

    constexpr Side aSide = Side::Front;
    constexpr double theta = 0;
    constexpr double phi = 0;

    double T_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.836918, T_dir_dir, 1e-6);

    double T_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0, T_dir_dif, 1e-6);

    double T_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.756157, T_dif_dif, 1e-6);

    double R_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.075619, R_dir_dir, 1e-6);

    double R_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0, R_dir_dif, 1e-6);

    double R_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.147130, R_dif_dif, 1e-6);

    double A_dir1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.087463, A_dir1, 1e-6);

    double A_dif1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.096713, A_dif1, 1e-6);
}

TEST_F(MultiPaneScattered_102_NonStandardSolar, TestSpecular2)
{
    SCOPED_TRACE("Begin Test: Specular layer - Scattering model back side (normal incidence).");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiLayerScattered & aLayer = getLayer();

    constexpr Side aSide = Side::Back;
    constexpr double theta = 0;
    constexpr double phi = 0;

    double T_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.836918, T_dir_dir, 1e-6);

    double T_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0, T_dir_dif, 1e-6);

    double T_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.756179, T_dif_dif, 1e-6);

    double R_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.075735, R_dir_dir, 1e-6);

    double R_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0, R_dir_dif, 1e-6);

    double R_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.147249, R_dif_dif, 1e-6);

    double A_dir1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.087347, A_dir1, 1e-6);

    double A_dif1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.096572, A_dif1, 1e-6);
}

TEST_F(MultiPaneScattered_102_NonStandardSolar, TestSpecular3)
{
    SCOPED_TRACE("Begin Test: Specular layer - Scattering model front side (Theta = 40 deg).");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiLayerScattered & aLayer = getLayer();

    constexpr Side aSide = Side::Front;
    constexpr double theta = 40;
    constexpr double phi = 0;

    double T_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.821105, T_dir_dir, 1e-6);

    double T_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0, T_dir_dif, 1e-6);

    double T_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.756157, T_dif_dif, 1e-6);

    double R_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDirect, theta, phi);
    EXPECT_NEAR(0.083379, R_dir_dir, 1e-6);

    double R_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDiffuse, theta, phi);
    EXPECT_NEAR(0, R_dir_dif, 1e-6);

    double R_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DiffuseDiffuse, theta, phi);
    EXPECT_NEAR(0.147130, R_dif_dif, 1e-6);

    double A_dir1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Direct, theta, phi);
    EXPECT_NEAR(0.095516, A_dir1, 1e-6);

    double A_dif1 = aLayer.getAbsorptanceLayer(1, aSide, ScatteringSimple::Diffuse, theta, phi);
    EXPECT_NEAR(0.096713, A_dif1, 1e-6);
}
