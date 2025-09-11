#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "WCEViewer.hpp"


using namespace Viewer;

class TestPolarPoint : public testing::Test
{
protected:
    virtual void SetUp()
    {}
};

TEST_F(TestPolarPoint, PolarPointTest1)
{
    SCOPED_TRACE("Begin Test: Polar point conversions (1).");

    const auto aPoint{CPoint2D::createPointFromPolarCoordinates(259, 1.58)};

    double x = aPoint.x();
    double y = aPoint.y();

    EXPECT_NEAR(-0.301478213, x, 1e-6);
    EXPECT_NEAR(-1.55097095, y, 1e-6);
}

TEST_F(TestPolarPoint, PolarPointTest2)
{
    SCOPED_TRACE("Begin Test: Polar point conversions (2).");

    const auto aPoint{CPoint2D::createPointFromPolarCoordinates(43, 0.76)};

    double x = aPoint.x();
    double y = aPoint.y();

    EXPECT_NEAR(0.555828813, x, 1e-6);
    EXPECT_NEAR(0.518318754, y, 1e-6);
}
