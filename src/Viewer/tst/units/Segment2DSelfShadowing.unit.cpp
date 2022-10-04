#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "WCEViewer.hpp"


using namespace Viewer;

class TestSegment2DSelfShadowing : public testing::Test
{
protected:
    void SetUp() override
    {}
};

TEST_F(TestSegment2DSelfShadowing, Segment2DNoShadowing)
{
    SCOPED_TRACE("Begin Test: Segments self shadowing - No shadowing case.");

    CPoint2D aStartPoint1{10.0, 0.0};
    CPoint2D aEndPoint1{0.0, 0.0};

    auto aSegment1 = CViewSegment2D(aStartPoint1, aEndPoint1);

    CPoint2D aStartPoint2{0, 1};
    CPoint2D aEndPoint2{10, 1};

    auto aSegment2 = CViewSegment2D(aStartPoint2, aEndPoint2);

    auto aShadowing = aSegment1.selfShadowing(aSegment2);

    EXPECT_EQ(Shadowing::No, aShadowing);
}

TEST_F(TestSegment2DSelfShadowing, Segment2DTotalShadowing)
{
    SCOPED_TRACE("Begin Test: Segments self shadowing - Total shadowing case.");

    CPoint2D aStartPoint1{10.0, 0.0};
    CPoint2D aEndPoint1{0.0, 0.0};

    auto aSegment1 = CViewSegment2D(aStartPoint1, aEndPoint1);

    CPoint2D aStartPoint2{10.0, 1.0};
    CPoint2D aEndPoint2{0.0, 1.0};

    auto aSegment2 = CViewSegment2D(aStartPoint2, aEndPoint2);

    auto aShadowing = aSegment1.selfShadowing(aSegment2);

    EXPECT_EQ(Shadowing::Total, aShadowing);
}

TEST_F(TestSegment2DSelfShadowing, Segment2DNoShadowingSamePoint1)
{
    SCOPED_TRACE(
      "Begin Test: Segments self shadowing - No shadowing case (share same point angle < 180).");

    CPoint2D aStartPoint1{10.0, 0.0};
    CPoint2D aEndPoint1{0.0, 0.0};

    auto aSegment1 = CViewSegment2D(aStartPoint1, aEndPoint1);

    CPoint2D aStartPoint2{0.0, 1.0};
    CPoint2D aEndPoint2{10.0, 0.0};

    auto aSegment2 = CViewSegment2D(aStartPoint2, aEndPoint2);

    auto aShadowing = aSegment1.selfShadowing(aSegment2);

    EXPECT_EQ(Shadowing::No, aShadowing);
}

TEST_F(TestSegment2DSelfShadowing, Segment2DNoShadowingSamePoint2)
{
    SCOPED_TRACE(
      "Begin Test: Segments self shadowing - No shadowing case (share same point, angle > 180).");

    CPoint2D aStartPoint1{10.0, 0.0};
    CPoint2D aEndPoint1{0.0, 0.0};

    auto aSegment1 = CViewSegment2D(aStartPoint1, aEndPoint1);

    CPoint2D aStartPoint2{0.0, 0.0};
    CPoint2D aEndPoint2{0.0, -2.0};

    auto aSegment2 = CViewSegment2D(aStartPoint2, aEndPoint2);

    auto aShadowing = aSegment1.selfShadowing(aSegment2);

    EXPECT_EQ(Shadowing::Total, aShadowing);
}

TEST_F(TestSegment2DSelfShadowing, Segment2DPartialShadowingThis)
{
    SCOPED_TRACE(
      "Begin Test: Segments self shadowing - Partial shadowing case (view blocked by itself).");

    CPoint2D aStartPoint1{10.0, 0.0};
    CPoint2D aEndPoint1{0.0, 0.0};

    auto aSegment1 = CViewSegment2D(aStartPoint1, aEndPoint1);

    CPoint2D aStartPoint2{-5.0, -1.0};
    CPoint2D aEndPoint2{-5.0, 1.0};

    auto aSegment2 = CViewSegment2D(aStartPoint2, aEndPoint2);

    auto aShadowing = aSegment1.selfShadowing(aSegment2);

    EXPECT_EQ(Shadowing::Partial, aShadowing);
}

TEST_F(TestSegment2DSelfShadowing, Segment2DPartialShadowingOther)
{
    SCOPED_TRACE("Begin Test: Segments self shadowing - Partial shadowing case (view blocked by "
                 "viewed surface).");

    CPoint2D aStartPoint1{10.0, 0.0};
    CPoint2D aEndPoint1{0.0, 0.0};

    auto aSegment1 = CViewSegment2D(aStartPoint1, aEndPoint1);

    CPoint2D aStartPoint2{5.0, 5.0};
    CPoint2D aEndPoint2{5.0, 10.0};

    auto aSegment2 = CViewSegment2D(aStartPoint2, aEndPoint2);

    auto aShadowing = aSegment1.selfShadowing(aSegment2);

    EXPECT_EQ(Shadowing::Partial, aShadowing);
}
