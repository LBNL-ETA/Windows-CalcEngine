#include <gtest/gtest.h>

#include <WCETarcog.hpp>

using namespace Tarcog::ISO15099;

TEST(FrameEdgeUValue, ReturnsUCenterWhenGapIsZero)
{
    FrameClass frameClass = {1.0, 2.0, 3.0, 4.0, 5.0};
    double uCenter = 1.5;
    double gap = 0.0;

    double result = frameEdgeUValue(frameClass, uCenter, gap);

    EXPECT_DOUBLE_EQ(result, uCenter);
}

TEST(FrameEdgeUValue, CalculatesCorrectValueForNonZeroGap)
{
    FrameClass frameClass = {1.0, 2.0, 3.0, 4.0, 5.0};
    double uCenter = 1.5;
    double gap = 0.5;

    double result = frameEdgeUValue(frameClass, uCenter, gap);

    EXPECT_DOUBLE_EQ(result, 1.0 + 2.0 * 1.5 + 3.0 * 1.5 * 1.5 + 4.0 * 0.5 + 5.0 * 0.5 * 0.5);
}

TEST(FrameEdgeUValue, HandlesNegativeGap)
{
    FrameClass frameClass = {1.0, 2.0, 3.0, 4.0, 5.0};
    double uCenter = 1.5;
    double gap = -0.5;

    double result = frameEdgeUValue(frameClass, uCenter, gap);

    EXPECT_DOUBLE_EQ(result, 1.0 + 2.0 * 1.5 + 3.0 * 1.5 * 1.5 + 4.0 * -0.5 + 5.0 * -0.5 * -0.5);
}
