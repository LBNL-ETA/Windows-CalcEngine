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

TEST(TestEffectiveOpenness, Venetian0DegFrontOpenness1)
{
    SCOPED_TRACE("Begin Test: Venetian blind effective openness calculations (slat tilt = 0 deg.");

    const auto materialThickness{0.0006};   // m
    const auto slatTiltAngle{0.0};
    const auto slatWidth{0.016};      // m
    const auto slatSpacing{0.0114};   // m
    const auto slatCurvature{0.0};

    auto opennessFactor{ThermalPermeability::Venetian::frontOpenness(
      slatTiltAngle, slatSpacing, materialThickness, slatCurvature, slatWidth)};

    EXPECT_NEAR(0.95, opennessFactor, 1e-6);

    EffectiveLayers::ShadeOpenness openness{opennessFactor, 0, 0, 0, 0};

    const auto width{1.0};    // m
    const auto height{1.0};   // m

    EffectiveLayers::EffectiveHorizontalVenetian venetian{
      width, height, materialThickness, openness, slatTiltAngle, slatWidth};

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(0.688e-3, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(0.015570892, effectiveOpenness.Ah, 1e-8);
}

TEST(TestEffectiveOpenness, Venetian45DegFrontOpenness1)
{
    SCOPED_TRACE("Begin Test: Venetian blind effective openness calculations (slat tilt = 0 deg.");

    const auto materialThickness{0.0006};   // m
    const auto slatTiltAngle{45.0};
    const auto slatWidth{0.016};      // m
    const auto slatSpacing{0.0114};   // m
    const auto slatCurvature{0.0};

    auto opennessFactor{ThermalPermeability::Venetian::frontOpenness(
      slatTiltAngle, slatSpacing, materialThickness, slatCurvature, slatWidth)};

    EXPECT_NEAR(0.931845, opennessFactor, 1e-6);

    EffectiveLayers::ShadeOpenness openness{opennessFactor, 0, 0, 0, 0};

    const auto width{1.0};    // m
    const auto height{1.0};   // m

    EffectiveLayers::EffectiveHorizontalVenetian venetian{
      width, height, materialThickness, openness, slatTiltAngle, slatWidth};

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(0.504733e-3, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(0.01730331, effectiveOpenness.Ah, 1e-8);
}

TEST(TestEffectiveOpenness, Venetian0DegWithRise)
{
    SCOPED_TRACE("Begin Test: Venetian blind effective openness calculations (slat tilt = 0 deg.");

    const auto materialThickness{0.0006};   // m
    const auto slatTiltAngle{0.0};          // deg
    const auto slatWidth{0.016};            // m
    const auto slatSpacing{0.0114};         // m
    const auto slatRise{0.002};             // m

    auto opennessFactor{ThermalPermeability::Venetian::frontOpenness(
      slatTiltAngle,
      slatSpacing,
      materialThickness,
      ThermalPermeability::Venetian::calculateCurvature(slatRise, slatWidth),
      slatWidth)};

    EXPECT_NEAR(0.95, opennessFactor, 1e-6);

    EffectiveLayers::ShadeOpenness openness{opennessFactor, 0, 0, 0, 0};

    const auto width{1.0};    // m
    const auto height{1.0};   // m

    EffectiveLayers::EffectiveHorizontalVenetian venetian{
      width, height, materialThickness, openness, slatTiltAngle, slatWidth};

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(0.688e-3, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(0.015570892, effectiveOpenness.Ah, 1e-8);
}