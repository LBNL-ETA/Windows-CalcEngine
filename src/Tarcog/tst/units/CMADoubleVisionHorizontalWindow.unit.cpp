#include <gtest/gtest.h>

#include "WCETarcog.hpp"

class TestCMADoubleVisionHorizontalWindow : public testing::Test
{
protected:
    void SetUp() override
    {}
};

TEST_F(TestCMADoubleVisionHorizontalWindow, CMASingleVision)
{
    SCOPED_TRACE("Begin Test: CMA test for double vision horizontal window.");

    const double uValue{3.16130853830995};
    const double edgeUValue{3.16130853830995};
    const double projectedFrameDimension{0.0610362};
    const double wettedLength{0.0661612};
    const double absorptance{0.3};

    Tarcog::ISO15099::FrameData frameData{
      uValue, edgeUValue, projectedFrameDimension, wettedLength, absorptance};

    const CMA::CMAFrame cmaFrame{frameData, frameData, frameData, frameData};

    const auto width{1.2};
    const auto height{1.5};    

    auto window{CMA::CMAWindowDualVisionHorizontal(width, height)};

    window.setFrameTopLeft(cmaFrame);
    window.setFrameTopRight(cmaFrame);
    window.setFrameBottomLeft(cmaFrame);
    window.setFrameBottomRight(cmaFrame);
    window.setFrameLeft(cmaFrame);
    window.setFrameRight(cmaFrame);
    window.setFrameMeetingRail(cmaFrame);

    const auto UvalueCOG{1.258};
    const auto SHGCCOG{0.341};
    const auto tVis{0.535};
    const auto tSol{0.3716};
    const auto spacerKeff{2.05280045621776};

    const double vt{window.vt(tVis)};
    EXPECT_NEAR(0.416469, vt, 1e-6);

    const double uvalue{window.uValue(UvalueCOG, spacerKeff)};
    EXPECT_NEAR(2.152215, uvalue, 1e-6);

    const double windowSHGC{window.shgc(SHGCCOG, tSol, spacerKeff)};
    EXPECT_NEAR(0.272906, windowSHGC, 1e-6);
}
