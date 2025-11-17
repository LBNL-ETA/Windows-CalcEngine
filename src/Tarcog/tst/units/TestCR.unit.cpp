#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

using namespace Tarcog;

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static constexpr double eps = 1e-6;

// -----------------------------------------------------------------------------
// Tests for Humidity
// -----------------------------------------------------------------------------

TEST(Humidity, CanConstructFromValidValues)
{
    EXPECT_NO_THROW(Humidity::fromFixed(30.0));
    EXPECT_NO_THROW(Humidity::fromFixed(50.0));
    EXPECT_NO_THROW(Humidity::fromFixed(70.0));

    EXPECT_NO_THROW(Humidity::H30());
    EXPECT_NO_THROW(Humidity::H50());
    EXPECT_NO_THROW(Humidity::H70());
}

TEST(Humidity, RejectsInvalidValues)
{
    EXPECT_THROW(Humidity::fromFixed(0.0), std::invalid_argument);
    EXPECT_THROW(Humidity::fromFixed(22.0), std::invalid_argument);
    EXPECT_THROW(Humidity::fromFixed(46.0), std::invalid_argument);
    EXPECT_THROW(Humidity::fromFixed(101.0), std::invalid_argument);
    EXPECT_THROW(Humidity::fromFixed(55.0), std::invalid_argument);  // good edge case
}

TEST(Humidity, AsDoubleReturnsCorrectValue)
{
    auto h = Humidity::fromFixed(30.0);
    EXPECT_NEAR(h.asDouble(), 30.0, eps);

    EXPECT_NEAR(Humidity::H50().asDouble(), 50.0, eps);
    EXPECT_NEAR(Humidity::H70().asDouble(), 70.0, eps);
}

TEST(Humidity, HumidityComparisonWorks)
{
    auto h30a = Humidity::fromFixed(30.0);
    auto h30b = Humidity::H30();
    auto h50  = Humidity::H50();

    // Operator==
    EXPECT_TRUE(h30a == h30b);
    EXPECT_FALSE(h30a == h50);

    // Operator<
    EXPECT_TRUE(h30a < h50);
    EXPECT_FALSE(h50 < h30a);
}

TEST(Humidity, CanBeUsedAsMapKey)
{
    std::map<Humidity, double> crMap;

    crMap[Humidity::H30()] = 1.23;
    crMap[Humidity::H50()] = 4.56;
    crMap[Humidity::H70()] = 7.89;

    ASSERT_EQ(crMap.size(), 3);

    EXPECT_NEAR(crMap[Humidity::H30()], 1.23, eps);
    EXPECT_NEAR(crMap[Humidity::H50()], 4.56, eps);
    EXPECT_NEAR(crMap[Humidity::H70()], 7.89, eps);
}

TEST(Humidity, DistinctKeysRemainDistinct)
{
    auto h30 = Humidity::H30();

    // Slightly perturbed value that *should* be rejected
    EXPECT_NO_THROW(Humidity::fromFixed(30.00000000000001));

    // Map contains only the canonical value
    std::map<Humidity, double> m;
    m[h30] = 1.0;

    ASSERT_EQ(m.size(), 1);
    EXPECT_NEAR(m[h30], 1.0, eps);
}
