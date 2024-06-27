#include <memory>
#include <gtest/gtest.h>

#include "WCECommon.hpp"


class MathFunctionsTest : public testing::Test
{
protected:
    void SetUp() override
    {}
};

TEST_F(MathFunctionsTest, TestFloatEquality)
{
    constexpr auto x1{1.0};
    constexpr auto x2{1.0};

    EXPECT_EQ(FenestrationCommon::isEqual(x1, x2), true);
}


TEST_F(MathFunctionsTest, TestFloatInequality)
{
    constexpr auto x1{1.00000000001};
    constexpr auto x2{1.0};

    EXPECT_EQ(FenestrationCommon::isEqual(x1, x2), false);
}

TEST_F(MathFunctionsTest, DoubleToStringTwoDeimals)
{
    constexpr double value{13.234};
    constexpr auto numberOfDecimals{2u};
    const std::string correct{"13.23"};
    EXPECT_EQ(correct, FenestrationCommon::to_string_with_precision(value, numberOfDecimals));
}

TEST_F(MathFunctionsTest, DoubleToStringThreeDeimals)
{
    constexpr double value{13.234564};
    constexpr auto numberOfDecimals{3u};
    const std::string correct{"13.235"};
    EXPECT_EQ(correct, FenestrationCommon::to_string_with_precision(value, numberOfDecimals));
}

TEST_F(MathFunctionsTest, TriangleArea)
{
    EXPECT_NEAR(3 * std::sqrt(3) / 4, FenestrationCommon::nTagonArea(3, 1), 1e-6);
}

TEST_F(MathFunctionsTest, SquareArea)
{
    EXPECT_NEAR(2, FenestrationCommon::nTagonArea(4, 1), 1e-6);
}

TEST_F(MathFunctionsTest, PentagonArea)
{
    EXPECT_NEAR(2.377641, FenestrationCommon::nTagonArea(5, 1), 1e-6);
}

TEST_F(MathFunctionsTest, HexagonArea)
{
    EXPECT_NEAR(3 * std::sqrt(3) / 2, FenestrationCommon::nTagonArea(6, 1), 1e-6);
}