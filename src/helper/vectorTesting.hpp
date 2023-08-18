#pragma once

#include <gtest/gtest.h>

template<typename T>
void testVectors(const std::string & testName,
                 const std::vector<T> & correct,
                 const std::vector<T> & test,
                 const double tol = 1e-6)
{
    ASSERT_EQ(correct.size(), test.size());

    for(auto i = 0u; i < correct.size(); ++i)
    {
        EXPECT_NEAR(correct[i], test[i], tol)
          << testName << ": vector element " << i << " is not correct.";
    }
}