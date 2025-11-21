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

    ISO15099::DualVisionHorizontal makeTRR97HorizontalSlider()
    {
        constexpr auto width{2.4};
        constexpr auto height{1.5};
        constexpr auto tVis{0.707};
        constexpr auto tSol{0.3614};
        constexpr auto iguUValue{5.575};
        constexpr auto shgc{0.86};

        constexpr auto hout{20.42635};
        constexpr auto interiorGlassTemperature{284.780396};

        auto igu = std::make_shared<ISO15099::SimpleIGU>(iguUValue, shgc, hout);
        igu->setTemperatures({interiorGlassTemperature});

        auto window =
          ISO15099::DualVisionHorizontal(width, height, tVis, tSol, igu, tVis, tSol, igu);
        window.setFrameData(
          {{ISO15099::DualHorizontalFramePosition::TopLeft, Frame::headTRR97()},
           {ISO15099::DualHorizontalFramePosition::TopRight, Frame::headTRR97()},
           {ISO15099::DualHorizontalFramePosition::BottomLeft, Frame::sillTRR97()},
           {ISO15099::DualHorizontalFramePosition::BottomRight, Frame::sillTRR97()},
           {ISO15099::DualHorizontalFramePosition::MeetingRail, Frame::jambTRR97()},
           {ISO15099::DualHorizontalFramePosition::Left, Frame::jambTRR97()},
           {ISO15099::DualHorizontalFramePosition::Right, Frame::jambTRR97()}});

        return window;
    }

    ISO15099::DualVisionHorizontal makeTRR97HorizontalSliderWithDividers()
    {
        auto window{makeTRR97HorizontalSlider()};

        window.setDividersAuto(Frame::sampleDivider());

        return window;
    }
}   // namespace

// -----------------------------------------------------------------------------
// Tests for TRR97 horizontal slider
// -----------------------------------------------------------------------------

TEST(CR_TRR97_Horizontal_Slider, FrameCRValues)
{
    const auto vision = makeTRR97HorizontalSlider();
    const auto values = CR::frameAreaContributions(vision.frames());

    auto framesView =
      values | std::views::values
      | std::views::transform([](auto const & val) -> auto const & { return val.data; })
      | std::views::join | std::views::transform([](auto const & cond) { return cond.frame; });

    const std::vector frameVals(std::ranges::begin(framesView), std::ranges::end(framesView));

    // clang-format off
    const std::vector expectedFrameVals = {
        // Top-Left
        0.002038, 0.025962, 0.078704,

        // Top-Right
        0.002038, 0.025962, 0.078704,

        // Bottom-Left
        0.005875, 0.039896, 0.095246,

        // Bottom-Right
        0.005875, 0.039896, 0.095246,

        // Left
        0.004263, 0.035218, 0.090093,

        // Right
        0.004263, 0.035218, 0.090093,

        // Meeting Rail
        0.004263, 0.035218, 0.090093
    };
    // clang-format on


    Helper::testVectors("CR Frame Values", expectedFrameVals, frameVals);
}

TEST(CR_TRR97_Horizontal_Slider, EdgeCRValues)
{
    const auto vision = makeTRR97HorizontalSlider();
    const auto values = CR::frameAreaContributions(vision.frames());

    auto edgesView =
      values | std::views::values
      | std::views::transform([](auto const & val) -> auto const & { return val.data; })
      | std::views::join | std::views::transform([](auto const & cond) { return cond.edge; });

    const std::vector edgeVals(std::ranges::begin(edgesView), std::ranges::end(edgesView));

    // clang-format off
    const std::vector expectedEdgeVals = {
      // Top-Left
      0.008546, 0.078640, 0.198630,

      // Top-Right
      0.008546, 0.078640, 0.198630,

      // Bottom-Left
      0.043411, 0.216304, 0.334905,

      // Bottom-Right
      0.043411, 0.216304, 0.334905,

      // Left
      0.022622, 0.146351, 0.275538,

      // Right
      0.022622, 0.146351, 0.275538,

      // Meeting Rail
      0.022622, 0.146351, 0.275538
    };
    // clang-format on


    Helper::testVectors("CR Edge Values", expectedEdgeVals, edgeVals);
}

TEST(CR_TRR97_Horizontal_Slider, AveragesFrame)
{
    const auto vision = makeTRR97HorizontalSlider();
    const auto val = CR::frameAreaContributions(vision.frames());
    const auto averages = CR::computeAverages(val);

    ASSERT_EQ(averages.size(), 7);

    auto avgFramesView =
      averages | std::views::values
      | std::views::transform([](auto const & cond) { return cond.average->frame; });

    const std::vector avgFrames(std::ranges::begin(avgFramesView), std::ranges::end(avgFramesView));

    const std::vector expectedAvgFrames = {
      0.035568, 0.035568, 0.047006, 0.047006, 0.043192, 0.043192, 0.043192};

    Helper::testVectors("CR Avg Frame", expectedAvgFrames, avgFrames);
}


TEST(CR_TRR97_Horizontal_Slider, AveragesEdge)
{
    const auto vision = makeTRR97HorizontalSlider();
    const auto val = CR::frameAreaContributions(vision.frames());
    const auto averages = CR::computeAverages(val);

    ASSERT_EQ(averages.size(), 7);

    auto avgEdgesView =
      averages | std::views::values
      | std::views::transform([](auto const & cond) { return cond.average->edge; });

    const std::vector avgEdges(std::ranges::begin(avgEdgesView), std::ranges::end(avgEdgesView));

    const std::vector expectedAvgEdges = {
        0.095272, 0.095272, 0.198207, 0.198207, 0.148171, 0.148171, 0.148171};

    Helper::testVectors("CR Avg Edge", expectedAvgEdges, avgEdges);
}


TEST(CR_TRR97_Horizontal_Slider, CRf)
{
    const auto window{makeTRR97HorizontalSlider()};

    const auto [values, average] = CR::crf(window.frames());

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H30()), 83.990777, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 67.596681, eps);
    EXPECT_NEAR(values.at(Humidity::H70()), 55.442167, eps);

    EXPECT_NEAR(average, 65.185479, eps);
}

TEST(CR_TRR97_Horizontal_Slider, CRe)
{
    const auto vision{makeTRR97HorizontalSlider()};

    auto [values, average] = CR::cre(vision.frames());

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H30()), 71.094122, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 47.240183, eps);
    EXPECT_NEAR(values.at(Humidity::H70()), 35.244324, eps);

    EXPECT_NEAR(average, 47.161751, eps);
}

TEST(CR_TRR97_Horizontal_Slider, CRg1)
{
    const auto window{makeTRR97HorizontalSlider()};

    constexpr auto outsideTemperature{255.15};

    auto [values, average] =
      CR::crg(window.vision1(), CR::defaultDewPointSettings(), outsideTemperature);

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H70()), 54.361923, eps);
    EXPECT_NEAR(values.at(Humidity::H30()), 100.0, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 100.0, eps);

    EXPECT_NEAR(average, 68.356325, eps);
}

TEST(CR_TRR97_Horizontal_Slider, CR)
{
    const auto window{makeTRR97HorizontalSlider()};

    constexpr auto outsideTemperature{255.15};

    const auto [values, average] =
      CR::cr(window, CR::defaultDewPointSettings(), outsideTemperature);

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H30()), 83.284748, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 67.596681, eps);
    EXPECT_NEAR(values.at(Humidity::H70()), 49.448451, eps);

    EXPECT_NEAR(average, 62.182873, eps);
}

TEST(CR_TRR97_Horizontal_Slider, CRb)
{
    const auto window{makeTRR97HorizontalSlider()};

    constexpr auto outsideTemperature{255.15};

    const auto [values, average] =
      CR::crb(window, CR::defaultDewPointSettings(), outsideTemperature);

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H30()), 71.094122, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 47.240183, eps);
    EXPECT_NEAR(values.at(Humidity::H70()), 35.244324, eps);

    EXPECT_NEAR(average, 47.161751, eps);
}

/// Dividers tests
/// Next sets of tests are used to test dividiers
///

TEST(CR_TRR97_Horizontal_Slider, CR_no_div)
{
    const auto window{makeTRR97HorizontalSlider()};

    const auto val = CR::dividerAreaContribution(window);

    EXPECT_EQ(val.has_value(), false);
}

TEST(CR_TRR97_Horizontal_Slider, CR_no_div_edge)
{
    const auto window{makeTRR97HorizontalSlider()};

    const auto val = CR::dividerEdgeAreaContribution(window);

    EXPECT_EQ(val.has_value(), false);
}

TEST(CR_TRR97_Horizontal_Slider, CR_div_frame)
{
    const auto window = makeTRR97HorizontalSliderWithDividers();
    const auto vals = CR::dividerAreaContribution(window);

    ASSERT_EQ(vals.has_value(), true);

    std::vector<double> dividerVals;
    dividerVals.reserve(3);

    for(const auto & c : vals.value().data)
    {
        dividerVals.push_back(c.frame);
    }

    const std::vector expectedDividerVals = {
      0.0,
      0.0067814402282237998,
      0.15709045529365501,
    };

    Helper::testVectors("CR Divider Values", expectedDividerVals, dividerVals);
}

TEST(CR_TRR97_Horizontal_Slider, CR_div_edge)
{
    const auto window = makeTRR97HorizontalSliderWithDividers();
    const auto vals = CR::dividerEdgeAreaContribution(window);

    ASSERT_EQ(vals.has_value(), true);

    std::vector<double> dividerVals;
    dividerVals.reserve(3);

    for(const auto & c : vals.value().data)
    {
        dividerVals.push_back(c.edge);
    }

    const std::vector expectedDividerVals = {
      0.0,
      6.6004547988995896e-05,
      0.11303797364234899,
    };

    Helper::testVectors("CR Divider Edge Values", expectedDividerVals, dividerVals);
}

TEST(CR_TRR97_Horizontal_Slider, CR_div_and_edge_average)
{
    const auto window = makeTRR97HorizontalSliderWithDividers();
    const auto vals = CR::dividerAreaContribution(window);
    ASSERT_EQ(vals.has_value(), true);

    const auto average = CR::computeAverage(vals.value());

    EXPECT_EQ(average.average.has_value(), true);
    EXPECT_NEAR(average.average->frame, 0.0546239651739596, eps);
    EXPECT_NEAR(average.average->edge, 0.037701326063445999, eps);
}

TEST(CR_TRR97_Horizontal_Slider, CRdiv)
{
    const auto window{makeTRR97HorizontalSliderWithDividers()};

    const auto values = CR::crdiv(window);

    ASSERT_EQ(values.has_value(), true);

    ASSERT_EQ(values->values.size(), 3U);

    EXPECT_NEAR(values->values.at(Humidity::H30()), 100.0, eps);
    EXPECT_NEAR(values->values.at(Humidity::H50()), 81.071887, eps);
    EXPECT_NEAR(values->values.at(Humidity::H70()), 46.042734, eps);

    EXPECT_NEAR(values->average, 62.057343, eps);
}

TEST(CR_TRR97_Horizontal_Slider, CRdiv_edge)
{
    const auto window{makeTRR97HorizontalSliderWithDividers()};

    const auto values = CR::crdive(window);

    ASSERT_EQ(values.has_value(), true);

    ASSERT_EQ(values->values.size(), 3U);

    EXPECT_NEAR(values->values.at(Humidity::H30()), 100.0, eps);
    EXPECT_NEAR(values->values.at(Humidity::H50()), 95.958667, eps);
    EXPECT_NEAR(values->values.at(Humidity::H70()), 51.648704, eps);

    EXPECT_NEAR(values->average, 66.468560, eps);
}

TEST(CR_TRR97_Horizontal_Slider, CRf_with_divider)
{
    const auto vision{makeTRR97HorizontalSliderWithDividers()};

    const auto [values, average] = CR::crf(vision.frames());

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H30()), 83.990777, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 67.596681, eps);
    EXPECT_NEAR(values.at(Humidity::H70()), 55.442167, eps);

    EXPECT_NEAR(average, 65.185479, eps);
}

TEST(CR_TRR97_Horizontal_Slider, CRe_with_divider)
{
    const auto vision{makeTRR97HorizontalSliderWithDividers()};

    auto [values, average] = CR::cre(vision.frames());

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H30()), 71.106514, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 47.241425, eps);
    EXPECT_NEAR(values.at(Humidity::H70()), 35.237874, eps);

    EXPECT_NEAR(average, 47.160170, eps);
}

TEST(CR_TRR97_Horizontal_Slider, CRg1_with_divider)
{
    const auto window{makeTRR97HorizontalSliderWithDividers()};

    constexpr auto outsideTemperature{255.15};

    auto [values, average] =
      CR::crg(window.vision1(), CR::defaultDewPointSettings(), outsideTemperature);

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H70()), 51.715650, eps);
    EXPECT_NEAR(values.at(Humidity::H30()), 100.0, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 100.0, eps);

    EXPECT_NEAR(average, 66.521502, eps);
}

TEST(CR_TRR97_Horizontal_Slider, CR_with_divider)
{
    const auto window{makeTRR97HorizontalSliderWithDividers()};

    constexpr auto outsideTemperature{255.15};

    const auto [values, average] =
      CR::cr(window, CR::defaultDewPointSettings(), outsideTemperature);

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H30()), 82.877042, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 67.596681, eps);
    EXPECT_NEAR(values.at(Humidity::H70()), 47.383003, eps);

    EXPECT_NEAR(average, 60.760372, eps);
}

TEST(CR_TRR97_Horizontal_Slider, CRb_with_divier)
{
    const auto window{makeTRR97HorizontalSliderWithDividers()};

    constexpr auto outsideTemperature{255.15};

    const auto [values, average] =
      CR::crb(window, CR::defaultDewPointSettings(), outsideTemperature);

    ASSERT_EQ(values.size(), 3);

    EXPECT_NEAR(values.at(Humidity::H30()), 71.106514, eps);
    EXPECT_NEAR(values.at(Humidity::H50()), 47.241425, eps);
    EXPECT_NEAR(values.at(Humidity::H70()), 35.237874, eps);

    EXPECT_NEAR(average, 47.160170, eps);
}