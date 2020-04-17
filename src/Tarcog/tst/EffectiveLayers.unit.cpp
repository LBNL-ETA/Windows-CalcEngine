#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

class TestEffectiveLayers : public testing::Test
{
protected:
    void SetUp() override
    {}
};

TEST_F(TestEffectiveLayers, TestVenetianHorizontalEffectiveLayer)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    const auto width{1.0};                 // m
    const auto height{1.0};                // m
    const auto materialThickness{0.0001};   // m
    const auto slatTiltAngle{0.0};
    const auto slatWidth{0.0148};   // m

    EffectiveLayers::ShadeOpenness openness{0.991657018661499, 0, 0, 0, 0};

    EffectiveLayers::EffectiveHorizontalVenetian venetian{
      width, height, materialThickness, openness, slatTiltAngle, slatWidth};

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(6.364e-4, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(1.592911e-2, effectiveOpenness.Ah, 1e-8);
}

TEST_F(TestEffectiveLayers, TestVenetianHorizontalEffectiveLayerWithTopAndBotOpenness)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    const auto width{1.3};                 // m
    const auto height{1.8};                // m
    const auto materialThickness{0.0001};   // m
    const auto slatTiltAngle{0.0};
    const auto slatWidth{0.0148};   // m

    EffectiveLayers::ShadeOpenness openness{2.32047748565674, 0, 0, 0.01, 0.008};

    EffectiveLayers::EffectiveHorizontalVenetian venetian{
      width, height, materialThickness, openness, slatTiltAngle, slatWidth};

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(6.364e-4, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(3.727412206e-2, effectiveOpenness.Ah, 1e-8);
    EXPECT_NEAR(1.3e-2, effectiveOpenness.Atop, 1e-8);
    EXPECT_NEAR(1.04e-2, effectiveOpenness.Abot, 1e-8);
    EXPECT_NEAR(0, effectiveOpenness.Al, 1e-8);
    EXPECT_NEAR(0, effectiveOpenness.Ar, 1e-8);
}

TEST_F(TestEffectiveLayers, TestVenetianVerticalEffectiveLayerWithTopAndBotOpenness)
{
    SCOPED_TRACE("Begin Test: Venetian horizontal effective layer properties.");

    const auto width{1.3};                 // m
    const auto height{1.8};                // m
    const auto materialThickness{0.0001};   // m
    const auto slatTiltAngle{0.0};
    const auto slatWidth{0.0762};   // m

    EffectiveLayers::ShadeOpenness openness{2.33584642410278, 0, 0, 0.01, 0.008};

    EffectiveLayers::EffectiveVerticalVenentian venetian{
      width, height, materialThickness, openness, slatTiltAngle, slatWidth};

    const auto effectiveThickness{venetian.effectiveThickness()};

    EXPECT_NEAR(9.144e-4, effectiveThickness, 1e-9);

    const auto effectiveOpenness{venetian.getEffectiveOpenness()};
    EXPECT_NEAR(9.589398567e-2, effectiveOpenness.Ah, 1e-8);
    EXPECT_NEAR(1.3e-2, effectiveOpenness.Atop, 1e-8);
    EXPECT_NEAR(1.04e-2, effectiveOpenness.Abot, 1e-8);
    EXPECT_NEAR(0, effectiveOpenness.Al, 1e-8);
    EXPECT_NEAR(0, effectiveOpenness.Ar, 1e-8);
}