#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

#include "thermal/commonFrames.hpp"
#include "vectorTesting.hpp"

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
// Tests for TRR97 single vision
// -----------------------------------------------------------------------------

TEST(CR_TRR97, FramePositions)
{
    const auto vision = makeTRR97SingleVision();
    const auto v = CR::frameAreaContributions(vision);

    ASSERT_EQ(v.size(), 4);

    std::set<FramePosition> found;
    for(const auto & pos : v | std::views::keys)
    {
        found.insert(pos);
    }

    EXPECT_TRUE(found.contains(FramePosition::Top));
    EXPECT_TRUE(found.contains(FramePosition::Bottom));
    EXPECT_TRUE(found.contains(FramePosition::Left));
    EXPECT_TRUE(found.contains(FramePosition::Right));
}

TEST(CR_TRR97, FrameAreas)
{
    const auto vision = makeTRR97SingleVision();
    const auto v = CR::frameAreaContributions(vision);

    std::vector<double> areas;
    areas.reserve(v.size());

    for(const auto & c : v | std::views::values)
    {
        areas.push_back(c.area);
    }

    const std::vector expectedAreas = {
      0.058466,
      0.058466,
      0.058466,
      0.058466,
    };

    Helper::testVectors("CR Frame Areas", expectedAreas, areas);
}


TEST(CR_TRR97, FrameCRValues)
{
    const auto vision = makeTRR97SingleVision();
    const auto v = CR::frameAreaContributions(vision);

    std::vector<double> frameVals;
    frameVals.reserve(12);

    for(const auto & c : v | std::views::values)
    {
        for(const auto & cd : c.data)
        {
            frameVals.push_back(cd.frame);
        }
    }

    const std::vector expectedFrameVals = {
      // Top
      0.002038,
      0.025962,
      0.078704,

      // Bottom
      0.005875,
      0.039896,
      0.095246,

      // Left
      0.004263,
      0.035218,
      0.090093,

      // Right
      0.004263,
      0.035218,
      0.090093,
    };

    Helper::testVectors("CR Frame Values", expectedFrameVals, frameVals);
}


TEST(CR_TRR97, EdgeCRValues)
{
    const auto vision = makeTRR97SingleVision();
    const auto v = CR::frameAreaContributions(vision);

    std::vector<double> edgeVals;
    edgeVals.reserve(12);

    for(const auto & [pos, c] : v)
    {
        for(const auto & cd : c.data)
        {
            edgeVals.push_back(cd.edge);
        }
    }

    const std::vector expectedEdgeVals = {
      // Top
      0.008546,
      0.078640,
      0.198630,

      // Bottom
      0.043411,
      0.216304,
      0.334905,

      // Left
      0.022622,
      0.146351,
      0.275538,

      // Right
      0.022622,
      0.146351,
      0.275538,
    };

    Helper::testVectors("CR Edge Values", expectedEdgeVals, edgeVals);
}


TEST(CR_TRR97, AveragesFrame)
{
    const auto vision = makeTRR97SingleVision();
    const auto v = CR::frameAreaContributions(vision);
    const auto averages = CR::computeAverages(v);

    ASSERT_EQ(averages.size(), 4);

    std::vector<double> avgFrames;
    avgFrames.reserve(4);

    for(const auto & [pos, c] : averages)
    {
        ASSERT_TRUE(c.average.has_value());
        avgFrames.push_back(c.average->frame);
    }

    const std::vector expectedAvgFrames = {
        0.035568,   // Top
        0.047006,   // Bottom
        0.043192,   // Left
        0.043192    // Right
    };

    Helper::testVectors("CR Avg Frame", expectedAvgFrames, avgFrames);
}


TEST(CR_TRR97, AveragesEdge)
{
    const auto vision = makeTRR97SingleVision();
    const auto v = CR::frameAreaContributions(vision);
    const auto averages = CR::computeAverages(v);

    ASSERT_EQ(averages.size(), 4);

    std::vector<double> avgEdges;
    avgEdges.reserve(4);

    for(const auto & [pos, c] : averages)
    {
        avgEdges.push_back(c.average->edge);
    }

    const std::vector expectedAvgEdges = {
        0.095272,
        0.198207,
        0.148171,
        0.148171
    };

    Helper::testVectors("CR Avg Edge", expectedAvgEdges, avgEdges);
}



TEST(CR_TRR97, CRf)
{
    const auto vision{makeTRR97SingleVision()};

    const auto [values, average] = CR::crf(vision.frames());

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H30()), 83.981921, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 67.580541, eps);
    EXPECT_NEAR(values.at(Humidity::H70()), 55.430536, eps);

    EXPECT_NEAR(average, 65.173843, eps);
}

TEST(CR_TRR97, CRe)
{
    const auto vision{makeTRR97SingleVision()};

    auto [values, average] = CR::cre(vision.frames());

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H30()), 71.035203, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 47.234265, eps);
    EXPECT_NEAR(values.at(Humidity::H70()), 35.275084, eps);

    EXPECT_NEAR(average, 47.169288, eps);
}

TEST(CR_TRR97, CRg)
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

TEST(CR_TRR97, CR)
{
    const auto vision{makeTRR97SingleVision()};

    constexpr auto outsideTemperature{255.15};

    const auto [values, average] =
      CR::cr(vision, CR::defaultDewPointSettings(), outsideTemperature);

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H30()), 82.671892, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 67.580541, eps);
    EXPECT_NEAR(values.at(Humidity::H70()), 46.366992, eps);

    EXPECT_NEAR(average, 60.074171, eps);
}

TEST(CR_TRR97, CRb)
{
    const auto vision{makeTRR97SingleVision()};

    constexpr auto outsideTemperature{255.15};

    const auto [values, average] =
      CR::crb(vision, CR::defaultDewPointSettings(), outsideTemperature);

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H30()), 71.035203, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 47.234265, eps);
    EXPECT_NEAR(values.at(Humidity::H70()), 35.275084, eps);

    EXPECT_NEAR(average, 47.169288, eps);
}