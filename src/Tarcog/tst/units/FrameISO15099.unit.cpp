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

    const double uValue{1.0};
    const double edgeUValue{1.0};
    const double projectedFrameDimension{0.2};
    const double wettedLength{0.3};
    const double absorptance{0.3};

    Tarcog::ISO15099::FrameData frameData{
      uValue, edgeUValue, projectedFrameDimension, wettedLength, absorptance};

    const double frameLength{1.0};
    Tarcog::ISO15099::Frame frame{frameLength, Tarcog::ISO15099::FrameType::Exterior, frameData};

    Tarcog::ISO15099::Frame leftFrame{frameLength, Tarcog::ISO15099::FrameType::Exterior, frameData};

    frame.assignFrame(leftFrame, Tarcog::ISO15099::FrameSide::Left);

    const double projectedArea{frame.projectedArea()};
    EXPECT_NEAR(0.18, projectedArea, 1e-6);

    const double eogLength{frame.edgeOfAreaLength()};
    EXPECT_NEAR(0.8, eogLength, 1e-6);

    const double wettedArea{frame.wettedArea()};
    EXPECT_NEAR(0.27, wettedArea, 1e-6);
}

TEST_F(TestFrameISO15099, ExteriorFrameLeftSideFrameInterior)
{
    SCOPED_TRACE("Begin Test: Left side frame assigned.");

    const double uValue{1.0};
    const double edgeUValue{1.0};
    const double projectedFrameDimension{0.2};
    const double wettedLength{0.3};
    const double absorptance{0.3};

    Tarcog::ISO15099::FrameData frameData{
      uValue, edgeUValue, projectedFrameDimension, wettedLength, absorptance};

    const double frameLength{1.0};
    Tarcog::ISO15099::Frame frame{frameLength, Tarcog::ISO15099::FrameType::Exterior, frameData};

    const Tarcog::ISO15099::Frame leftFrame{frameLength, Tarcog::ISO15099::FrameType::Interior, frameData};

    frame.assignFrame(leftFrame, Tarcog::ISO15099::FrameSide::Left);

    const double projectedArea{frame.projectedArea()};
    EXPECT_NEAR(0.2, projectedArea, 1e-6);

    const double eogLength{frame.edgeOfAreaLength()};
    EXPECT_NEAR(0.8, eogLength, 1e-6);

    const double wettedArea{frame.wettedArea()};
    EXPECT_NEAR(0.3, wettedArea, 1e-6);
}