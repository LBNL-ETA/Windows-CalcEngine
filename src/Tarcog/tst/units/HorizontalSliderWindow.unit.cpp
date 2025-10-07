#include <gtest/gtest.h>

#include <WCETarcog.hpp>

#include "thermal/commonFrames.hpp"
#include "thermal/commonThermal.hpp"

class TestHorizontalSliderWindow : public testing::Test
{
protected:
    // Double Clear Air (NFRC_103-NFRC_103) - Winter conditions
    static std::shared_ptr<Tarcog::ISO15099::CSystem> getDoubleLayerUValueBC()
    {
        return std::make_shared<Tarcog::ISO15099::CSystem>(IGU::NFRC::doubleClearAir(),
                                                           Environment::NFRC::Winter::indoor(),
                                                           Environment::NFRC::Winter::outdoor());
    }

    // Double Clear Air (NFRC_103-NFRC_103) - Summer conditions
    static std::shared_ptr<Tarcog::ISO15099::CSystem> getDoubleLayerSHGCBC()
    {
        return std::make_shared<Tarcog::ISO15099::CSystem>(IGU::NFRC::doubleClearAir(),
                                                           Environment::NFRC::Summer::indoor(),
                                                           Environment::NFRC::Summer::outdoor());
    }
};

TEST_F(TestHorizontalSliderWindow, PredefinedCOGValues)
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

TEST_F(TestHorizontalSliderWindow, FrameAreas)
{
    constexpr auto width{1.5};
    constexpr auto height{1.2};

    // These values correspond to double clear (NFRC-103; NFRC-103)
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window = Tarcog::ISO15099::DualVisionHorizontal(
      width, height, tVis, tSol, getDoubleLayerUValueBC(), tVis, tSol, getDoubleLayerUValueBC());

    using FP = Tarcog::ISO15099::DualHorizontalFramePosition;

    window.setFrameData({{FP::Left, Frame::sampleJamb()},
                         {FP::Right, Frame::sampleJamb()},
                         {FP::BottomLeft, Frame::sampleSill()},
                         {FP::BottomRight, Frame::sampleSill()},
                         {FP::TopLeft, Frame::sampleHead()},
                         {FP::TopRight, Frame::sampleHead()},
                         {FP::MeetingRail, Frame::sampleSill()}});

    EXPECT_NEAR(0.031237, window.getFrameArea(FP::TopLeft), 1e-6);
    EXPECT_NEAR(0.049612, window.getFrameArea(FP::Left), 1e-6);
    EXPECT_NEAR(0.031237, window.getFrameArea(FP::TopRight), 1e-6);
    EXPECT_NEAR(0.047774, window.getFrameArea(FP::MeetingRail), 1e-6);
    EXPECT_NEAR(0.049612, window.getFrameArea(FP::Right), 1e-6);
    EXPECT_NEAR(0.031237, window.getFrameArea(FP::BottomLeft), 1e-6);
    EXPECT_NEAR(0.031237, window.getFrameArea(FP::BottomRight), 1e-6);

    EXPECT_NEAR(0.271946, window.getFrameArea(), 1e-6);
}

TEST_F(TestHorizontalSliderWindow, DoubleClearAirWinter)
{
    constexpr auto width{1.5};
    constexpr auto height{1.2};

    // These values correspond to double clear (NFRC-103; NFRC-103)
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window = Tarcog::ISO15099::DualVisionHorizontal(
      width, height, tVis, tSol, getDoubleLayerUValueBC(), tVis, tSol, getDoubleLayerUValueBC());

    using FP = Tarcog::ISO15099::DualHorizontalFramePosition;

    window.setFrameData({{FP::Left, Frame::sampleJamb()},
                         {FP::Right, Frame::sampleJamb()},
                         {FP::BottomLeft, Frame::sampleSill()},
                         {FP::BottomRight, Frame::sampleSill()},
                         {FP::TopLeft, Frame::sampleHead()},
                         {FP::TopRight, Frame::sampleHead()},
                         {FP::MeetingRail, Frame::sampleSill()}});

    const double vt{window.vt()};
    EXPECT_NEAR(0.667335, vt, 1e-6);

    const double uvalue{window.uValue()};
    EXPECT_NEAR(2.500568, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.002654, windowSHGC, 1e-6);
}