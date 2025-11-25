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
    Tarcog::ISO15099::DualVisionHorizontal createWindow(FnLeft && mkLeft, FnRight && mkRight)
    {
        constexpr auto width{1.5};
        constexpr auto height{1.2};

        auto left = std::invoke(std::forward<FnLeft>(mkLeft));
        auto right = std::invoke(std::forward<FnRight>(mkRight));   // NOTE: FnRight here (bugfix)

        auto window = Window::makeDualVisionHorizontal(
          width, height, left, kDoubleClear.optics, right, kDoubleClear.optics);

        return Window::withDefaultDualHorizontalFrames(window);
    }
}   // namespace

TEST(TestHorizontalSliderWindow, FramesRetrieval)
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

    auto window = Tarcog::ISO15099::DualVisionHorizontal(
      width,
      height,
      tVis,
      tSol,
      std::make_shared<Tarcog::ISO15099::SimpleIGU>(iguUValue, shgc, hcout),
      tVis,
      tSol,
      std::make_shared<Tarcog::ISO15099::SimpleIGU>(iguUValue, shgc, hcout));

    using FP = Tarcog::ISO15099::DualHorizontalFramePosition;

    window.setFrameData({{FP::Left, frameData},
                         {FP::Right, frameData},
                         {FP::BottomLeft, frameData},
                         {FP::BottomRight, frameData},
                         {FP::TopLeft, frameData},
                         {FP::TopRight, frameData},
                         {FP::MeetingRail, frameData}});

    using Tarcog::ISO15099::DualHorizontalFramePosition;

    /// Testing frame areas

    std::map<DualHorizontalFramePosition, double> areas;
    for(const auto & [framePosition, frame] : window.frames())
    {
        areas.emplace(framePosition, Tarcog::ISO15099::frameArea(frame));
    }

    const std::map<DualHorizontalFramePosition, double> correctAreas{
      {DualHorizontalFramePosition::TopLeft, 0.029196819515499996},
      {DualHorizontalFramePosition::TopRight, 0.029196819515499996},
      {DualHorizontalFramePosition::Left, 0.07363753903100001},
      {DualHorizontalFramePosition::Right, 0.07363753903100001},
      {DualHorizontalFramePosition::BottomLeft, 0.029196819515499996},
      {DualHorizontalFramePosition::BottomRight, 0.029196819515499996},
      {DualHorizontalFramePosition::MeetingRail, 0.071055578061999997}
    };

    Helper::testMaps("Single vision frame areas", correctAreas, areas, 1e-6);

    /// Testing edge areas

    std::map<DualHorizontalFramePosition, double> edgeAreas;
    for(const auto & [framePosition, frame] : window.frames())
    {
        edgeAreas.emplace(framePosition, Tarcog::ISO15099::edgeOfGlassArea(frame));
    }

    const std::map<DualHorizontalFramePosition, double> correctEdgeAreas{
          {DualHorizontalFramePosition::TopLeft, 0.03124393675},
          {DualHorizontalFramePosition::TopRight, 0.03124393675},
          {DualHorizontalFramePosition::Left, 0.084764499},
          {DualHorizontalFramePosition::Right, 0.084764499},
          {DualHorizontalFramePosition::BottomLeft, 0.03124393675},
          {DualHorizontalFramePosition::BottomRight, 0.03124393675},
          {DualHorizontalFramePosition::MeetingRail, 0.161464498}
        };

    Helper::testMaps("Single vision frame areas", correctEdgeAreas, edgeAreas, 1e-6);
}

TEST(TestHorizontalSliderWindow, PredefinedCOGValues)
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

    auto window = Tarcog::ISO15099::DualVisionHorizontal(
      width,
      height,
      tVis,
      tSol,
      std::make_shared<Tarcog::ISO15099::SimpleIGU>(iguUValue, shgc, hcout),
      tVis,
      tSol,
      std::make_shared<Tarcog::ISO15099::SimpleIGU>(iguUValue, shgc, hcout));

    using FP = Tarcog::ISO15099::DualHorizontalFramePosition;

    window.setFrameData({{FP::Left, frameData},
                         {FP::Right, frameData},
                         {FP::BottomLeft, frameData},
                         {FP::BottomRight, frameData},
                         {FP::TopLeft, frameData},
                         {FP::TopRight, frameData},
                         {FP::MeetingRail, frameData}});

    const double vt{window.vt()};
    EXPECT_NEAR(0.519647, vt, 1e-6);

    const double uvalue{window.uValue()};
    EXPECT_NEAR(1.902392, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.357692, windowSHGC, 1e-6);
}

TEST(TestHorizontalSliderWindow, FrameAreas)
{
    auto window{createWindow(makeWinterSystem, makeWinterSystem)};

    using FP = Tarcog::ISO15099::DualHorizontalFramePosition;

    EXPECT_NEAR(0.031237, window.getFrameArea(FP::TopLeft), 1e-6);
    EXPECT_NEAR(0.049612, window.getFrameArea(FP::Left), 1e-6);
    EXPECT_NEAR(0.031237, window.getFrameArea(FP::TopRight), 1e-6);
    EXPECT_NEAR(0.047774, window.getFrameArea(FP::MeetingRail), 1e-6);
    EXPECT_NEAR(0.049612, window.getFrameArea(FP::Right), 1e-6);
    EXPECT_NEAR(0.031237, window.getFrameArea(FP::BottomLeft), 1e-6);
    EXPECT_NEAR(0.031237, window.getFrameArea(FP::BottomRight), 1e-6);

    EXPECT_NEAR(0.271946, window.getFrameArea(), 1e-6);
}

TEST(TestHorizontalSliderWindow, FrameEdgeOfGlassAreas)
{
    auto window{createWindow(makeWinterSystem, makeWinterSystem)};

    using FP = Tarcog::ISO15099::DualHorizontalFramePosition;

    EXPECT_NEAR(0.041525, window.getFrameEdgeOfGlassArea(FP::TopLeft), 1e-6);
    EXPECT_NEAR(0.066723, window.getFrameEdgeOfGlassArea(FP::Left), 1e-6);
    EXPECT_NEAR(0.041525, window.getFrameEdgeOfGlassArea(FP::TopRight), 1e-6);
    EXPECT_NEAR(0.125381, window.getFrameEdgeOfGlassArea(FP::MeetingRail), 1e-6);
    EXPECT_NEAR(0.066723, window.getFrameEdgeOfGlassArea(FP::Right), 1e-6);
    EXPECT_NEAR(0.041525, window.getFrameEdgeOfGlassArea(FP::BottomLeft), 1e-6);
    EXPECT_NEAR(0.041525, window.getFrameEdgeOfGlassArea(FP::BottomRight), 1e-6);

    EXPECT_NEAR(0.424926, window.getFrameEdgeOfGlassArea(), 1e-6);
}

TEST(TestHorizontalSliderWindow, DoubleClearAirWinter)
{
    constexpr auto width{1.5};
    constexpr auto height{1.2};

    // Optics now come from the IGU preset
    const auto & optics = kDoubleClear.optics;

    auto window = Window::makeDualVisionHorizontal(
      width, height, makeWinterSystem(), optics, makeWinterSystem(), optics);

    window = Window::withDefaultDualHorizontalFrames(window);

    const double vt{window.vt()};
    EXPECT_NEAR(0.667335, vt, 1e-6);

    const double uvalue{window.uValue()};
    EXPECT_NEAR(2.500568, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.002654, windowSHGC, 1e-6);
}

TEST(TestHorizontalSliderWindow, DoubleClearAirSummer)
{
    constexpr auto width{1.5};
    constexpr auto height{1.2};

    // Optics now come from the IGU preset
    const auto & optics = kDoubleClear.optics;

    auto window = Window::makeDualVisionHorizontal(
      width, height, makeSummerSystem(), optics, makeSummerSystem(), optics);

    window = Window::withDefaultDualHorizontalFrames(window);

    const double vt{window.vt()};
    EXPECT_NEAR(0.667335, vt, 1e-6);

    const double uvalue{window.uValue()};
    EXPECT_NEAR(2.589659, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.601122, windowSHGC, 1e-6);
}