#include <gtest/gtest.h>

#include <WCETarcog.hpp>

/// Results are compared vs Robert Hart's blindOpenness.xlsx spreadsheet

TEST(TestEffectiveOpenness, Venetian0DegFrontOpenness)
{
    SCOPED_TRACE("Begin Test: Venetian blind effective openness calculations (slat tilt = 0 deg.");

    const auto materialThickness{0.0001};   // m

    const FenestrationCommon::Venetian::Geometry geometry{0.00125, 0.001, 0.0, 0.0};

    auto opennessFactor{
      ThermalPermeability::Venetian::permeabilityFactor(materialThickness, geometry)};

    EXPECT_NEAR(0.909091, opennessFactor, 1e-6);

    const auto venetian{EffectiveLayers::makeHorizontalVenetianValues(materialThickness, geometry)};

    const auto effectiveThickness{venetian.thickness};

    EXPECT_NEAR(0.05375e-03, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.openness};
    EXPECT_NEAR(0.01521184, effectiveOpenness.Mfront, 1e-8);
}

TEST(TestEffectiveOpenness, Venetian0DegFrontOpenness1)
{
    SCOPED_TRACE("Begin Test: Venetian blind effective openness calculations (slat tilt = 0 deg.");

    const auto materialThickness{0.0006};   // m

    const FenestrationCommon::Venetian::Geometry geometry{0.016, 0.0114, 0.0, 0.0};

    auto opennessFactor{
      ThermalPermeability::Venetian::permeabilityFactor(materialThickness, geometry)};

    EXPECT_NEAR(0.95, opennessFactor, 1e-6);

    const auto venetian{EffectiveLayers::makeHorizontalVenetianValues(materialThickness, geometry)};

    const auto effectiveThickness{venetian.thickness};

    EXPECT_NEAR(0.688e-3, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.openness};
    EXPECT_NEAR(0.015570892, effectiveOpenness.Mfront, 1e-8);
}

TEST(TestEffectiveOpenness, Venetian45DegFrontOpenness1)
{
    SCOPED_TRACE("Begin Test: Venetian blind effective openness calculations (slat tilt = 0 deg.");

    const auto materialThickness{0.0006};   // m

    const FenestrationCommon::Venetian::Geometry geometry{0.016, 0.0114, 45.0, 0.0};

    auto opennessFactor{
      ThermalPermeability::Venetian::permeabilityFactor(materialThickness, geometry)};

    EXPECT_NEAR(0.931845, opennessFactor, 1e-6);

    const auto venetian{EffectiveLayers::makeHorizontalVenetianValues(materialThickness, geometry)};

    const auto effectiveThickness{venetian.thickness};

    EXPECT_NEAR(0.504733e-3, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.openness};
    EXPECT_NEAR(0.01730331, effectiveOpenness.Mfront, 1e-8);
}

TEST(TestEffectiveOpenness, Venetian0DegWithRise)
{
    SCOPED_TRACE("Begin Test: Venetian blind effective openness calculations (slat tilt = 0 deg.");

    constexpr auto materialThickness{0.0006};   // m


    const FenestrationCommon::Venetian::Geometry geometry{
      0.016, 0.0114, 0.0, ThermalPermeability::Venetian::calculateCurvature(0.002, 0.016)};

    auto opennessFactor{
      ThermalPermeability::Venetian::permeabilityFactor(materialThickness, geometry)};

    EXPECT_NEAR(0.95, opennessFactor, 1e-6);

    const auto venetian{EffectiveLayers::makeHorizontalVenetianValues(materialThickness, geometry)};

    const auto effectiveThickness{venetian.thickness};

    EXPECT_NEAR(0.688e-3, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.openness};
    EXPECT_NEAR(0.015570892, effectiveOpenness.Mfront, 1e-8);
}
