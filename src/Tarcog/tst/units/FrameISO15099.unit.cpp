#include <gtest/gtest.h>

#include "WCETarcog.hpp"

class TestFrameISO15099 : public testing::Test
{
protected:
    void SetUp() override
    {}
};

TEST_F(TestFrameISO15099, SquareFrame)
{
    SCOPED_TRACE("Begin Test: Square frame.");

    const double uValue{1.0};
    const double edgeUValue{1.0};
    const double projectedFrameDimension{0.2};
    const double wettedLength{0.3};
    const double absorptance{0.3};

    Tarcog::ISO15099::FrameData frameData{
      uValue, edgeUValue, projectedFrameDimension, wettedLength, absorptance};

    const double frameLength{1.0};
    Tarcog::ISO15099::Frame frame{
      frameLength, Tarcog::ISO15099::FrameGeometryType::Square, frameData};

    const double projectedArea{frame.projectedArea()};
    EXPECT_NEAR(0.2, projectedArea, 1e-6);

    const double wettedArea{frame.wettedArea()};
    EXPECT_NEAR(0.3, wettedArea, 1e-6);
}

TEST_F(TestFrameISO15099, HalfTrapezoidFrame)
{
    SCOPED_TRACE("Begin Test: Half trapezoid frame.");

    const double uValue{1.0};
    const double edgeUValue{1.0};
    const double projectedFrameDimension{0.2};
    const double wettedLength{0.3};
    const double absorptance{0.3};

    Tarcog::ISO15099::FrameData frameData{
      uValue, edgeUValue, projectedFrameDimension, wettedLength, absorptance};

    const double frameLength{1.0};
    Tarcog::ISO15099::Frame frame{
      frameLength, Tarcog::ISO15099::FrameGeometryType::HalfTrapezoid, frameData};

    const double projectedArea{frame.projectedArea()};
    EXPECT_NEAR(0.18, projectedArea, 1e-6);

    const double wettedArea{frame.wettedArea()};
    EXPECT_NEAR(0.255, wettedArea, 1e-6);
}

TEST_F(TestFrameISO15099, TrapezoidFrame)
{
    SCOPED_TRACE("Begin Test: Trapezoid frame.");

    const double uValue{1.0};
    const double edgeUValue{1.0};
    const double projectedFrameDimension{0.2};
    const double wettedLength{0.3};
    const double absorptance{0.3};

    Tarcog::ISO15099::FrameData frameData{
      uValue, edgeUValue, projectedFrameDimension, wettedLength, absorptance};

    const double frameLength{1.0};
    Tarcog::ISO15099::Frame frame{
      frameLength, Tarcog::ISO15099::FrameGeometryType::Trapezoid, frameData};

    const double projectedArea{frame.projectedArea()};
    EXPECT_NEAR(0.16, projectedArea, 1e-6);

    const double wettedArea{frame.wettedArea()};
    EXPECT_NEAR(0.21, wettedArea, 1e-6);
}