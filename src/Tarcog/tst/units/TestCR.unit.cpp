#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

#include "thermal/commonFrames.hpp"

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
    EXPECT_THROW(Humidity::fromFixed(55.0), std::invalid_argument);   // good edge case
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
    auto h50 = Humidity::H50();

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

TEST(CR, ComputesCRfForTRR97)
{
    using namespace Tarcog;

    constexpr auto width{1.219};
    constexpr auto height{1.219};
    constexpr auto tVis{0.707};
    constexpr auto tSol{0.3614};

    constexpr auto iguUValue{5.575};
    constexpr auto shgc{0.86};
    constexpr auto hout{20.42635};

    ISO15099::WindowVision vision = ISO15099::WindowVision(
      width, height, tVis, tSol, std::make_shared<ISO15099::SimpleIGU>(iguUValue, shgc, hout));

    vision.setFrameData(FramePosition::Bottom, Frame::sillTRR97());
    vision.setFrameData(FramePosition::Top, Frame::headTRR97());
    vision.setFrameData(FramePosition::Left, Frame::jambTRR97());
    vision.setFrameData(FramePosition::Right, Frame::jambTRR97());

    CR::DewPointTable dewPoints = {
      {Humidity::H30(), 2.9}, {Humidity::H50(), 10.3}, {Humidity::H70(), 15.4}};

    CRResult crf_result = CR::crf(vision);

    auto & result = crf_result.values;

    ASSERT_EQ(result.size(), 3);

    EXPECT_NEAR(result.at(Humidity::H30()), 83.981921, eps);
    EXPECT_NEAR(result.at(Humidity::H50()), 67.580541, eps);
    EXPECT_NEAR(result.at(Humidity::H70()), 55.430536, eps);

    // Check the area-weighted total average CR
    EXPECT_NEAR(crf_result.average, 65.173843, eps);
}

TEST(CR, ComputesCReForTRR97)
{
    using namespace Tarcog;

    constexpr auto width{1.219};
    constexpr auto height{1.219};
    constexpr auto tVis{0.707};
    constexpr auto tSol{0.3614};

    constexpr auto iguUValue{5.575};
    constexpr auto shgc{0.86};
    constexpr auto hout{20.42635};

    ISO15099::WindowVision vision = ISO15099::WindowVision(
      width, height, tVis, tSol, std::make_shared<ISO15099::SimpleIGU>(iguUValue, shgc, hout));

    vision.setFrameData(FramePosition::Bottom, Frame::sillTRR97());
    vision.setFrameData(FramePosition::Top, Frame::headTRR97());
    vision.setFrameData(FramePosition::Left, Frame::jambTRR97());
    vision.setFrameData(FramePosition::Right, Frame::jambTRR97());

    CR::DewPointTable dewPoints = {
        {Humidity::H30(), 2.9}, {Humidity::H50(), 10.3}, {Humidity::H70(), 15.4}};

    CRResult cre_result = CR::cre(vision);

    auto & result = cre_result.values;

    ASSERT_EQ(result.size(), 3);

    EXPECT_NEAR(result.at(Humidity::H30()), 71.035203038841516, eps);
    EXPECT_NEAR(result.at(Humidity::H50()), 47.234265274930252, eps);
    EXPECT_NEAR(result.at(Humidity::H70()), 35.275083816086294, eps);

    // Check the area-weighted total average CR
    EXPECT_NEAR(cre_result.average, 47.169287513342574, eps);
}