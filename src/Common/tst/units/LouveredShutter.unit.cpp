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