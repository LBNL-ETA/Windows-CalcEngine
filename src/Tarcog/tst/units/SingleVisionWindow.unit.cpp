#include <gtest/gtest.h>

#include "WCETarcog.hpp"

class TestSingleVisionWindow : public testing::Test
{
protected:
    void SetUp() override
    {}
};

TEST_F(TestSingleVisionWindow, ExteriorFrameLeftSideFrameExterior)
{
    SCOPED_TRACE("Begin Test: Left side frame exterior.");

    const double uValue{2.134};
    const double edgeUValue{2.251};
    const double projectedFrameDimension{0.050813};
    const double wettedLength{0.05633282};
    const double absorptance{0.3};

    Tarcog::ISO15099::FrameData frameData{
      uValue, edgeUValue, projectedFrameDimension, wettedLength, absorptance};

    const auto width{1.2};
    const auto height{1.5};
    const auto iguUValue{1.668};
    const auto shgc{0.431};
    const auto tVis{0.639};
    const auto tSol{0.3716};
    const auto hcout{15.0};

    const Tarcog::ISO15099::SimpleIGU simpleIGU{iguUValue, shgc, hcout};

    Tarcog::ISO15099::WindowVision vision{width, height, tVis, tSol, simpleIGU};

    vision.setFrameData(Tarcog::FramePosition::Top, frameData);
    vision.setFrameData(Tarcog::FramePosition::Bottom, frameData);
    vision.setFrameData(Tarcog::FramePosition::Left, frameData);
    vision.setFrameData(Tarcog::FramePosition::Right, frameData);

    Tarcog::ISO15099::WindowSingleVision window{vision};

    const double vt{window.vt()};
    EXPECT_NEAR(0.545257864, vt, 1e-6);

    const double uvalue{window.uValue()};
    EXPECT_NEAR(1.833840, uvalue, 1e-6);
}