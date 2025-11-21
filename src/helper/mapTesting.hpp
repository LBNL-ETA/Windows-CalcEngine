#pragma once
#include <gtest/gtest.h>
#include <map>
#include <type_traits>
#include <string>

namespace Helper
{
    template<typename K, typename V>
    void testMaps(const std::string & testName,
                  const std::map<K, V> & correct,
                  const std::map<K, V> & test,
                  const double tol = 1e-6)
    {
        ASSERT_EQ(correct.size(), test.size()) << testName << ": map sizes differ.";

        size_t idx = 0;
        for(const auto & entry : correct)
        {
            const K & key = entry.first;
            const V & value = entry.second;

            auto it = test.find(key);
            ASSERT_NE(it, test.end()) << testName << ": missing key at index " << idx << ".";

            if constexpr(std::is_floating_point_v<V>)
            {
                EXPECT_NEAR(value, it->second, tol)
                  << testName << ": value for key at index " << idx << " differs.";
            }
            else
            {
                EXPECT_EQ(value, it->second)
                  << testName << ": value for key at index " << idx << " differs.";
            }
            ++idx;
        }
    }
}   // namespace Helper
