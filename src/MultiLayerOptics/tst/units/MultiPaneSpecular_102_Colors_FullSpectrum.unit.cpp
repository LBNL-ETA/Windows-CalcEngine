/// Test color properties for single-layer SPECULAR (non-BSDF) using NFRC 102 glass.
/// Verifies Trichromatic (CIE XYZ), sRGB, CIE L*a*b*, dominant wavelength, and purity
/// for both transmittance and reflectance using full spectrum calculations.
/// This test should match the GUI "Non BSDF" results.

#include <memory>
#include <optional>
#include <gtest/gtest.h>

#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCESpectralAveraging.hpp>

#include "optical/standardData.hpp"
#include "optical/spectralSampleData.hpp"

using FenestrationCommon::CSeries;

class Test_MultiPaneSpecular_102_Colors_FullSpectrum : public testing::Test
{
    std::optional<SingleLayerOptics::ColorProperties> m_Color;

    /// Creates a single-layer SPECULAR system using NFRC 102 clear glass
    [[nodiscard]] static std::unique_ptr<MultiLayerOptics::CMultiPaneSpecular>
      createLayer(const CSeries & astmStandard)
    {
        constexpr double thickness = 3.048e-3;   // [m]
        const auto aMaterial = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_102(), thickness, FenestrationCommon::MaterialType::Monolithic);

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
        // Initialize ColorProperties with D65 illuminant and CIE 1964 10-degree observer
        auto LayerX = createLayer(StandardData::Photopic::ASTM_E308_1964_X());
        const SingleLayerOptics::ColorDetectors observer{
            StandardData::Photopic::ASTM_E308_1964_X(),
            StandardData::Photopic::ASTM_E308_1964_Y(),
            StandardData::Photopic::ASTM_E308_1964_Z()};
        m_Color.emplace(std::move(LayerX), StandardData::Photopic::solarRadiation(), observer);
    }

public:
    [[nodiscard]] SingleLayerOptics::ColorProperties & getLayer()
    {
        return m_Color.value();
    }
};

/// CIE XYZ tristimulus values for direct-direct transmittance
TEST_F(Test_MultiPaneSpecular_102_Colors_FullSpectrum, Trichromatic_T)
{
    SCOPED_TRACE("Begin Test: Trichromatic.");

    auto & aLayer = getLayer();

    constexpr auto aSide = FenestrationCommon::Side::Front;

    const auto [X, Y, Z] = aLayer.getTrichromatic(
      FenestrationCommon::PropertySurface::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(84.829226, X, 1e-6);
    EXPECT_NEAR(89.942929, Y, 1e-6);
    EXPECT_NEAR(96.526428, Z, 1e-6);
}

/// CIE XYZ tristimulus values for direct-direct reflectance
TEST_F(Test_MultiPaneSpecular_102_Colors_FullSpectrum, Trichromatic_R)
{
    SCOPED_TRACE("Begin Test: Trichromatic.");

    auto & aLayer = getLayer();

    constexpr auto aSide = FenestrationCommon::Side::Front;

    const auto [X, Y, Z] = aLayer.getTrichromatic(
      FenestrationCommon::PropertySurface::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(7.784711, X, 1e-6);
    EXPECT_NEAR(8.270063, Y, 1e-6);
    EXPECT_NEAR(9.133256, Z, 1e-6);
}

/// CIE L*a*b* color values for direct-direct transmittance
TEST_F(Test_MultiPaneSpecular_102_Colors_FullSpectrum, CIE_LAB_T)
{
    SCOPED_TRACE("Begin Test: CIE_LAB.");

    auto & aLayer = getLayer();

    constexpr auto aSide = FenestrationCommon::Side::Front;

    const auto [L, a, b] = aLayer.getCIE_Lab(
      FenestrationCommon::PropertySurface::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(95.973090, L, 1e-6);
    EXPECT_NEAR(-0.955122, a, 1e-6);
    EXPECT_NEAR(0.174787, b, 1e-6);
}

/// CIE L*a*b* color values for direct-direct reflectance
TEST_F(Test_MultiPaneSpecular_102_Colors_FullSpectrum, CIE_LAB_R)
{
    SCOPED_TRACE("Begin Test: CIE_LAB.");

    auto & aLayer = getLayer();

    constexpr auto aSide = FenestrationCommon::Side::Front;

    const auto [L, a, b] = aLayer.getCIE_Lab(
      FenestrationCommon::PropertySurface::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(34.539111, L, 1e-6);
    EXPECT_NEAR(-0.572018, a, 1e-6);
    EXPECT_NEAR(-0.756160, b, 1e-6);
}

/// Dominant wavelength and excitation purity for direct-direct transmittance
TEST_F(Test_MultiPaneSpecular_102_Colors_FullSpectrum, DominantWavelengthPurity_T)
{
    SCOPED_TRACE("Begin Test: Dominant Wavelength and Purity.");

    auto & aLayer = getLayer();

    constexpr auto aSide = FenestrationCommon::Side::Front;

    const auto [dominantWavelength, purity] = aLayer.getDominantWavelengthAndPurity(
      FenestrationCommon::PropertySurface::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(0.497812, dominantWavelength, 1e-6);
    EXPECT_NEAR(0.317418, purity, 1e-6);
}

/// Dominant wavelength and excitation purity for direct-direct reflectance
TEST_F(Test_MultiPaneSpecular_102_Colors_FullSpectrum, DominantWavelengthPurity_R)
{
    SCOPED_TRACE("Begin Test: Dominant Wavelength and Purity.");

    auto & aLayer = getLayer();

    constexpr auto aSide = FenestrationCommon::Side::Front;

    const auto [dominantWavelength, purity] = aLayer.getDominantWavelengthAndPurity(
      FenestrationCommon::PropertySurface::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(0.478947, dominantWavelength, 1e-6);
    EXPECT_NEAR(1.944590, purity, 1e-6);
}
