#include <memory>
#include <gtest/gtest.h>

#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCESpectralAveraging.hpp>

#include "optical/standardData.hpp"
#include "optical/spectralSampleData.hpp"

using FenestrationCommon::CSeries;

class TestNFRC_5439_SB70XL_Colors_MultiPaneBSDF_FullSpectrum : public testing::Test
{
    std::shared_ptr<SingleLayerOptics::ColorProperties> m_Color;

    [[nodiscard]] static std::unique_ptr<MultiLayerOptics::CMultiPaneBSDF> createLayer()
    {
        double thickness = 3.048e-3;   // [m]
        const auto aMaterial = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_5439(), thickness, FenestrationCommon::MaterialType::Monolithic);

        const auto aBSDF =
          SingleLayerOptics::BSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Quarter);
        auto layer_5439 = SingleLayerOptics::CBSDFLayerMaker::getSpecularLayer(aMaterial, aBSDF);

        auto layer = MultiLayerOptics::CMultiPaneBSDF::create({layer_5439});

        return layer;
    }

protected:
    void SetUp() override
    {
        m_Color = std::make_shared<SingleLayerOptics::ColorProperties>(
          std::move(createLayer()),
          StandardData::Photopic::solarRadiation(),
          StandardData::Photopic::ASTM_E308_1964_X(),
          StandardData::Photopic::ASTM_E308_1964_Y(),
          StandardData::Photopic::ASTM_E308_1964_Z(),
          StandardData::Photopic::CIE_1964_Locus_X(),
          StandardData::Photopic::CIE_1964_Locus_Y());
    }

public:
    [[nodiscard]] std::shared_ptr<SingleLayerOptics::ColorProperties> getLayer() const
    {
        return m_Color;
    }
};

TEST_F(TestNFRC_5439_SB70XL_Colors_MultiPaneBSDF_FullSpectrum, TestTrichromatic_T)
{
    SCOPED_TRACE("Begin Test: Trichromatic.");

    std::shared_ptr<SingleLayerOptics::ColorProperties> aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    SingleLayerOptics::Trichromatic T = aLayer->getTrichromatic(
      FenestrationCommon::PropertySurface::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(66.479550507930455, T.X, 1e-6);
    EXPECT_NEAR(71.676433571429115, T.Y, 1e-6);
    EXPECT_NEAR(72.036556794335624, T.Z, 1e-6);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_MultiPaneBSDF_FullSpectrum, TestTrichromatic_R)
{
    SCOPED_TRACE("Begin Test: Trichromatic.");

    std::shared_ptr<SingleLayerOptics::ColorProperties> aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    SingleLayerOptics::Trichromatic T = aLayer->getTrichromatic(
      FenestrationCommon::PropertySurface::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(6.9615975925406062, T.X, 1e-6);
    EXPECT_NEAR(7.6320645694529237, T.Y, 1e-6);
    EXPECT_NEAR(10.144263012183918, T.Z, 1e-6);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_MultiPaneBSDF_FullSpectrum, TestRGB_T)
{
    SCOPED_TRACE("Begin Test: RGB.");

    std::shared_ptr<SingleLayerOptics::ColorProperties> aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    auto rgb = aLayer->getRGB(
      FenestrationCommon::PropertySurface::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_EQ(239, rgb.R);
    EXPECT_EQ(245, rgb.G);
    EXPECT_EQ(233, rgb.B);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_MultiPaneBSDF_FullSpectrum, TestRGB_R)
{
    SCOPED_TRACE("Begin Test: RGB.");

    std::shared_ptr<SingleLayerOptics::ColorProperties> aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    auto rgb = aLayer->getRGB(
      FenestrationCommon::PropertySurface::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_EQ(75, rgb.R);
    EXPECT_EQ(88, rgb.G);
    EXPECT_EQ(96, rgb.B);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_MultiPaneBSDF_FullSpectrum, TestCIE_LAB_T)
{
    SCOPED_TRACE("Begin Test: CIE_LAB.");

    std::shared_ptr<SingleLayerOptics::ColorProperties> aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    auto cie = aLayer->getCIE_Lab(
      FenestrationCommon::PropertySurface::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(87.812612040616273, cie.L, 1e-6);
    EXPECT_NEAR(-3.3693813072975809, cie.a, 1e-6);
    EXPECT_NEAR(4.0316540801110357, cie.b, 1e-6);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_MultiPaneBSDF_FullSpectrum, TestCIE_LAB_R)
{
    SCOPED_TRACE("Begin Test: CIE_LAB.");

    std::shared_ptr<SingleLayerOptics::ColorProperties> aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    auto cie = aLayer->getCIE_Lab(
      FenestrationCommon::PropertySurface::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(33.204557956508474, cie.L, 1e-6);
    EXPECT_NEAR(-2.764212021208273, cie.a, 1e-6);
    EXPECT_NEAR(-6.1873961643050261, cie.b, 1e-6);
}
