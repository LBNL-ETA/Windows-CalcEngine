#include <gtest/gtest.h>

#include <vector>
#include <array>
#include <tuple>
#include <type_traits>
#include <ranges>

#include "WCECommon.hpp"

using common::views::zip;
using common::views::zip_strict;
using common::views::enumerate;

TEST(ZipView, MutatesSecondRangeByReference)
{
    std::vector<int> xs{1, 2, 3, 4};
    std::vector<double> ys{0.5, 0.5, 0.5, 0.5};

    for(auto && [x, y] : zip(xs, ys))
    {
        y *= x;   // mutate through reference
    }

    const std::vector<double> expected{0.5, 1.0, 1.5, 2.0};
    EXPECT_EQ(ys, expected);
}

TEST(ZipView, TupleElementTypesAreReferences_NonConst)
{
    std::vector<int> xs{1};
    std::vector<double> ys{0.1};

    auto v = zip(xs, ys);
    using Tup = decltype(*v.begin());
    static_assert(std::is_same_v<Tup, std::tuple<int &, double &>>, "zip must yield refs");
    (void)v;   // silence unused in some compilers
}

TEST(ZipView, TupleElementTypesAreReferences_Const)
{
    const std::vector<int> xs{1};
    const std::vector<double> ys{0.1};

    auto v = zip(xs, ys);
    using Tup = decltype(*v.begin());
    static_assert(std::is_same_v<Tup, std::tuple<const int &, const double &>>,
                  "zip(const) must yield const refs");
    (void)v;
}

TEST(ZipView, StopsAtShortestRange)
{
    std::vector<int> xs{10, 20, 30, 40, 50};
    std::vector<double> ys{1.0, 2.0, 3.0};   // shorter

    int count = 0;
    for(auto && [x, y] : zip(xs, ys))
    {
        y += x;   // mutate first 3 only
        ++count;
    }

    EXPECT_EQ(count, 3);   // min size
    EXPECT_DOUBLE_EQ(ys[0], 11.0);
    EXPECT_DOUBLE_EQ(ys[1], 22.0);
    EXPECT_DOUBLE_EQ(ys[2], 33.0);

    // xs[3], xs[4] untouched by the loop
    EXPECT_EQ(xs[3], 40);
    EXPECT_EQ(xs[4], 50);
}

TEST(ZipView, ThreeRangesZipAndAccumulate)
{
    std::vector<int> a{1, 2, 3};
    std::vector<int> b{4, 5, 6};
    std::array<int, 4> c{7, 8, 9, 10};   // longer; shortest is size 3

    std::vector<int> summed;
    for(auto && [x, y, z] : zip(a, b, c))
    {
        summed.push_back(x + y + z);
    }

    std::vector<int> expected{12, 15, 18};
    EXPECT_EQ(summed, expected);
}

TEST(EnumerateView, YieldsIndexAndReferenceAndMutates)
{
    std::vector<int> xs{5, 5, 5, 5};

    for(auto && [i, v] : enumerate(xs))
    {
        v = static_cast<int>(i);   // mutate through ref
    }

    std::vector<int> expected{0, 1, 2, 3};
    EXPECT_EQ(xs, expected);
}

TEST(EnumerateView, EnumerateTypes)
{
    std::vector<int> xs{42};
    auto v = enumerate(xs);
    using Tup = decltype(*v.begin());
    static_assert(std::is_same_v<Tup, std::tuple<std::size_t, int &>>,
                  "enumerate yields (index, ref)");
    (void)v;

    const std::vector<int> cxs{42};
    auto cv = enumerate(cxs);
    using CTup = decltype(*cv.begin());
    static_assert(std::is_same_v<CTup, std::tuple<std::size_t, const int &>>,
                  "enumerate(const) yields (index, const ref)");
    (void)cv;
}

// Optional: if you keep zip_strict and want a sanity check, test only equal sizes.
// (In Release builds asserts are disabled; testing the mismatch path would be flaky.)
TEST(ZipView, ZipStrictEqualSizes)
{
    std::vector<int> a{1, 2, 3};
    std::vector<int> b{4, 5, 6};
    int sum = 0;
    for(auto && [x, y] : zip_strict(a, b))
    {
        sum += x * y;
    }
    EXPECT_EQ(sum, 1 * 4 + 2 * 5 + 3 * 6);
}
