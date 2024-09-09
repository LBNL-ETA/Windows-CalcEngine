#include <gtest/gtest.h>

#include <WCETarcog.hpp>

/// Results are compared vs Robert Hart's blindOpenness.xlsx spreadsheet

TEST(TestEffectiveOpenness, Venetian0DegFrontOpenness)
{
    SCOPED_TRACE("Begin Test: Venetian blind effective openness calculations (slat tilt = 0 deg.");

    const auto materialThickness{0.0001};   // m
    const auto slatTiltAngle{0.0};
    const auto slatWidth{0.00125};   // m
    const auto slatSpacing{0.001};   // m
    const auto slatCurvature{0.0};

    auto opennessFactor{ThermalPermeability::Venetian::frontOpenness(
      slatTiltAngle, slatSpacing, materialThickness, slatCurvature, slatWidth)};

    EXPECT_NEAR(0.909091, opennessFactor, 1e-6);

    EffectiveLayers::ShadeOpenness openness{opennessFactor, 0, 0, 0, 0};

    const auto width{1.0};    // m
    const auto height{1.0};   // m

    EffectiveLayers::EffectiveHorizontalVenetian venetian{
      width, height, materialThickness, openness, slatTiltAngle, slatWidth};

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(0.05375e-03, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(0.01521184, effectiveOpenness.Ah, 1e-8);
}