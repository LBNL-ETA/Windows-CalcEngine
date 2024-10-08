#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

TEST(TestEffectiveLayers, TestVenetianHorizontalEffectiveLayer)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    const auto width{1.0};                  // m
    const auto height{1.0};                 // m
    const auto materialThickness{0.0029};   // m
    const auto slatTiltAngle{0.0};
    const auto slatWidth{0.0508};   // m

    EffectiveLayers::ShadeOpenness openness{0.937093, 0, 0, 0, 0};

    EffectiveLayers::EffectiveHorizontalVenetian venetian{
      width, height, materialThickness, openness, slatTiltAngle, slatWidth};

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(0.0021843999999999995, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(0.015458409591817657, effectiveOpenness.EffectiveFrontThermalOpennessArea, 1e-8);
}

TEST(TestEffectiveLayers, TestVenetianHorizontalEffectiveLayer1)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    const auto width{1.0};                  // m
    const auto height{1.0};                 // m
    const auto materialThickness{0.0029};   // m
    const auto slatTiltAngle{0.0};
    const auto slatWidth{0.0508};   // m

    EffectiveLayers::ShadeOpenness openness{0.991657018661499, 0, 0, 0, 0};

    EffectiveLayers::EffectiveHorizontalVenetian venetian{
      width, height, materialThickness, openness, slatTiltAngle, slatWidth};

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(0.0021843999999999995, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(1.592911e-2, effectiveOpenness.EffectiveFrontThermalOpennessArea, 1e-8);
}

TEST(TestEffectiveLayers, TestVenetianHorizontalEffectiveLayerWithTopAndBotOpenness)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    const auto width{1.3};                  // m
    const auto height{1.8};                 // m
    const auto materialThickness{0.0001};   // m
    const auto slatTiltAngle{0.0};
    const auto slatWidth{0.0148};   // m

    EffectiveLayers::ShadeOpenness openness{0.991657045, 0, 0, 0.01, 0.008};

    EffectiveLayers::EffectiveHorizontalVenetian venetian{
      width, height, materialThickness, openness, slatTiltAngle, slatWidth};

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(6.364e-4, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(3.727412206e-2, effectiveOpenness.EffectiveFrontThermalOpennessArea, 1e-8);
    EXPECT_NEAR(1.3e-2, effectiveOpenness.Atop, 1e-8);
    EXPECT_NEAR(1.04e-2, effectiveOpenness.Abot, 1e-8);
    EXPECT_NEAR(0, effectiveOpenness.Al, 1e-8);
    EXPECT_NEAR(0, effectiveOpenness.Ar, 1e-8);
}

TEST(TestEffectiveLayers, TestVenetianVerticalEffectiveLayerWithTopAndBotOpenness)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    const auto width{1.3};                  // m
    const auto height{1.8};                 // m
    const auto materialThickness{0.0001};   // m
    const auto slatTiltAngle{0.0};
    const auto slatWidth{0.0762};   // m

    EffectiveLayers::ShadeOpenness openness{0.998224968, 0, 0, 0.01, 0.008};

    EffectiveLayers::EffectiveVerticalVenetian venetian{
      width, height, materialThickness, openness, slatTiltAngle, slatWidth};

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(9.144e-4, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(9.589398567e-2, effectiveOpenness.EffectiveFrontThermalOpennessArea, 1e-8);
    EXPECT_NEAR(1.3e-2, effectiveOpenness.Atop, 1e-8);
    EXPECT_NEAR(1.04e-2, effectiveOpenness.Abot, 1e-8);
    EXPECT_NEAR(0, effectiveOpenness.Al, 1e-8);
    EXPECT_NEAR(0, effectiveOpenness.Ar, 1e-8);
}

TEST(TestEffectiveLayers, TestVenetianVerticalEffectiveLayerWithTopAndBotOpenness45Deg)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    const auto width{1.3};                  // m
    const auto height{1.8};                 // m
    const auto materialThickness{0.0001};   // m
    const auto slatTiltAngle{45.0};
    const auto slatWidth{0.0762};   // m

    EffectiveLayers::ShadeOpenness openness{0.998224966, 0, 0, 0.01, 0.008};

    EffectiveLayers::EffectiveVerticalVenetian venetian{
      width, height, materialThickness, openness, slatTiltAngle, slatWidth};

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(6.474269e-4, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(9.589398567e-2, effectiveOpenness.EffectiveFrontThermalOpennessArea, 1e-8);
    EXPECT_NEAR(1.3e-2, effectiveOpenness.Atop, 1e-8);
    EXPECT_NEAR(1.04e-2, effectiveOpenness.Abot, 1e-8);
    EXPECT_NEAR(0, effectiveOpenness.Al, 1e-8);
    EXPECT_NEAR(0, effectiveOpenness.Ar, 1e-8);
}

TEST(TestEffectiveLayers, TestPerforatedEffectiveOpenness)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    const auto width{1.3};                  // m
    const auto height{1.8};                 // m
    const auto materialThickness{0.0006};   // m

    const EffectiveLayers::ShadeOpenness openness{0.087265995, 0.005, 0.004, 0.01, 0.008};

    EffectiveLayers::EffectiveLayerPerforated perforated{
      width, height, materialThickness, openness};

    const auto effectiveThickness{perforated.effectiveThickness()};

    EXPECT_NEAR(6e-4, effectiveThickness, 1e-9);

    const auto effectiveOpenness{perforated.getEffectiveOpenness()};
    EXPECT_NEAR(9.779677e-3, effectiveOpenness.EffectiveFrontThermalOpennessArea, 1e-8);
    EXPECT_NEAR(13.0e-3, effectiveOpenness.Atop, 1e-8);
    EXPECT_NEAR(10.4e-3, effectiveOpenness.Abot, 1e-8);
    EXPECT_NEAR(9.0e-3, effectiveOpenness.Al, 1e-8);
    EXPECT_NEAR(7.2e-3, effectiveOpenness.Ar, 1e-8);
}

TEST(TestEffectiveLayers, TestOtherShadingEffectiveOpenness)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    const auto width{1.3};                  // m
    const auto height{1.8};                 // m
    const auto materialThickness{0.0006};   // m

    EffectiveLayers::ShadeOpenness openness{0.087265995, 0.005, 0.004, 0.01, 0.008};

    constexpr double PermeabilityFactor{0.0};

    EffectiveLayers::EffectiveLayerOther perforated{
      width, height, materialThickness, openness, PermeabilityFactor};

    const auto effectiveThickness{perforated.effectiveThickness()};

    EXPECT_NEAR(6e-4, effectiveThickness, 1e-9);

    const auto effectiveOpenness{perforated.getEffectiveOpenness()};
    EXPECT_NEAR(0.2042024283, effectiveOpenness.EffectiveFrontThermalOpennessArea, 1e-8);
    EXPECT_NEAR(0.013, effectiveOpenness.Atop, 1e-8);
    EXPECT_NEAR(0.0104, effectiveOpenness.Abot, 1e-8);
    EXPECT_NEAR(0.009, effectiveOpenness.Al, 1e-8);
    EXPECT_NEAR(0.0072, effectiveOpenness.Ar, 1e-8);
}

TEST(TestEffectiveLayers, RadiusFromRise)
{
    double curvature{23.88962765};
    double slatWidth{14.8};
    auto test{ThermalPermeability::Venetian::calculateRise(curvature, slatWidth)};
    EXPECT_NEAR(1.175, test, 1e-6);
}