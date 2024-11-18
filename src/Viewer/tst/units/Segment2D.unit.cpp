#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "WCEViewer.hpp"


using namespace Viewer;

class TestSegment2D : public testing::Test
{
protected:
    virtual void SetUp()
    {}
};

TEST_F(TestSegment2D, Segment2DTest1)
{
    CPoint2D aStartPoint{0, 0};
    CPoint2D aEndPoint{10, 0};

    CSegment2D aSegment = CSegment2D(aStartPoint, aEndPoint);

    const double length = aSegment.length();

    EXPECT_NEAR(10, length, 1e-6);

    const auto angle = aSegment.angle();
    EXPECT_NEAR(0, angle, 1e-6);

    const auto surfaceNormal{aSegment.surfaceUnitNormal()};
    const CPoint2D correctSurfaceNormal{0.0, 1.0};

    EXPECT_TRUE(correctSurfaceNormal == surfaceNormal);
}

TEST_F(TestSegment2D, Segment2DTest2)
{
    CPoint2D aStartPoint{0, 0};
    CPoint2D aEndPoint{10, 10};

    CSegment2D aSegment = CSegment2D(aStartPoint, aEndPoint);

    const double length = aSegment.length();

    EXPECT_NEAR(14.14213562, length, 1e-6);

    const auto angle = aSegment.angle();

    EXPECT_NEAR(45, angle, 1e-6);

    const auto surfaceNormal{aSegment.surfaceUnitNormal()};
    const CPoint2D correctSurfaceNormal{0.7071067811865476, -0.7071067811865476};

    EXPECT_TRUE(correctSurfaceNormal == surfaceNormal);
}

TEST_F(TestSegment2D, Segment2DTest3)
{
    CPoint2D aStartPoint{10, 10};
    CPoint2D aEndPoint{0, 0};

    CSegment2D aSegment = CSegment2D(aStartPoint, aEndPoint);

    const double length = aSegment.length();
    EXPECT_NEAR(14.14213562, length, 1e-6);

    const auto angle = aSegment.angle();
    EXPECT_NEAR(45, angle, 1e-6);

    const auto surfaceNormal{aSegment.surfaceUnitNormal()};
    const CPoint2D correctSurfaceNormal{-0.7071067811865476, 0.7071067811865476};

    EXPECT_TRUE(correctSurfaceNormal == surfaceNormal);
}

TEST_F(TestSegment2D, Segment2DTest4)
{
    CPoint2D aStartPoint{30, 30};
    CPoint2D aEndPoint{10, 10};

    CSegment2D aSegment = CSegment2D(aStartPoint, aEndPoint);

    const double length = aSegment.length();
    EXPECT_NEAR(28.284271, length, 1e-6);

    const auto angle = aSegment.angle();
    EXPECT_NEAR(45, angle, 1e-6);

    const auto surfaceNormal{aSegment.surfaceUnitNormal()};
    const CPoint2D correctSurfaceNormal{-0.7071067811865476, 0.7071067811865476};

    EXPECT_TRUE(correctSurfaceNormal == surfaceNormal);
}