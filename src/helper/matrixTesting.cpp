#include <gtest/gtest.h>

#include "matrixTesting.hpp"

namespace Helper
{
    void compareMatrices(const std::vector<std::vector<double>> & expected,
                         const std::vector<std::vector<double>> & actual,
                         double tolerance)
    {
        ASSERT_EQ(expected.size(), actual.size());
        for(size_t i = 0; i < expected.size(); ++i)
        {
            ASSERT_EQ(expected[i].size(), actual[i].size());
            for(size_t j = 0; j < expected[i].size(); ++j)
            {
                EXPECT_NEAR(expected[i][j], actual[i][j], tolerance);
            }
        }
    }
}   // namespace Helper