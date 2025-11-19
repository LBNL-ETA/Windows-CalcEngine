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

TEST(CR_TRR97, FrameContributions)
{
    const auto vision{makeTRR97SingleVision()};
    const auto v{CR::frameAreaContributions(vision)};

    ASSERT_EQ(v.size(), 4);


    std::set<FramePosition> found;
    std::ranges::transform(
      v, std::inserter(found, found.end()), [](const auto & c) { return c.pos; });

    EXPECT_EQ(found.size(), 4);
    EXPECT_TRUE(found.contains(FramePosition::Top));
    EXPECT_TRUE(found.contains(FramePosition::Bottom));
    EXPECT_TRUE(found.contains(FramePosition::Left));
    EXPECT_TRUE(found.contains(FramePosition::Right));

    // ---- AREA ----
    std::vector<double> areas;
    areas.reserve(v.size());
    std::ranges::transform(v, std::back_inserter(areas), [](const auto & c) { return c.area; });

    const std::vector expectedAreas = {
      0.0584656658,
      0.0584656658,
      0.0584656658,
      0.0584656658,
    };

    Helper::testVectors("CR Frame Areas", expectedAreas, areas);

    // ---- FRAME CR VALUES ----
    std::vector<double> frameVals;
    frameVals.reserve(v.size() * 3);

    for(const auto & c : v)
    {
        for(const auto & cd : c.data)
        {
            frameVals.push_back(cd.frame);
        }
    }

    // Hard-coded expected values (in the order TRR97 returns them)
    const std::vector expectedFrameVals = {
      // Top
      0.0020379023626446698,
      0.0259622453595846402,
      0.0787044763565056306,

      // Bottom
      0.0058753238990902901,
      0.039895989000797299,
      0.095246292650699602,

      // Left
      0.0042631877586245502,
      0.035217877477407497,
      0.090093493461608901,

      // Right
      0.0042631877586245502,
      0.035217877477407497,
      0.090093493461608901,
    };

    Helper::testVectors("CR Frame Values", expectedFrameVals, frameVals);

    // ---- EDGE CONDENSATION VALUES ----
    std::vector<double> edgeVals;
    edgeVals.reserve(12);
    for(const auto & c : v)
    {
        for(const auto & cd : c.data)
        {
            edgeVals.push_back(cd.edge);
        }
    }

    const std::vector<double> expectedEdgeVals = {
      // Top (unchanged)
      0.0085457945242524094,
      0.078639589250087696,
      0.19863036274910001,

      // Bottom
      0.043410677462816197,
      0.216304361820221,
      0.33490517735481301,

      // Left
      0.022622345015406602,
      0.146351143717766,
      0.27553832530975297,

      // Right
      0.022622345015406602,
      0.146351143717766,
      0.27553832530975297,
    };

    Helper::testVectors("CR Edge Values", expectedEdgeVals, edgeVals);

    auto averages{CR::computeAverages(v)};

    // ---- AVERAGES ----
    ASSERT_EQ(averages.size(), 4);

    // Extract avg.frame
    std::vector<double> avgFrames;
    avgFrames.reserve(4);

    for (const auto& c : averages)
    {
        ASSERT_TRUE(c.average.has_value());
        avgFrames.push_back(c.average->frame);
    }

    const std::vector<double> expectedAvgFrames = {
        0.03556820736,   // Top
        0.04700586855,   // Bottom
        0.04319151956,   // Left
        0.04319151956    // Right
    };

    Helper::testVectors("CR Avg Frame Values", expectedAvgFrames, avgFrames);


    // Extract avg.area
    std::vector<double> avgEdges;
    avgEdges.reserve(4);

    for (const auto& c : averages)
    {
        avgEdges.push_back(c.average->edge);
    }

    const std::vector expectedAvgEdges = {
        0.095271915507813373,
        0.19820673887928342,
        0.1481706046809752,
        0.1481706046809752
    };

    Helper::testVectors("CR Avg Frame Areas", expectedAvgEdges, avgEdges);
}

TEST(CR_TRR97, CRf)
{
    const auto vision{makeTRR97SingleVision()};

    const auto [values, average] = CR::crf(vision);

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H30()), 83.981921, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 67.580541, eps);
    EXPECT_NEAR(values.at(Humidity::H70()), 55.430536, eps);

    EXPECT_NEAR(average, 65.173843, eps);
}

TEST(CR_TRR97, CRe)
{
    const auto vision{makeTRR97SingleVision()};

    auto [values, average] = CR::cre(vision);

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