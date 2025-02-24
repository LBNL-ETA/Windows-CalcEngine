#include <memory>
#include <gtest/gtest.h>

#include <WCECommon.hpp>

TEST(TestLouveredShutter, TestFlatSlat)
{
    SCOPED_TRACE("Begin Test: Test Louvered Shutter - Flat slat.");

    FenestrationCommon::LouveredShutter::Geometry aShutter;
    aShutter.SlatWidth = 0.1;
    aShutter.SlatSpacing = 0.1;
    aShutter.SlatAngle = 0;

    EXPECT_NEAR(0.1, FenestrationCommon::LouveredShutter::thickness(aShutter), 1e-6);
}

TEST(TestLouveredShutter, TestTiltedSlat)
{
    SCOPED_TRACE("Begin Test: Test Louvered Shutter - Tilted slat.");

    FenestrationCommon::LouveredShutter::Geometry aShutter;
    aShutter.SlatWidth = 0.1;
    aShutter.SlatSpacing = 0.1;
    aShutter.SlatAngle = 45;

    EXPECT_NEAR(0.070711, FenestrationCommon::LouveredShutter::thickness(aShutter), 1e-6);
}

TEST(TestLouveredShutter, TestTiltedSlat2)
{
    SCOPED_TRACE("Begin Test: Test Louvered Shutter - Tilted slat.");

    FenestrationCommon::LouveredShutter::Geometry aShutter;
    aShutter.SlatWidth = 0.1;
    aShutter.SlatSpacing = 0.1;
    aShutter.SlatAngle = 60;

    EXPECT_NEAR(0.05, FenestrationCommon::LouveredShutter::thickness(aShutter), 1e-6);
}