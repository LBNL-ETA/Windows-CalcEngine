#include <gtest/gtest.h>

#include <WCETarcog.hpp>

using namespace Tarcog::ISO15099;

TEST(DividerEdgeUValue, ZeroGap)
{
    DividerEdgePoly edgePoly{1, 2, 3, 4};

    // gap = 0, uCenter = 1.0
    double expected = 1 + 2 * 0 * 1000 + 3 * 1.0 + 4 * 1.0 * 1.0;
    EXPECT_DOUBLE_EQ(dividerEdgeUValue(edgePoly, 1.0, 0), expected);
}

TEST(DividerEdgeUValue, NegativeCoefficients)
{
    DividerEdgePoly edgePoly{0, -1, -2, -3};

    // gap = 2, uCenter = 0.5
    double expected = 0 + (-1) * 2 * 1000 + (-2) * 0.5 + (-3) * 0.5 * 0.5;
    EXPECT_NEAR(dividerEdgeUValue(edgePoly, 0.5, 2), expected, 1e-8);
}

TEST(DividerEdgeUValue, LargeGap)
{
    DividerEdgePoly edgePoly{0.5, 0.1, 0.2, 0.3};

    // gap = 10, uCenter = 2.0
    double expected = 0.5 + 0.1 * 10 * 1000 + 0.2 * 2.0 + 0.3 * 2.0 * 2.0;
    EXPECT_NEAR(dividerEdgeUValue(edgePoly, 2.0, 10), expected, 1e-8);
}

TEST(DividerEdgeUValue, AllZeros)
{
    DividerEdgePoly edgePoly{0, 0, 0, 0};

    EXPECT_DOUBLE_EQ(dividerEdgeUValue(edgePoly, 0, 0), 0);
    EXPECT_DOUBLE_EQ(dividerEdgeUValue(edgePoly, 1.5, 2.5), 0);
}
