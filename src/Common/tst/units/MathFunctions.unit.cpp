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