#include <memory>
#include <gtest/gtest.h>

#include "WCECommon.hpp"


using namespace FenestrationCommon;

class TestVectorOperations : public testing::Test
{};

TEST_F(TestVectorOperations, TestVectorMultiplication)
{
    SCOPED_TRACE("Begin Test: Multiplication between two vectors.");

    std::vector<double> ser1{1, 2, 3};
    std::vector<double> ser2{1, 2, 3};

    const auto result{mult<double>(ser1, ser2)};

    std::vector<double> correctResults{1, 4, 9};

    EXPECT_EQ(result.size(), correctResults.size());

    for(size_t i = 0; i < result.size(); ++i)
    {
        EXPECT_NEAR(correctResults[i], result[i], 1e-6);
    }
}

TEST_F(TestVectorOperations, TwoVectorsMultDifferentSize)
{
    SCOPED_TRACE("Begin Test: Multiplication between two vectors of different size.");

    std::vector<double> ser1{1, 2, 3};
    std::vector<double> ser2{1, 2, 3, 4};

    EXPECT_THROW(mult<double>(ser1, ser2), std::runtime_error);

    try
    {
        const auto result{mult<double>(ser1, ser2)};
    }
    catch(const std::exception & e)
    {
        EXPECT_STREQ(e.what(), "Vectors that are multiplied with each other are different size.");
    }
}