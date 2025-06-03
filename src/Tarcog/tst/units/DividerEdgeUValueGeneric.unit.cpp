#include <gtest/gtest.h>

#include <WCETarcog.hpp>

using namespace Tarcog::ISO15099;

TEST(DividerEdgeUValue, BasicCalculation)
{
    // Test coefficients: c[0]=1, c[1]=2, c[2]=3, c[3]=4
    GenericDivider divider{
        .GapMin = 0,
        .GapMax = 10,
        .Divider = {1, 2, 3, 4, 0, 0, 0, 0} // Only first 4 are used in edge calculation
    };

    // gap = 0.5, uCenter = 1.2
    // Expected: 1 + 2*0.5*1000 + 3*1.2 + 4*1.2*1.2
    double expected = 1 + 2*0.5*1000 + 3*1.2 + 4*1.2*1.2;
    double result = dividerEdgeUValue(divider, 1.2, 0.5);
    EXPECT_NEAR(result, expected, 1e-8);
}

TEST(DividerEdgeUValue, ZeroGap)
{
    GenericDivider divider{
        .GapMin = 0,
        .GapMax = 10,
        .Divider = {1, 2, 3, 4, 0, 0, 0, 0}
    };

    // gap = 0, uCenter = 1.0
    double expected = 1 + 2*0*1000 + 3*1.0 + 4*1.0*1.0;
    EXPECT_DOUBLE_EQ(dividerEdgeUValue(divider, 1.0, 0), expected);
}

TEST(DividerEdgeUValue, NegativeCoefficients)
{
    GenericDivider divider{
        .GapMin = 0,
        .GapMax = 10,
        .Divider = {0, -1, -2, -3, 0, 0, 0, 0}
    };

    // gap = 2, uCenter = 0.5
    double expected = 0 + (-1)*2*1000 + (-2)*0.5 + (-3)*0.5*0.5;
    EXPECT_NEAR(dividerEdgeUValue(divider, 0.5, 2), expected, 1e-8);
}

TEST(DividerEdgeUValue, LargeGap)
{
    GenericDivider divider{
        .GapMin = 0,
        .GapMax = 100,
        .Divider = {0.5, 0.1, 0.2, 0.3, 0, 0, 0, 0}
    };

    // gap = 10, uCenter = 2.0
    double expected = 0.5 + 0.1*10*1000 + 0.2*2.0 + 0.3*2.0*2.0;
    EXPECT_NEAR(dividerEdgeUValue(divider, 2.0, 10), expected, 1e-8);
}

TEST(DividerEdgeUValue, AllZeros)
{
    GenericDivider divider{
        .GapMin = 0,
        .GapMax = 10,
        .Divider = {0, 0, 0, 0, 0, 0, 0, 0}
    };

    EXPECT_DOUBLE_EQ(dividerEdgeUValue(divider, 0, 0), 0);
    EXPECT_DOUBLE_EQ(dividerEdgeUValue(divider, 1.5, 2.5), 0);
}

