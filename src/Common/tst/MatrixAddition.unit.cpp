#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCECommon.hpp"

using namespace FenestrationCommon;

class TestMatrixAddition : public testing::Test
{
protected:
    void SetUp() override
    {}
};

TEST_F(TestMatrixAddition, Test1)
{
    SCOPED_TRACE("Begin Test: Test matrix addition operation.");

    const SquareMatrix a{{1, 2}, {3, 4}};
    const SquareMatrix b{{2, 3}, {4, 5}};

    auto c = a + b;

    EXPECT_NEAR(3, c(0, 0), 1e-6);
    EXPECT_NEAR(5, c(0, 1), 1e-6);
    EXPECT_NEAR(7, c(1, 0), 1e-6);
    EXPECT_NEAR(9, c(1, 1), 1e-6);
}

TEST_F(TestMatrixAddition, Test2)
{
    SCOPED_TRACE("Begin Test: Test matrix subtraction operation.");

    const SquareMatrix a{ { 1, 2 }, { 3, 4 } };
    const SquareMatrix b{ { 2, 3 }, { 4, 5 } };

    auto c = a - b;

    EXPECT_NEAR(-1, c(0, 0), 1e-6);
    EXPECT_NEAR(-1, c(0, 1), 1e-6);
    EXPECT_NEAR(-1, c(1, 0), 1e-6);
    EXPECT_NEAR(-1, c(1, 1), 1e-6);
}
