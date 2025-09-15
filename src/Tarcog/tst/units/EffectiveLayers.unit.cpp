#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

TEST(TestEffectiveLayers, TestVenetianHorizontalEffectiveLayer)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    const auto width{1.0};                  // m
    const auto height{1.0};                 // m
    const auto materialThickness{0.0029};   // m

    const FenestrationCommon::Venetian::Geometry geometry{0.0508, 0.012, 0.0, 0.0};

    EffectiveLayers::EffectiveHorizontalVenetian venetian{
      width, height, materialThickness, geometry};

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(0.0021844, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(0.014265839, effectiveOpenness.EffectiveFrontThermalOpennessArea, 1e-8);

    EXPECT_NEAR(0.805369, effectiveOpenness.PermeabilityFactor, 1e-6);
}

TEST(TestEffectiveLayers, TestVenetianHorizontalEffectiveLayer1)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    const auto width{1.0};                  // m
    const auto height{1.0};                 // m
    const auto materialThickness{0.0029};   // m

    const FenestrationCommon::Venetian::Geometry geometry{0.0508, 0.012, 0.0, 0.0};

    EffectiveLayers::EffectiveHorizontalVenetian venetian{
      width, height, materialThickness, geometry};

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(0.0021844, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(0.014265839, effectiveOpenness.EffectiveFrontThermalOpennessArea, 1e-8);

    EXPECT_NEAR(0.805369, effectiveOpenness.PermeabilityFactor, 1e-6);
}

TEST(TestEffectiveLayers, TestVenetianHorizontalEffectiveLayerWithTopAndBotOpenness)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    const auto width{1.3};                  // m
    const auto height{1.8};                 // m
    const auto materialThickness{0.0001};   // m
    const FenestrationCommon::Venetian::Geometry geometry{0.0148, 0.012, 0.0, 0.0};

    EffectiveLayers::ShadeOpenness openness{0, 0, 0.01, 0.008};

    EffectiveLayers::EffectiveHorizontalVenetian venetian{
      width, height, materialThickness, geometry, openness};

    EXPECT_NEAR(6.364e-4, venetian.effectiveThickness(), 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(3.727569e-2, effectiveOpenness.EffectiveFrontThermalOpennessArea, 1e-8);
    EXPECT_NEAR(0.991736, effectiveOpenness.PermeabilityFactor, 1e-6);
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
    const FenestrationCommon::Venetian::Geometry geometry{0.0762, 0.012, 0.0, 0.0};

    EffectiveLayers::ShadeOpenness openness{0, 0, 0.01, 0.008};

    EffectiveLayers::EffectiveVerticalVenetian venetian{
      width, height, materialThickness, geometry, openness};

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(9.144e-4, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(9.572527e-2, effectiveOpenness.EffectiveFrontThermalOpennessArea, 1e-8);
    EXPECT_NEAR(1.3e-2, effectiveOpenness.Atop, 1e-8);
    EXPECT_NEAR(1.04e-2, effectiveOpenness.Abot, 1e-8);
    EXPECT_NEAR(0, effectiveOpenness.Al, 1e-8);
    EXPECT_NEAR(0, effectiveOpenness.Ar, 1e-8);
}

TEST(TestEffectiveLayers, TestEffectiveLayerCommon)
{
    SCOPED_TRACE("Begin Test: Effective layer common.");

    const auto width{1.0};                  // m
    const auto height{1.0};                 // m
    const auto materialThickness{0.0001};   // m

    const auto permeabilityFactor{0.15};

    EffectiveLayers::EffectiveLayerCommon common{
      width, height, materialThickness, permeabilityFactor};

    const auto effectiveThickness{common.effectiveThickness()};

    EXPECT_NEAR(0.0001, effectiveThickness, 1e-6);

    const auto effectiveOpenness{common.getEffectiveOpenness()};
    EXPECT_NEAR(8.0057885183479893e-3, effectiveOpenness.EffectiveFrontThermalOpennessArea, 1e-8);
    EXPECT_NEAR(0, effectiveOpenness.Atop, 1e-8);
    EXPECT_NEAR(0, effectiveOpenness.Abot, 1e-8);
    EXPECT_NEAR(0, effectiveOpenness.Al, 1e-8);
    EXPECT_NEAR(0, effectiveOpenness.Ar, 1e-8);
}

TEST(TestEffectiveLayers, TestVenetianVerticalEffectiveLayerWithTopAndBotOpenness45Deg)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    const auto width{1.3};                  // m
    const auto height{1.8};                 // m
    const auto materialThickness{0.0001};   // m
    const FenestrationCommon::Venetian::Geometry geometry{0.0762, 0.012, 45.0, 0.0};

    EffectiveLayers::ShadeOpenness openness{0, 0, 0.01, 0.008};

    EffectiveLayers::EffectiveVerticalVenetian venetian{
      width, height, materialThickness, geometry, openness};

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(6.474269e-4, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(9.563634e-2, effectiveOpenness.EffectiveFrontThermalOpennessArea, 1e-8);
    EXPECT_NEAR(1.3e-2, effectiveOpenness.Atop, 1e-8);
    EXPECT_NEAR(1.04e-2, effectiveOpenness.Abot, 1e-8);
    EXPECT_NEAR(0, effectiveOpenness.Al, 1e-8);
    EXPECT_NEAR(0, effectiveOpenness.Ar, 1e-8);
}

TEST(TestEffectiveLayers, TestPerforatedEffectiveOpenness)
{
    SCOPED_TRACE("Begin Test: Perforated screen effective layer properties.");

    const auto width{1.3};                  // m
    const auto height{1.8};                 // m
    const auto materialThickness{0.0006};   // m
    const FenestrationCommon::Perforated::Geometry geometry{
      FenestrationCommon::Perforated::Type::Circular, 0.0148, 0.0148, 0.012, 0.012};

    const EffectiveLayers::ShadeOpenness openness{0.005, 0.004, 0.01, 0.008};

    EffectiveLayers::EffectiveLayerPerforated perforated{
      width, height, materialThickness, geometry, openness};

    EXPECT_NEAR(6e-4, perforated.effectiveThickness(), 1e-9);

    const auto effectiveOpenness{perforated.getEffectiveOpenness()};
    EXPECT_NEAR(8.257055e-2, effectiveOpenness.EffectiveFrontThermalOpennessArea, 1e-8);
    EXPECT_NEAR(0.516332, effectiveOpenness.PermeabilityFactor, 1e-6);
    EXPECT_NEAR(13.0e-3, effectiveOpenness.Atop, 1e-8);
    EXPECT_NEAR(10.4e-3, effectiveOpenness.Abot, 1e-8);
    EXPECT_NEAR(9.0e-3, effectiveOpenness.Al, 1e-8);
    EXPECT_NEAR(7.2e-3, effectiveOpenness.Ar, 1e-8);
}

TEST(TestEffectiveLayer, TestLouveredShutterEffectiveOpenness)
{
    constexpr auto systemWidth{1.0};            // m
    constexpr auto systemHeight{1.0};           // m
    FenestrationCommon::LouveredShutter::Geometry geometry{0.0889, 0.01, 87.0, 0.0762};
    const EffectiveLayers::ShadeOpenness openness{0.0, 0.0, 0.0, 0.0};

    EffectiveLayers::EffectiveLayerLouveredShutter louveredShutter{
      systemWidth, systemHeight, geometry, openness};

    EXPECT_NEAR(0.00179327, louveredShutter.effectiveThickness(), 1e-8);

    const auto effectiveOpenness{louveredShutter.getEffectiveOpenness()};
    EXPECT_NEAR(0.00464752, effectiveOpenness.EffectiveFrontThermalOpennessArea, 1e-8);
    EXPECT_NEAR(0.295495, effectiveOpenness.PermeabilityFactor, 1e-6);
}

TEST(TestEffectiveLayers, RadiusFromRise)
{
    double curvature{23.88962765};
    double slatWidth{14.8};
    auto test{ThermalPermeability::Venetian::calculateRise(curvature, slatWidth)};
    EXPECT_NEAR(1.175, test, 1e-6);
}