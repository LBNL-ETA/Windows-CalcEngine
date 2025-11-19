#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

#include "thermal/commonFrames.hpp"

using namespace Tarcog;

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

namespace
{
    constexpr double eps = 1e-6;

    ISO15099::WindowVision makeTRR97SingleVision()
    {
        constexpr auto width{1.219};
        constexpr auto height{1.219};
        constexpr auto tVis{0.707};
        constexpr auto tSol{0.3614};
        constexpr auto iguUValue{5.575};
        constexpr auto shgc{0.86};
        constexpr auto hout{20.42635};
        constexpr auto interiorGlassTemperature{284.79001};

        auto igu = std::make_shared<ISO15099::SimpleIGU>(iguUValue, shgc, hout);
        igu->setTemperatures({interiorGlassTemperature});

        ISO15099::WindowVision vision(width, height, tVis, tSol, igu);
        vision.setFrameData(FramePosition::Bottom, Frame::sillTRR97());
        vision.setFrameData(FramePosition::Top, Frame::headTRR97());
        vision.setFrameData(FramePosition::Left, Frame::jambTRR97());
        vision.setFrameData(FramePosition::Right, Frame::jambTRR97());
        return vision;
    }
}   // namespace

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
    EXPECT_THROW(Humidity::fromFixed(55.0), std::invalid_argument);
}

TEST(Humidity, AsDoubleReturnsCorrectValue)
{
    const auto h = Humidity::fromFixed(30.0);
    EXPECT_NEAR(h.asDouble(), 30.0, eps);

    EXPECT_NEAR(Humidity::H50().asDouble(), 50.0, eps);
    EXPECT_NEAR(Humidity::H70().asDouble(), 70.0, eps);
}

TEST(Humidity, HumidityComparisonWorks)
{
    const auto h30a = Humidity::fromFixed(30.0);
    const auto h30b = Humidity::H30();
    const auto h50 = Humidity::H50();

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
    const auto h30 = Humidity::H30();

    // Slightly perturbed value that *should* not be rejected
    EXPECT_NO_THROW(Humidity::fromFixed(30.00000000000001));

    std::map<Humidity, double> m{{h30, 1.0}};

    ASSERT_EQ(m.size(), 1);
    EXPECT_NEAR(m[h30], 1.0, eps);
}

TEST(CR, ComputesCRfForTRR97)
{
    const auto vision{makeTRR97SingleVision()};

    const auto [values, average] = CR::crf(vision);

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H30()), 83.981921, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 67.580541, eps);
    EXPECT_NEAR(values.at(Humidity::H70()), 55.430536, eps);

    EXPECT_NEAR(average, 65.173843, eps);
}

TEST(CR, ComputesCReForTRR97)
{
    const auto vision{makeTRR97SingleVision()};

    auto [values, average] = CR::cre(vision);

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H30()), 71.035203, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 47.234265, eps);
    EXPECT_NEAR(values.at(Humidity::H70()), 35.275084, eps);

    EXPECT_NEAR(average, 47.169288, eps);
}

TEST(CR, ComputesCRgForTRR97)
{
    const auto vision{makeTRR97SingleVision()};

    constexpr auto outsideTemperature{255.15};

    auto [values, average] = CR::crg(vision, CR::defaultDewPointSettings(), outsideTemperature);

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H70()), 50.344723, eps);
    EXPECT_NEAR(values.at(Humidity::H30()), 100.0, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 100.0, eps);

    EXPECT_NEAR(average, 65.570954, eps);
}

TEST(CR, ComputesCRForTRR97)
{
    const auto vision{makeTRR97SingleVision()};

    constexpr auto outsideTemperature{255.15};

    const auto [values, average] = CR::cr(vision, CR::defaultDewPointSettings(), outsideTemperature);

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H30()), 82.671892, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 67.580541, eps);
    EXPECT_NEAR(values.at(Humidity::H70()), 46.366992, eps);

    EXPECT_NEAR(average, 60.074171, eps);
}

TEST(CR, ComputesCRbForTRR97)
{
    const auto vision{makeTRR97SingleVision()};

    constexpr auto outsideTemperature{255.15};

    const auto [values, average] = CR::crb(vision, CR::defaultDewPointSettings(), outsideTemperature);

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H30()), 71.035203, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 47.234265, eps);
    EXPECT_NEAR(values.at(Humidity::H70()), 35.275084, eps);

    EXPECT_NEAR(average, 47.169288, eps);
}