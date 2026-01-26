/// Test color properties for single-layer BSDF using NFRC 102 glass.
/// Verifies Trichromatic (CIE XYZ), sRGB, and CIE L*a*b* color values
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

class Test_MultiPaneBSDF_102_Colors_FullSpectrum : public testing::Test
{
    std::optional<SingleLayerOptics::ColorProperties> m_Color;

    /// Creates a single-layer BSDF system using NFRC 102 clear glass
    [[nodiscard]] static std::unique_ptr<MultiLayerOptics::CMultiPaneBSDF> createLayer()
    {
        constexpr double thickness = 3.048e-3;   // [m]
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
        // Initialize ColorProperties with D65 illuminant and CIE 1964 10-degree observer
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

/// CIE XYZ tristimulus values for direct-direct transmittance
TEST_F(Test_MultiPaneBSDF_102_Colors_FullSpectrum, Trichromatic_T)
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
TEST_F(Test_MultiPaneBSDF_102_Colors_FullSpectrum, Trichromatic_R)
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

/// sRGB color values (0-255) for direct-direct transmittance
TEST_F(Test_MultiPaneBSDF_102_Colors_FullSpectrum, RGB_T)
{
    SCOPED_TRACE("Begin Test: RGB.");

    auto & aLayer = getLayer();

    constexpr auto aSide = FenestrationCommon::Side::Front;

    const auto [R, G, B] = aLayer.getRGB(
      FenestrationCommon::PropertySurface::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_EQ(255, R);
    EXPECT_EQ(255, G);
    EXPECT_EQ(255, B);
}

/// sRGB color values (0-255) for direct-direct reflectance
TEST_F(Test_MultiPaneBSDF_102_Colors_FullSpectrum, RGB_R)
{
    SCOPED_TRACE("Begin Test: RGB.");

    auto & aLayer = getLayer();

    constexpr auto aSide = FenestrationCommon::Side::Front;

    const auto [R, G, B] = aLayer.getRGB(
      FenestrationCommon::PropertySurface::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_EQ(88, R);
    EXPECT_EQ(90, G);
    EXPECT_EQ(90, B);
}

/// CIE L*a*b* color values for direct-direct transmittance
/// L* = lightness, a* = green-red axis, b* = blue-yellow axis
TEST_F(Test_MultiPaneBSDF_102_Colors_FullSpectrum, CIE_LAB_T)
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
TEST_F(Test_MultiPaneBSDF_102_Colors_FullSpectrum, CIE_LAB_R)
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
