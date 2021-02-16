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

    const double uValue{2.134059};
    const double edgeUValue{2.251039};
    const double projectedFrameDimension{0.050813};
    const double wettedLength{0.05633282};
    const double absorptance{0.3};

    Tarcog::ISO15099::FrameData frameData{
      uValue, edgeUValue, projectedFrameDimension, wettedLength, absorptance};

    const auto width{1.2};
    const auto height{1.5};
    const auto iguUValue{1.667875};
    const auto shgc{0.430713};
    const auto tVis{0.638525};
    const auto tSol{0.3716};
    const auto hcout{15.0};

    auto window = Tarcog::ISO15099::WindowSingleVision::Create(
      width,
      height,
      tVis,
      tSol,
      std::make_shared<Tarcog::ISO15099::SimpleIGU>(iguUValue, shgc, hcout));

    window.setFrameTop(frameData);
    window.setFrameBottom(frameData);
    window.setFrameLeft(frameData);
    window.setFrameRight(frameData);

    const double vt{window.vt()};
    EXPECT_NEAR(0.544853, vt, 1e-6);

    const double uvalue{window.uValue()};
    EXPECT_NEAR(1.833769, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.373175, windowSHGC, 1e-6);
}
