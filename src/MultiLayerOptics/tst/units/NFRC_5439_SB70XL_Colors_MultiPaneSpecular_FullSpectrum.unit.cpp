#include <memory>
#include <gtest/gtest.h>

#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>

#include "standardData.hpp"
#include "spectralSampleData.hpp"

using FenestrationCommon::CSeries;

class TestNFRC_5439_SB70XL_Colors_MultiPaneSpecular_FullSpectrum : public testing::Test
{
    std::shared_ptr<SingleLayerOptics::ColorProperties> m_Color;

    [[nodiscard]] static std::unique_ptr<MultiLayerOptics::CMultiPaneSpecular>
      createLayer(const CSeries & astmStandard)
    {
        double thickness = 3.048e-3;   // [m]
        const auto aMaterial = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_5439(), thickness, FenestrationCommon::MaterialType::Monolithic);

        auto single_layer = SingleLayerOptics::SpecularLayer::createLayer(aMaterial);

        single_layer->setSourceData(StandardData::Photopic::solarRadiation());

        auto layer = MultiLayerOptics::CMultiPaneSpecular::create({single_layer});

        const SingleLayerOptics::CalculationProperties input{
          StandardData::Photopic::solarRadiation(),
          StandardData::condensedSpectrumDefault(),
          astmStandard};
        layer->setCalculationProperties(input);

        return layer;
    }

protected:
    void SetUp() override
    {
        auto LayerX = createLayer(StandardData::Photopic::ASTM_E308_1964_X());

        m_Color = std::make_shared<SingleLayerOptics::ColorProperties>(
          std::move(LayerX),
          StandardData::Photopic::solarRadiation(),
          StandardData::Photopic::ASTM_E308_1964_X(),
          StandardData::Photopic::ASTM_E308_1964_Y(),
          StandardData::Photopic::ASTM_E308_1964_Z());
    }

public:
    [[nodiscard]] std::shared_ptr<SingleLayerOptics::ColorProperties> getLayer() const
    {
        return m_Color;
    }
};

TEST_F(TestNFRC_5439_SB70XL_Colors_MultiPaneSpecular_FullSpectrum, TestTrichromatic_T)
{
    SCOPED_TRACE("Begin Test: Trichromatic.");

    std::shared_ptr<SingleLayerOptics::ColorProperties> aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    SingleLayerOptics::Trichromatic T = aLayer->getTrichromatic(
      FenestrationCommon::PropertySimple::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(66.479550507930426, T.X, 1e-6);
    EXPECT_NEAR(71.676433571429129, T.Y, 1e-6);
    EXPECT_NEAR(72.036556794335624, T.Z, 1e-6);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_MultiPaneSpecular_FullSpectrum, TestTrichromatic_R)
{
    SCOPED_TRACE("Begin Test: Trichromatic.");

    std::shared_ptr<SingleLayerOptics::ColorProperties> aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    SingleLayerOptics::Trichromatic T = aLayer->getTrichromatic(
      FenestrationCommon::PropertySimple::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(6.9615975925406008, T.X, 1e-6);
    EXPECT_NEAR(7.6320645694529219, T.Y, 1e-6);
    EXPECT_NEAR(10.144263012183917, T.Z, 1e-6);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_MultiPaneSpecular_FullSpectrum, TestRGB_T)
{
    SCOPED_TRACE("Begin Test: RGB.");

    std::shared_ptr<SingleLayerOptics::ColorProperties> aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    auto rgb = aLayer->getRGB(
      FenestrationCommon::PropertySimple::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_EQ(239, rgb.R);
    EXPECT_EQ(245, rgb.G);
    EXPECT_EQ(233, rgb.B);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_MultiPaneSpecular_FullSpectrum, TestRGB_R)
{
    SCOPED_TRACE("Begin Test: RGB.");

    std::shared_ptr<SingleLayerOptics::ColorProperties> aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    auto rgb = aLayer->getRGB(
      FenestrationCommon::PropertySimple::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_EQ(75, rgb.R);
    EXPECT_EQ(88, rgb.G);
    EXPECT_EQ(96, rgb.B);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_MultiPaneSpecular_FullSpectrum, TestCIE_LAB_T)
{
    SCOPED_TRACE("Begin Test: CIE_LAB.");

    std::shared_ptr<SingleLayerOptics::ColorProperties> aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    auto cie = aLayer->getCIE_Lab(
      FenestrationCommon::PropertySimple::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(87.812612040616273, cie.L, 1e-6);
    EXPECT_NEAR(-3.3693813072976919, cie.a, 1e-6);
    EXPECT_NEAR(4.0316540801110579, cie.b, 1e-6);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_MultiPaneSpecular_FullSpectrum, TestCIE_LAB_R)
{
    SCOPED_TRACE("Begin Test: CIE_LAB.");

    std::shared_ptr<SingleLayerOptics::ColorProperties> aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    auto cie = aLayer->getCIE_Lab(
      FenestrationCommon::PropertySimple::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(33.204557956508467, cie.L, 1e-6);
    EXPECT_NEAR(-2.764212021208301, cie.a, 1e-6);
    EXPECT_NEAR(-6.1873961643050368, cie.b, 1e-6);
}
