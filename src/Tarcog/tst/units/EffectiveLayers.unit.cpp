#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

TEST(TestEffectiveLayers, TestVenetianHorizontalEffectiveLayer)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    constexpr auto materialThickness{0.0029};   // m

    const FenestrationCommon::Venetian::Geometry geometry{0.0508, 0.012, 0.0, 0.0};

    const auto venetian =
      EffectiveLayers::makeHorizontalVenetianValues(materialThickness, geometry);

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(0.0021844, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(0.014265839, effectiveOpenness.Mfront, 1e-8);

    EXPECT_NEAR(0.805369, effectiveOpenness.PermeabilityFactor, 1e-6);
}

TEST(TestEffectiveLayers, TestVenetianHorizontalEffectiveLayer1)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    constexpr auto materialThickness{0.0029};   // m

    const FenestrationCommon::Venetian::Geometry geometry{0.0508, 0.012, 0.0, 0.0};

    const auto venetian =
      EffectiveLayers::makeHorizontalVenetianValues(materialThickness, geometry);

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(0.0021844, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(0.014265839, effectiveOpenness.Mfront, 1e-8);

    EXPECT_NEAR(0.805369, effectiveOpenness.PermeabilityFactor, 1e-6);
}

TEST(TestEffectiveLayers, TestVenetianHorizontalEffectiveLayerWithTopAndBotOpenness)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    constexpr auto materialThickness{0.0001};   // m
    constexpr FenestrationCommon::Venetian::Geometry geometry{0.0148, 0.012, 0.0, 0.0};

    const EffectiveLayers::ShadeOpenness openness{0, 0, 0.01, 0.008};

    const auto venetian =
      EffectiveLayers::makeHorizontalVenetianValues(materialThickness, geometry, openness);

    EXPECT_NEAR(6.364e-4, venetian.effectiveThickness(), 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(0.015930, effectiveOpenness.Mfront, 1e-6);
    EXPECT_NEAR(0.991736, effectiveOpenness.PermeabilityFactor, 1e-6);
}

TEST(TestEffectiveLayers, TestVenetianVerticalEffectiveLayerWithTopAndBotOpenness)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    constexpr auto materialThickness{0.0001};   // m
    const FenestrationCommon::Venetian::Geometry geometry{0.0762, 0.012, 0.0, 0.0};

    const EffectiveLayers::ShadeOpenness openness{0, 0, 0.01, 0.008};

    const auto venetian =
      EffectiveLayers::makeVerticalVenetianValues(materialThickness, geometry, openness);

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(9.144e-4, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(0.040908, effectiveOpenness.Mfront, 1e-6);
}

TEST(TestEffectiveLayers, TestEffectiveLayerCommon)
{
    SCOPED_TRACE("Begin Test: Effective layer common.");

    constexpr auto materialThickness{0.0001};   // m
    const auto permeabilityFactor{0.15};

    const auto common = EffectiveLayers::makeCommonValues(
      materialThickness, permeabilityFactor, EffectiveLayers::ShadeOpenness{0, 0, 0, 0});

    const auto effectiveThickness{common.effectiveThickness()};

    EXPECT_NEAR(0.0001, effectiveThickness, 1e-6);

    const auto effectiveOpenness{common.getEffectiveOpenness()};
    EXPECT_NEAR(8.0057885183479893e-3, effectiveOpenness.Mfront, 1e-8);
}

TEST(TestEffectiveLayers, TestVenetianVerticalEffectiveLayerWithTopAndBotOpenness45Deg)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    constexpr auto materialThickness{0.0001};   // m
    const FenestrationCommon::Venetian::Geometry geometry{0.0762, 0.012, 45.0, 0.0};

    const EffectiveLayers::ShadeOpenness openness{0, 0, 0.01, 0.008};

    const auto venetian =
      EffectiveLayers::makeVerticalVenetianValues(materialThickness, geometry, openness);

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(6.474269e-4, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(0.040870, effectiveOpenness.Mfront, 1e-6);
}

TEST(TestEffectiveLayers, TestPerforatedEffectiveOpenness)
{
    SCOPED_TRACE("Begin Test: Perforated screen effective layer properties.");

    constexpr auto materialThickness{0.0006};   // m
    const FenestrationCommon::Perforated::Geometry geometry{
      FenestrationCommon::Perforated::Type::Circular, 0.0148, 0.0148, 0.012, 0.012};

    const EffectiveLayers::ShadeOpenness openness{0.005, 0.004, 0.01, 0.008};

    const auto perforated =
      EffectiveLayers::makePerforatedValues(materialThickness, geometry, openness);

    EXPECT_NEAR(6e-4, perforated.effectiveThickness(), 1e-9);

    const auto effectiveOpenness{perforated.getEffectiveOpenness()};
    EXPECT_NEAR(0.035287, effectiveOpenness.Mfront, 1e-6);
    EXPECT_NEAR(0.516332, effectiveOpenness.PermeabilityFactor, 1e-6);
}

TEST(TestEffectiveLayer, TestLouveredShutterEffectiveOpenness)
{
    FenestrationCommon::LouveredShutter::Geometry geometry{
      .SlatWidth = 0.0889, .SlatThickness = 0.01, .SlatAngle = 87.0, .SlatSpacing = 0.0762};

    const auto louveredShutter = EffectiveLayers::makeLouveredShutterValues(geometry);

    EXPECT_NEAR(0.00161029, louveredShutter.effectiveThickness(), 1e-8);

    const auto effectiveOpenness{louveredShutter.getEffectiveOpenness()};
    EXPECT_NEAR(0.00503352, effectiveOpenness.Mfront, 1e-8);
    EXPECT_NEAR(0.295495, effectiveOpenness.PermeabilityFactor, 1e-6);
}

TEST(TestEffectiveLayers, RadiusFromRise)
{
    constexpr double curvature{23.88962765};
    constexpr double slatWidth{14.8};
    auto test{ThermalPermeability::Venetian::calculateRise(curvature, slatWidth)};
    EXPECT_NEAR(1.175, test, 1e-6);
}
