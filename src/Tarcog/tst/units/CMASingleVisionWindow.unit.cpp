#include <gtest/gtest.h>

#include "WCETarcog.hpp"

class TestCMASingleVisionWindow : public testing::Test
{
protected:
    void SetUp() override
    {}
};

TEST_F(TestCMASingleVisionWindow, CMASingleVision)
{
    SCOPED_TRACE("Begin Test: CMA test for single vision window.");

    const double uValue{3.16130853830995};
    const double edgeUValue{3.16130853830995};
    const double projectedFrameDimension{0.0610362};
    const double wettedLength{0.0661612};
    const double absorptance{0.3};

    Tarcog::ISO15099::FrameData frameData{
      uValue, edgeUValue, projectedFrameDimension, wettedLength, absorptance};

    CMA::CMAFrame cmaFrame{frameData, frameData, frameData, frameData};

    const auto width{1.2};
    const auto height{1.5};
    const auto tVis{0.535};
    const auto tSol{0.3716};

    auto window{CMA::CMAWindowSingleVision(width, height, tVis, tSol)};

    window.setFrameTop(cmaFrame);
    window.setFrameBottom(cmaFrame);
    window.setFrameLeft(cmaFrame);
    window.setFrameRight(cmaFrame);

    const auto UvalueCOG{1.258};
    const auto SHGCCOG{0.341};
    const auto spacerKeff{2.05280045621776};

    const double vt{window.vt()};
    EXPECT_NEAR(0.441466, vt, 1e-6);

    const double uvalue{window.uValue(UvalueCOG, spacerKeff)};
    EXPECT_NEAR(1.903495, uvalue, 1e-6);

    const double windowSHGC{window.shgc(SHGCCOG, spacerKeff)};
    EXPECT_NEAR(0.287266, windowSHGC, 1e-6);
}
