/// Test color properties for double-layer BSDF using NFRC 102 and 103 glass.
/// Verifies Trichromatic (CIE XYZ), sRGB, CIE L*a*b*, dominant wavelength, and purity
/// for both transmittance and reflectance using full spectrum calculations.

#include <memory>
#include <optional>
#include <gtest/gtest.h>

#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCESpectralAveraging.hpp>

#include "optical/standardData.hpp"
#include "optical/spectralSampleData.hpp"

using FenestrationCommon::CSeries;

class Test_MultiPaneBSDF_102_103_Colors_FullSpectrum : public testing::Test
{
    std::optional<SingleLayerOptics::ColorProperties> m_Color;

    /// Creates a double-layer BSDF system using NFRC 102 and 103 glass
    [[nodiscard]] static std::unique_ptr<MultiLayerOptics::CMultiPaneBSDF> createLayer()
    {
        constexpr double thickness_102 = 3.048e-3;   // [m]
        const auto aMaterial_102 = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_102(), thickness_102, FenestrationCommon::MaterialType::Monolithic);

        constexpr double thickness_103 = 5.715e-3;   // [m]
        const auto aMaterial_103 = SingleLayerOptics::Material::nBandMaterial(
          SpectralSample::NFRC_103(), thickness_103, FenestrationCommon::MaterialType::Monolithic);

        const auto aBSDF =
          SingleLayerOptics::BSDFHemisphere::create(SingleLayerOptics::BSDFBasis::Quarter);
        auto layer_102 = SingleLayerOptics::CBSDFLayerMaker::getSpecularLayer(aMaterial_102, aBSDF);
        auto layer_103 = SingleLayerOptics::CBSDFLayerMaker::getSpecularLayer(aMaterial_103, aBSDF);

        return MultiLayerOptics::CMultiPaneBSDF::create({layer_102, layer_103});
    }

protected:
    void SetUp() override
    {
        // Initialize ColorProperties with D65 illuminant and CIE 1964 10-degree observer
        const SingleLayerOptics::ColorDetectors observer{
            StandardData::Photopic::ASTM_E308_1964_X(),
            StandardData::Photopic::ASTM_E308_1964_Y(),
            StandardData::Photopic::ASTM_E308_1964_Z()};
        m_Color.emplace(createLayer(), StandardData::Photopic::solarRadiation(), observer);
    }

public:
    [[nodiscard]] SingleLayerOptics::ColorProperties & getLayer()
    {
        return m_Color.value();
    }
};

/// CIE XYZ tristimulus values for direct-direct transmittance
TEST_F(Test_MultiPaneBSDF_102_103_Colors_FullSpectrum, Trichromatic_T)
{
    SCOPED_TRACE("Begin Test: Trichromatic.");

    auto & aLayer = getLayer();

    constexpr auto aSide = FenestrationCommon::Side::Front;

    const auto [X, Y, Z] = aLayer.getTrichromatic(
      FenestrationCommon::PropertySurface::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(74.652963, X, 1e-6);
    EXPECT_NEAR(80.056486, Y, 1e-6);
    EXPECT_NEAR(85.700573, Z, 1e-6);
}

/// CIE XYZ tristimulus values for direct-direct reflectance
TEST_F(Test_MultiPaneBSDF_102_103_Colors_FullSpectrum, Trichromatic_R)
{
    SCOPED_TRACE("Begin Test: Trichromatic.");

    auto & aLayer = getLayer();

    constexpr auto aSide = FenestrationCommon::Side::Front;

    const auto [X, Y, Z] = aLayer.getTrichromatic(
      FenestrationCommon::PropertySurface::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(13.863083, X, 1e-6);
    EXPECT_NEAR(14.830379, Y, 1e-6);
    EXPECT_NEAR(16.298601, Z, 1e-6);
}

/// sRGB color values (0-255) for direct-direct transmittance
TEST_F(Test_MultiPaneBSDF_102_103_Colors_FullSpectrum, RGB_T)
{
    SCOPED_TRACE("Begin Test: RGB.");

    auto & aLayer = getLayer();

    constexpr auto aSide = FenestrationCommon::Side::Front;

    const auto [R, G, B] = aLayer.getRGB(
      FenestrationCommon::PropertySurface::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_EQ(249, R);
    EXPECT_EQ(255, G);
    EXPECT_EQ(253, B);
}

/// sRGB color values (0-255) for direct-direct reflectance
TEST_F(Test_MultiPaneBSDF_102_103_Colors_FullSpectrum, RGB_R)
{
    SCOPED_TRACE("Begin Test: RGB.");

    auto & aLayer = getLayer();

    constexpr auto aSide = FenestrationCommon::Side::Front;

    const auto [R, G, B] = aLayer.getRGB(
      FenestrationCommon::PropertySurface::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_EQ(115, R);
    EXPECT_EQ(120, G);
    EXPECT_EQ(119, B);
}

/// CIE L*a*b* color values for direct-direct transmittance
/// L* = lightness, a* = green-red axis, b* = blue-yellow axis
TEST_F(Test_MultiPaneBSDF_102_103_Colors_FullSpectrum, CIE_LAB_T)
{
    SCOPED_TRACE("Begin Test: CIE_LAB.");

    auto & aLayer = getLayer();

    constexpr auto aSide = FenestrationCommon::Side::Front;

    const auto [L, a, b] = aLayer.getCIE_Lab(
      FenestrationCommon::PropertySurface::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(91.710199, L, 1e-6);
    EXPECT_NEAR(-2.667996, a, 1e-6);
    EXPECT_NEAR(0.323576, b, 1e-6);
}

/// CIE L*a*b* color values for direct-direct reflectance
TEST_F(Test_MultiPaneBSDF_102_103_Colors_FullSpectrum, CIE_LAB_R)
{
    SCOPED_TRACE("Begin Test: CIE_LAB.");

    auto & aLayer = getLayer();

    constexpr auto aSide = FenestrationCommon::Side::Front;

    const auto [L, a, b] = aLayer.getCIE_Lab(
      FenestrationCommon::PropertySurface::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(45.400994, L, 1e-6);
    EXPECT_NEAR(-1.307335, a, 1e-6);
    EXPECT_NEAR(-0.745165, b, 1e-6);
}

/// Dominant wavelength and excitation purity for direct-direct transmittance
TEST_F(Test_MultiPaneBSDF_102_103_Colors_FullSpectrum, DominantWavelengthPurity_T)
{
    SCOPED_TRACE("Begin Test: Dominant Wavelength and Purity.");

    auto & aLayer = getLayer();

    constexpr auto aSide = FenestrationCommon::Side::Front;

    const auto [dominantWavelength, purity] = aLayer.getDominantWavelengthAndPurity(
      FenestrationCommon::PropertySurface::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(0.495170, dominantWavelength, 1e-6);
    EXPECT_NEAR(1.021651, purity, 1e-6);
}

/// Dominant wavelength and excitation purity for direct-direct reflectance
TEST_F(Test_MultiPaneBSDF_102_103_Colors_FullSpectrum, DominantWavelengthPurity_R)
{
    SCOPED_TRACE("Begin Test: Dominant Wavelength and Purity.");

    auto & aLayer = getLayer();

    constexpr auto aSide = FenestrationCommon::Side::Front;

    const auto [dominantWavelength, purity] = aLayer.getDominantWavelengthAndPurity(
      FenestrationCommon::PropertySurface::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(0.482873, dominantWavelength, 1e-6);
    EXPECT_NEAR(2.154352, purity, 1e-6);
}
