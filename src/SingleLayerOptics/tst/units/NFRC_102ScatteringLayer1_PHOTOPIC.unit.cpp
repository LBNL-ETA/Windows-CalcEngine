#include <memory>
#include <gtest/gtest.h>

#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

#include "../../../helper/optical/standardData.hpp"
#include "../../../helper/optical/spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace SpectralAveraging;
using namespace FenestrationCommon;

class TestNFRC102ScatteringLayer1_PHOTOPIC : public testing::Test
{
    CScatteringLayer m_Layer;

protected:
    void SetUp() override
    {
        constexpr auto thickness = 3.048e-3;   // [m]
        const auto aMaterial =
          Material::nBandMaterial(SpectralSample::NFRC_102(), thickness, MaterialType::Monolithic);

        m_Layer = CScatteringLayer::createSpecularLayer(aMaterial);
        m_Layer.setSourceData(StandardData::Photopic::solarRadiation());
        m_Layer.setWavelengths(StandardData::Photopic::wavelengthSetPhotopic());
    }

public:
    CScatteringLayer & getLayer()
    {
        return m_Layer;
    }
};

TEST_F(TestNFRC102ScatteringLayer1_PHOTOPIC, Test1)
{
    SCOPED_TRACE("Begin Test: NFRC 102 scattering layer - 0 deg incident.");

    constexpr double minLambda = 0.38;
    constexpr double maxLambda = 0.78;

    auto aLayer = getLayer();

    constexpr Side aSide = Side::Front;

    double T_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDirect);
    EXPECT_NEAR(0.88392833946432292, T_dir_dir, 1e-6);

    double R_dir_dir = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDirect);
    EXPECT_NEAR(0.081491816754847721, R_dir_dir, 1e-6);

    double T_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DirectDiffuse);
    EXPECT_NEAR(0, T_dir_dif, 1e-6);

    double R_dir_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DirectDiffuse);
    EXPECT_NEAR(0, R_dir_dif, 1e-6);

    double T_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::T, aSide, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.80517620769857268, T_dif_dif, 1e-6);

    double R_dif_dif = aLayer.getPropertySurface(
      minLambda, maxLambda, PropertySurface::R, aSide, Scattering::DiffuseDiffuse);
    EXPECT_NEAR(0.1559084672280428, R_dif_dif, 1e-6);

    double A_dir = aLayer.getAbsorptance(aSide, ScatteringSimple::Direct);
    EXPECT_NEAR(0.034579843780829359, A_dir, 1e-6);

    double A_dif = aLayer.getAbsorptance(aSide, ScatteringSimple::Diffuse);
    EXPECT_NEAR(0.038915325073384521, A_dif, 1e-6);
}
