#include <memory>
#include <optional>
#include <gtest/gtest.h>

#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCESpectralAveraging.hpp>

#include "optical/standardData.hpp"
#include "optical/spectralSampleData.hpp"

using FenestrationCommon::CSeries;

class Test_MultiPaneBSDF_102_Colors_FullSpectrum : public testing::Test
{
    std::optional<SingleLayerOptics::ColorProperties> m_Color;

    [[nodiscard]] static std::unique_ptr<MultiLayerOptics::CMultiPaneBSDF> createLayer()
    {
        const double thickness = 3.048e-3;   // [m]
        const auto aMaterial = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_102(), thickness, FenestrationCommon::MaterialType::Monolithic);

        const auto aBSDF =
          SingleLayerOptics::BSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Quarter);
        auto layer_102 = SingleLayerOptics::CBSDFLayerMaker::getSpecularLayer(aMaterial, aBSDF);

        return MultiLayerOptics::CMultiPaneBSDF::create({layer_102});
    }

protected:
    void SetUp() override
    {
        m_Color.emplace(createLayer(),
                        StandardData::Photopic::solarRadiation(),
                        StandardData::Photopic::ASTM_E308_1964_X(),
                        StandardData::Photopic::ASTM_E308_1964_Y(),
                        StandardData::Photopic::ASTM_E308_1964_Z());
    }

public:
    [[nodiscard]] SingleLayerOptics::ColorProperties & getLayer()
    {
        return m_Color.value();
    }
};

TEST_F(Test_MultiPaneBSDF_102_Colors_FullSpectrum, TestTrichromatic_T)
{
    SCOPED_TRACE("Begin Test: Trichromatic.");

    auto & aLayer = getLayer();

    const auto aSide = FenestrationCommon::Side::Front;

    const auto tri = aLayer.getTrichromatic(
      FenestrationCommon::PropertySurface::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(84.829226, tri.X, 1e-6);
    EXPECT_NEAR(89.942929, tri.Y, 1e-6);
    EXPECT_NEAR(96.526428, tri.Z, 1e-6);
}

TEST_F(Test_MultiPaneBSDF_102_Colors_FullSpectrum, TestTrichromatic_R)
{
    SCOPED_TRACE("Begin Test: Trichromatic.");

    auto & aLayer = getLayer();

    const auto aSide = FenestrationCommon::Side::Front;

    const auto tri = aLayer.getTrichromatic(
      FenestrationCommon::PropertySurface::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(7.784711, tri.X, 1e-6);
    EXPECT_NEAR(8.270063, tri.Y, 1e-6);
    EXPECT_NEAR(9.133256, tri.Z, 1e-6);
}

TEST_F(Test_MultiPaneBSDF_102_Colors_FullSpectrum, TestRGB_T)
{
    SCOPED_TRACE("Begin Test: RGB.");

    auto & aLayer = getLayer();

    const auto aSide = FenestrationCommon::Side::Front;

    const auto rgb = aLayer.getRGB(
      FenestrationCommon::PropertySurface::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_EQ(255, rgb.R);
    EXPECT_EQ(255, rgb.G);
    EXPECT_EQ(255, rgb.B);
}

TEST_F(Test_MultiPaneBSDF_102_Colors_FullSpectrum, TestRGB_R)
{
    SCOPED_TRACE("Begin Test: RGB.");

    auto & aLayer = getLayer();

    const auto aSide = FenestrationCommon::Side::Front;

    const auto rgb = aLayer.getRGB(
      FenestrationCommon::PropertySurface::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_EQ(88, rgb.R);
    EXPECT_EQ(90, rgb.G);
    EXPECT_EQ(90, rgb.B);
}

TEST_F(Test_MultiPaneBSDF_102_Colors_FullSpectrum, TestCIE_LAB_T)
{
    SCOPED_TRACE("Begin Test: CIE_LAB.");

    auto & aLayer = getLayer();

    const auto aSide = FenestrationCommon::Side::Front;

    const auto cie = aLayer.getCIE_Lab(
      FenestrationCommon::PropertySurface::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(95.973090, cie.L, 1e-6);
    EXPECT_NEAR(-0.955122, cie.a, 1e-6);
    EXPECT_NEAR(0.174787, cie.b, 1e-6);
}

TEST_F(Test_MultiPaneBSDF_102_Colors_FullSpectrum, TestCIE_LAB_R)
{
    SCOPED_TRACE("Begin Test: CIE_LAB.");

    auto & aLayer = getLayer();

    const auto aSide = FenestrationCommon::Side::Front;

    const auto cie = aLayer.getCIE_Lab(
      FenestrationCommon::PropertySurface::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(34.539111, cie.L, 1e-6);
    EXPECT_NEAR(-0.572018, cie.a, 1e-6);
    EXPECT_NEAR(-0.756160, cie.b, 1e-6);
}
