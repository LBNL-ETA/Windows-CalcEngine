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

    using namespace Tarcog::ISO15099;

    const double uValue{1.0};
    const double edgeUValue{1.0};
    const double projectedFrameDimension{0.2};
    const double wettedLength{0.3};
    const double absorptance{0.3};

    FrameData frameData{uValue, edgeUValue, projectedFrameDimension, wettedLength, absorptance};

    const double frameLength{1.0};
    Frame frame{frameLength, FrameGeometryType::Square, frameData};

    const double area{frame.area()};

    EXPECT_NEAR(0.2, area, 1e-6);
}