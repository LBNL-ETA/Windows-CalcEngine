#include <gtest/gtest.h>

#include "WCETarcog.hpp"

class TestFrameISO15099 : public testing::Test
{
protected:
    void SetUp() override
    {}
};

TEST_F(TestFrameISO15099, ExteriorFrameLeftSideFrameExterior)
{
    SCOPED_TRACE("Begin Test: Left side frame exterior.");

    constexpr double uValue{1.0};
    constexpr double edgeUValue{1.0};
    constexpr double projectedFrameDimension{0.2};
    constexpr double wettedLength{0.3};
    constexpr double absorptance{0.3};

    constexpr Tarcog::ISO15099::FrameData frameData{.UValue = uValue,
                                                    .EdgeUValue = edgeUValue,
                                                    .ProjectedFrameDimension =
                                                      projectedFrameDimension,
                                                    .WettedLength = wettedLength,
                                                    .Absorptance = absorptance};

    const double frameLength{1.0};
    Tarcog::ISO15099::Frame frame{frameLength, Tarcog::ISO15099::FrameType::Exterior, frameData};

    Tarcog::ISO15099::Frame leftFrame{
      frameLength, Tarcog::ISO15099::FrameType::Exterior, frameData};

    frame.frame[Tarcog::ISO15099::FrameSide::Left] = leftFrame;

    const double projectedArea{Tarcog::ISO15099::frameArea(frame)};
    EXPECT_NEAR(0.18, projectedArea, 1e-6);

    const double eogArea{Tarcog::ISO15099::edgeOfGlassArea(frame)};
    EXPECT_NEAR(0.048783875, eogArea, 1e-6);

    const double wettedArea{Tarcog::ISO15099::wettedArea(frame)};
    EXPECT_NEAR(0.27, wettedArea, 1e-6);
}

TEST_F(TestFrameISO15099, ExteriorFrameLeftSideFrameInterior)
{
    SCOPED_TRACE("Begin Test: Left side frame assigned.");

    constexpr double uValue{1.0};
    constexpr double edgeUValue{1.0};
    constexpr double projectedFrameDimension{0.2};
    constexpr double wettedLength{0.3};
    constexpr double absorptance{0.3};

    constexpr Tarcog::ISO15099::FrameData frameData{.UValue = uValue,
                                                    .EdgeUValue = edgeUValue,
                                                    .ProjectedFrameDimension =
                                                      projectedFrameDimension,
                                                    .WettedLength = wettedLength,
                                                    .Absorptance = absorptance};

    constexpr double frameLength{1.0};
    Tarcog::ISO15099::Frame frame{frameLength, Tarcog::ISO15099::FrameType::Exterior, frameData};

    const Tarcog::ISO15099::Frame leftFrame{
      frameLength, Tarcog::ISO15099::FrameType::Interior, frameData};

    frame.frame[Tarcog::ISO15099::FrameSide::Left] = leftFrame;

    const double projectedArea{Tarcog::ISO15099::frameArea(frame)};
    EXPECT_NEAR(0.2, projectedArea, 1e-6);

    const double eogArea{Tarcog::ISO15099::edgeOfGlassArea(frame)};
    EXPECT_NEAR(0.0508, eogArea, 1e-6);

    const double wettedArea{Tarcog::ISO15099::wettedArea(frame)};
    EXPECT_NEAR(0.3, wettedArea, 1e-6);
}

TEST_F(TestFrameISO15099, InteriorFrameLeftandRightSideFramesExterior)
{
    SCOPED_TRACE("Begin Test: Frame is interior and left and right frames are exterior.");

    constexpr double uValue{1.0};
    constexpr double edgeUValue{1.0};
    constexpr double projectedFrameDimension{0.2};
    constexpr double wettedLength{0.3};
    constexpr double absorptance{0.3};

    constexpr Tarcog::ISO15099::FrameData frameData{.UValue = uValue,
                                                    .EdgeUValue = edgeUValue,
                                                    .ProjectedFrameDimension =
                                                      projectedFrameDimension,
                                                    .WettedLength = wettedLength,
                                                    .Absorptance = absorptance};

    constexpr double frameLength{1.0};
    Tarcog::ISO15099::Frame frame{frameLength, Tarcog::ISO15099::FrameType::Interior, frameData};

    const Tarcog::ISO15099::Frame leftFrame{
      frameLength, Tarcog::ISO15099::FrameType::Exterior, frameData};
    const Tarcog::ISO15099::Frame rightFrame{
      frameLength, Tarcog::ISO15099::FrameType::Exterior, frameData};

    frame.frame[Tarcog::ISO15099::FrameSide::Left] = leftFrame;
    frame.frame[Tarcog::ISO15099::FrameSide::Right] = rightFrame;

    const double projectedArea{Tarcog::ISO15099::frameArea(frame)};
    EXPECT_NEAR(0.12, projectedArea, 1e-6);

    const double eogArea{Tarcog::ISO15099::edgeOfGlassArea(frame)};
    EXPECT_NEAR(0.0300355, eogArea, 1e-6);

    const double wettedArea{Tarcog::ISO15099::wettedArea(frame)};
    EXPECT_NEAR(0.18, wettedArea, 1e-6);

    const double calculatedSHGC{Tarcog::ISO15099::frameSHGC(frameData.Absorptance,
                                                            frameData.UValue,
                                                            frameData.ProjectedFrameDimension,
                                                            frameData.WettedLength,
                                                            15)};
    EXPECT_NEAR(0.013333, calculatedSHGC, 1e-6);
}