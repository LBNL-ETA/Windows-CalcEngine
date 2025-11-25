#include <gtest/gtest.h>

#include <WCETarcog.hpp>

#include "thermal/commonThermal.hpp"
#include "mapTesting.hpp"

namespace
{
    // IGU preset with optics bound (NFRC_103-NFRC_103)
    const IGU::NFRC::Preset kDoubleClear = IGU::NFRC::doubleClearAir();

    // Callables to build systems with explicit environments
    const auto makeWinterSystem = [] {
        return System::make(
          kDoubleClear, Environment::NFRC::Winter::indoor(), Environment::NFRC::Winter::outdoor());
    };

    const auto makeSummerSystem = [] {
        return System::make(
          kDoubleClear, Environment::NFRC::Summer::indoor(), Environment::NFRC::Summer::outdoor());
    };

    // Create a window using preset optics (bound to IGU) and user-provided system factories
    template<typename FnLeft, typename FnRight>
    Tarcog::ISO15099::DualVisionVertical createWindow(FnLeft && mkLeft, FnRight && mkRight)
    {
        constexpr auto width{1.2};
        constexpr auto height{1.5};

        auto left = std::invoke(std::forward<FnLeft>(mkLeft));
        auto right = std::invoke(std::forward<FnRight>(mkRight));   // NOTE: FnRight here (bugfix)

        auto window = Window::makeDualVisionVertical(
          width, height, left, kDoubleClear.optics, right, kDoubleClear.optics);

        return Window::withDefaultDualVerticalFrames(window);
    }
}   // namespace

TEST(TestVerticalSliderWindow, FramesRetrieval)
{
    SCOPED_TRACE("Begin Test: Horizontal slider window predefined COG.");

    constexpr Tarcog::ISO15099::FrameData frameData{.UValue = 2.134059,
                                                    .EdgeUValue = 2.251039,
                                                    .ProjectedFrameDimension = 0.050813,
                                                    .WettedLength = 0.05633282,
                                                    .Absorptance = 0.3};

    constexpr auto width{1.2};
    constexpr auto height{1.5};
    constexpr auto iguUValue{1.667875};
    constexpr auto shgc{0.430713};
    constexpr auto tVis{0.638525};
    constexpr auto tSol{0.3716};
    constexpr auto hcout{15.0};

    auto window = Tarcog::ISO15099::DualVisionVertical(
      width,
      height,
      tVis,
      tSol,
      std::make_shared<Tarcog::ISO15099::SimpleIGU>(iguUValue, shgc, hcout),
      tVis,
      tSol,
      std::make_shared<Tarcog::ISO15099::SimpleIGU>(iguUValue, shgc, hcout));

    using FP = Tarcog::ISO15099::DualVerticalFramePosition;

    window.setFrameData({{FP::Top, frameData},
                         {FP::TopLeft, frameData},
                         {FP::TopRight, frameData},
                         {FP::MeetingRail, frameData},
                         {FP::BottomLeft, frameData},
                         {FP::BottomRight, frameData},
                         {FP::Bottom, frameData}});

    using Tarcog::ISO15099::DualVerticalFramePosition;

    /// Testing frame areas

    std::map<DualVerticalFramePosition, double> areas;
    for(const auto & [framePosition, frame] : window.frames())
    {
        areas.emplace(framePosition, Tarcog::ISO15099::frameArea(frame));
    }

    const std::map<DualVerticalFramePosition, double> correctAreas{
      {DualVerticalFramePosition::TopLeft, 0.0368187695155},
      {DualVerticalFramePosition::TopRight, 0.0368187695155},
      {DualVerticalFramePosition::Top, 0.058393639},
      {DualVerticalFramePosition::Bottom, 0.058393639},
      {DualVerticalFramePosition::BottomLeft, 0.0368187695155},
      {DualVerticalFramePosition::BottomRight, 0.0368187695155},
      {DualVerticalFramePosition::MeetingRail, 0.055811678062}};

    Helper::testMaps("Single vision frame areas", correctAreas, areas, 1e-6);

    /// Testing edge areas

    std::map<DualVerticalFramePosition, double> edgeAreas;
    for(const auto & [framePosition, frame] : window.frames())
    {
        edgeAreas.emplace(framePosition, Tarcog::ISO15099::edgeOfGlassArea(frame));
    }

    const std::map<DualVerticalFramePosition, double> correctEdgeAreas{
      {DualVerticalFramePosition::TopLeft, 0.04076893675},
      {DualVerticalFramePosition::TopRight, 0.04076893675},
      {DualVerticalFramePosition::Top, 0.065714499},
      {DualVerticalFramePosition::Bottom, 0.065714499},
      {DualVerticalFramePosition::BottomLeft, 0.04076893675},
      {DualVerticalFramePosition::BottomRight, 0.04076893675},
      {DualVerticalFramePosition::MeetingRail, 0.123364498}};

    Helper::testMaps("Single vision frame areas", correctEdgeAreas, edgeAreas, 1e-6);
}

TEST(TestVerticalSliderWindow, PredefinedCOGValues)
{
    SCOPED_TRACE("Begin Test: Horizontal slider window predefined COG.");

    constexpr Tarcog::ISO15099::FrameData frameData{.UValue = 2.134059,
                                                    .EdgeUValue = 2.251039,
                                                    .ProjectedFrameDimension = 0.050813,
                                                    .WettedLength = 0.05633282,
                                                    .Absorptance = 0.3};

    constexpr auto width{1.2};
    constexpr auto height{1.5};
    constexpr auto iguUValue{1.667875};
    constexpr auto shgc{0.430713};
    constexpr auto tVis{0.638525};
    constexpr auto tSol{0.3716};
    constexpr auto hcout{15.0};

    auto window = Tarcog::ISO15099::DualVisionVertical(
      width,
      height,
      tVis,
      tSol,
      std::make_shared<Tarcog::ISO15099::SimpleIGU>(iguUValue, shgc, hcout),
      tVis,
      tSol,
      std::make_shared<Tarcog::ISO15099::SimpleIGU>(iguUValue, shgc, hcout));

    using FP = Tarcog::ISO15099::DualVerticalFramePosition;

    window.setFrameData({{FP::Top, frameData},
                         {FP::TopLeft, frameData},
                         {FP::TopRight, frameData},
                         {FP::MeetingRail, frameData},
                         {FP::BottomLeft, frameData},
                         {FP::BottomRight, frameData},
                         {FP::Bottom, frameData}});

    const double vt{window.vt()};
    EXPECT_NEAR(0.525054, vt, 1e-6);

    const double uvalue{window.uValue()};
    EXPECT_NEAR(1.886101, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.361014, windowSHGC, 1e-6);
}

TEST(TestVerticalSliderWindow, FrameAreas)
{
    auto window{createWindow(makeWinterSystem, makeWinterSystem)};

    using FP = Tarcog::ISO15099::DualVerticalFramePosition;

    EXPECT_NEAR(0.049612, window.getFrameArea(FP::Top), 1e-6);
    EXPECT_NEAR(0.031237, window.getFrameArea(FP::TopLeft), 1e-6);
    EXPECT_NEAR(0.031237, window.getFrameArea(FP::TopRight), 1e-6);
    EXPECT_NEAR(0.047774, window.getFrameArea(FP::MeetingRail), 1e-6);
    EXPECT_NEAR(0.031237, window.getFrameArea(FP::BottomLeft), 1e-6);
    EXPECT_NEAR(0.031237, window.getFrameArea(FP::BottomRight), 1e-6);
    EXPECT_NEAR(0.049612, window.getFrameArea(FP::Bottom), 1e-6);

    EXPECT_NEAR(0.271947, window.getFrameArea(), 1e-6);
}

TEST(TestVerticalSliderWindow, FrameEdgeOfGlassAreas)
{
    auto window{createWindow(makeWinterSystem, makeWinterSystem)};

    using FP = Tarcog::ISO15099::DualVerticalFramePosition;

    EXPECT_NEAR(0.066723, window.getFrameEdgeOfGlassArea(FP::Top), 1e-6);
    EXPECT_NEAR(0.041525, window.getFrameEdgeOfGlassArea(FP::TopLeft), 1e-6);
    EXPECT_NEAR(0.041525, window.getFrameEdgeOfGlassArea(FP::TopRight), 1e-6);
    EXPECT_NEAR(0.125381, window.getFrameEdgeOfGlassArea(FP::MeetingRail), 1e-6);
    EXPECT_NEAR(0.041525, window.getFrameEdgeOfGlassArea(FP::BottomLeft), 1e-6);
    EXPECT_NEAR(0.041525, window.getFrameEdgeOfGlassArea(FP::BottomRight), 1e-6);
    EXPECT_NEAR(0.066723, window.getFrameEdgeOfGlassArea(FP::Bottom), 1e-6);

    EXPECT_NEAR(0.424926, window.getFrameEdgeOfGlassArea(), 1e-6);
}

TEST(TestVerticalSliderWindow, DoubleClearAirWinter)
{
    constexpr auto width{1.2};
    constexpr auto height{1.5};

    // Optics now come from the IGU preset
    const auto & optics = kDoubleClear.optics;

    auto window = Window::makeDualVisionVertical(
      width, height, makeWinterSystem(), optics, makeWinterSystem(), optics);

    window = Window::withDefaultDualVerticalFrames(window);

    const double vt{window.vt()};
    EXPECT_NEAR(0.591272, vt, 1e-6);

    const double uvalue{window.uValue()};
    EXPECT_NEAR(2.486524, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.002725, windowSHGC, 1e-6);
}

TEST(TestVerticalSliderWindow, DoubleClearAirSummer)
{
    constexpr auto width{1.2};
    constexpr auto height{1.5};

    // Optics now come from the IGU preset
    const auto & optics = kDoubleClear.optics;

    auto window = Window::makeDualVisionVertical(
      width, height, makeSummerSystem(), optics, makeSummerSystem(), optics);

    window = Window::withDefaultDualVerticalFrames(window);

    const double vt{window.vt()};
    EXPECT_NEAR(0.591272, vt, 1e-6);

    const double uvalue{window.uValue()};
    EXPECT_NEAR(2.579044, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.676736, windowSHGC, 1e-6);
}