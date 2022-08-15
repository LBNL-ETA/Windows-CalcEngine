#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "WCESingleLayerOptics.hpp"

class TestBSDFPatches : public testing::Test
{
protected:
    virtual void SetUp()
    {}
};

TEST_F(TestBSDFPatches, TestAngleLimits1)
{
    SingleLayerOptics::AngleLimits aLimits(-15, 15);

    const double angle = 350;

    bool isInLimits = aLimits.isInLimits(angle);
    EXPECT_EQ(isInLimits, true);

    EXPECT_NEAR(aLimits.average(), 0, 1e-6);
    EXPECT_NEAR(aLimits.low(), -15, 1e-6);
    EXPECT_NEAR(aLimits.high(), 15, 1e-6);
    EXPECT_NEAR(aLimits.delta(), 30, 1e-6);
}

TEST_F(TestBSDFPatches, TestAngleLimitsCenterPatch1)
{
    SingleLayerOptics::AngleLimits aLimits(15);

    const double angle = 0;

    bool isInLimits = aLimits.isInLimits(angle);
    EXPECT_EQ(isInLimits, true);

    EXPECT_NEAR(aLimits.average(), 0, 1e-6);
    EXPECT_NEAR(aLimits.low(), 0, 1e-6);
    EXPECT_NEAR(aLimits.high(), 15, 1e-6);
    EXPECT_NEAR(aLimits.delta(), 15, 1e-6);
}
