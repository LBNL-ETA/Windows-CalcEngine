#include <gtest/gtest.h>
#include <WCETarcog.hpp>

using namespace Tarcog::ISO15099;

TEST(DividerUValue, ZeroGapAndUCenter)
{
    DividerBodyPoly poly{1, 2, 3, 4, 5};
    // gap = 0, uCenter = 0
    double expected = 1 + 2 * 0 * 1000.0 + 3 * 0 * 0 * 1000000.0 + 4 * 0 + 5 * 0 * 0;
    EXPECT_DOUBLE_EQ(dividerUValue(poly, 0, 0), expected);
}

TEST(DividerUValue, TypicalCase)
{
    DividerBodyPoly poly{1, 2, 3, 4, 5};
    // gap = 0.5, uCenter = 2
    double gap1000 = 0.5 * 1000.0;
    double expected = 1 + 2 * gap1000 + 3 * gap1000 * gap1000 + 4 * 2 + 5 * 2 * 2;
    EXPECT_DOUBLE_EQ(dividerUValue(poly, 2, 0.5), expected);
}

TEST(DividerUValue, NegativeCoefficients)
{
    DividerBodyPoly poly{-1, -2, -3, -4, -5};
    // gap = 1, uCenter = 1
    double gap1000 = 1 * 1000.0;
    double expected = -1 + (-2) * gap1000 + (-3) * gap1000 * gap1000 + (-4) * 1 + (-5) * 1 * 1;
    EXPECT_DOUBLE_EQ(dividerUValue(poly, 1, 1), expected);
}

TEST(DividerUValue, AllZeros)
{
    DividerBodyPoly poly{0, 0, 0, 0, 0};
    EXPECT_DOUBLE_EQ(dividerUValue(poly, 0, 0), 0);
    EXPECT_DOUBLE_EQ(dividerUValue(poly, 1.5, 2.5), 0);
}

TEST(DividerUValue, LargeGap)
{
    DividerBodyPoly poly{0.1, 0.2, 0.3, 0.4, 0.5};
    // gap = 10, uCenter = 5
    double gap1000 = 10 * 1000.0;
    double expected = 0.1 + 0.2 * gap1000 + 0.3 * gap1000 * gap1000 + 0.4 * 5 + 0.5 * 5 * 5;
    EXPECT_DOUBLE_EQ(dividerUValue(poly, 5, 10), expected);
}