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