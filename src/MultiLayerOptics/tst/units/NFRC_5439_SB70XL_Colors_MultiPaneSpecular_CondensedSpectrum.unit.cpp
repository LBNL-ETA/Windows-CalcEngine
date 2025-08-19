#include <memory>
#include <gtest/gtest.h>

#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCESpectralAveraging.hpp>

#include "standardData.hpp"
#include "spectralSampleData.hpp"

using FenestrationCommon::CSeries;

class TestNFRC_5439_SB70XL_Colors_MultiPaneSpecular_CondensedSpectrum : public testing::Test
{
    std::shared_ptr<SingleLayerOptics::ColorProperties> m_Color;

    [[nodiscard]] std::unique_ptr<MultiLayerOptics::CMultiPaneSpecular>
      createLayer(const CSeries & astmStandard) const
    {
        double thickness = 3.048e-3;   // [m]
        const auto aMaterial = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_5439(), thickness, FenestrationCommon::MaterialType::Monolithic);

        auto single_layer = SingleLayerOptics::SpecularLayer::createLayer(aMaterial);

        single_layer->setSourceData(StandardData::Photopic::solarRadiation());

        auto layer = MultiLayerOptics::CMultiPaneSpecular::create({single_layer});

        const SingleLayerOptics::CalculationProperties input{
          StandardData::Photopic::solarRadiation(),
          StandardData::Photopic::wavelengthSetPhotopic(),
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
          StandardData::Photopic::ASTM_E308_1964_Z(),
          StandardData::condensedSpectrumDefault());
    }

public:
    [[nodiscard]] std::shared_ptr<SingleLayerOptics::ColorProperties> getLayer() const
    {
        return m_Color;
    }
};

TEST_F(TestNFRC_5439_SB70XL_Colors_MultiPaneSpecular_CondensedSpectrum, TestTrichromatic_T)
{
    SCOPED_TRACE("Begin Test: Trichromatic.");

    std::shared_ptr<SingleLayerOptics::ColorProperties> aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    SingleLayerOptics::Trichromatic T = aLayer->getTrichromatic(
      FenestrationCommon::PropertySimple::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(70.406305498022462, T.X, 1e-6);
    EXPECT_NEAR(71.820821122822124, T.Y, 1e-6);
    EXPECT_NEAR(94.292998579387429, T.Z, 1e-6);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_MultiPaneSpecular_CondensedSpectrum, TestTrichromatic_R)
{
    SCOPED_TRACE("Begin Test: Trichromatic.");

    std::shared_ptr<SingleLayerOptics::ColorProperties> aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    SingleLayerOptics::Trichromatic T = aLayer->getTrichromatic(
      FenestrationCommon::PropertySimple::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(6.5758982835767874, T.X, 1e-6);
    EXPECT_NEAR(7.4806100470829309, T.Y, 1e-6);
    EXPECT_NEAR(11.453114017922104, T.Z, 1e-6);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_MultiPaneSpecular_CondensedSpectrum, TestRGB_T)
{
    SCOPED_TRACE("Begin Test: RGB.");

    std::shared_ptr<SingleLayerOptics::ColorProperties> aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    auto rgb = aLayer->getRGB(
      FenestrationCommon::PropertySimple::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_EQ(242, rgb.R);
    EXPECT_EQ(241, rgb.G);
    EXPECT_EQ(255, rgb.B);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_MultiPaneSpecular_CondensedSpectrum, TestRGB_R)
{
    SCOPED_TRACE("Begin Test: RGB.");

    std::shared_ptr<SingleLayerOptics::ColorProperties> aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    auto rgb = aLayer->getRGB(
      FenestrationCommon::PropertySimple::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_EQ(62, rgb.R);
    EXPECT_EQ(89, rgb.G);
    EXPECT_EQ(103, rgb.B);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_MultiPaneSpecular_CondensedSpectrum, TestCIE_LAB_T)
{
    SCOPED_TRACE("Begin Test: CIE_LAB.");

    std::shared_ptr<SingleLayerOptics::ColorProperties> aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    auto cie = aLayer->getCIE_Lab(
      FenestrationCommon::PropertySimple::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(87.882273222125605, cie.L, 1e-6);
    EXPECT_NEAR(-3.0076116041811152, cie.a, 1e-6);
    EXPECT_NEAR(2.717350587299272, cie.b, 1e-6);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_MultiPaneSpecular_CondensedSpectrum, TestCIE_LAB_R)
{
    SCOPED_TRACE("Begin Test: CIE_LAB.");

    std::shared_ptr<SingleLayerOptics::ColorProperties> aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    auto cie = aLayer->getCIE_Lab(
      FenestrationCommon::PropertySimple::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(32.87690091884874, cie.L, 1e-6);
    EXPECT_NEAR(-8.8825582067682536, cie.a, 1e-6);
    EXPECT_NEAR(-3.0846715755109333, cie.b, 1e-6);
}
