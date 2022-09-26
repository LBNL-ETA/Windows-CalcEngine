#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "WCEViewer.hpp"


using namespace Viewer;

class TestSegment2DIntersection : public testing::Test
{
protected:
    virtual void SetUp()
    {}
};

TEST_F(TestSegment2DIntersection, Segment2DTest1)
{
    SCOPED_TRACE("Begin Test: Segment 2D - intersection point (1).");

    CPoint2D aStartPoint1{0, 0};
    CPoint2D aEndPoint1{10, 10};

    std::shared_ptr<CSegment2D> aSegment1 = std::make_shared<CSegment2D>(aStartPoint1, aEndPoint1);

    CPoint2D aStartPoint2{1, 0};
    CPoint2D aEndPoint2{10, 10};

    std::shared_ptr<CSegment2D> aSegment2 = std::make_shared<CSegment2D>(aStartPoint2, aEndPoint2);

    bool isInt = aSegment1->intersectionWithSegment(aSegment2);

    EXPECT_FALSE(isInt);
}

TEST_F(TestSegment2DIntersection, Segment2DTest2)
{
    SCOPED_TRACE("Begin Test: Segment 2D - intersection point (2).");

    CPoint2D aStartPoint1{4, 2};
    CPoint2D aEndPoint1{8, 1};

    std::shared_ptr<CSegment2D> aSegment1 = std::make_shared<CSegment2D>(aStartPoint1, aEndPoint1);

    CPoint2D aStartPoint2{1, 3};
    CPoint2D aEndPoint2{5, 7};

    std::shared_ptr<CSegment2D> aSegment2 = std::make_shared<CSegment2D>(aStartPoint2, aEndPoint2);

    bool isInt = aSegment1->intersectionWithSegment(aSegment2);

    EXPECT_FALSE(isInt);
}

TEST_F(TestSegment2DIntersection, Segment2DTest3)
{
    SCOPED_TRACE("Begin Test: Segment 2D - parallel lines (no intersection).");

    CPoint2D aStartPoint1{0, 0};
    CPoint2D aEndPoint1{0, 1};

    std::shared_ptr<CSegment2D> aSegment1 = std::make_shared<CSegment2D>(aStartPoint1, aEndPoint1);

    CPoint2D aStartPoint2{1, 0};
    CPoint2D aEndPoint2{1, 1};

    std::shared_ptr<CSegment2D> aSegment2 = std::make_shared<CSegment2D>(aStartPoint2, aEndPoint2);

    bool isInt = aSegment1->intersectionWithSegment(aSegment2);

    EXPECT_FALSE(isInt);
}

TEST_F(TestSegment2DIntersection, Segment2DTest4)
{
    SCOPED_TRACE("Begin Test: Segment 2D - parallel lines (Total overlap).");

    CPoint2D aStartPoint1{0, 0};
    CPoint2D aEndPoint1{0, 1};

    std::shared_ptr<CSegment2D> aSegment1 = std::make_shared<CSegment2D>(aStartPoint1, aEndPoint1);

    CPoint2D aStartPoint2{0, 0};
    CPoint2D aEndPoint2{0, 2};

    std::shared_ptr<CSegment2D> aSegment2 = std::make_shared<CSegment2D>(aStartPoint2, aEndPoint2);

    bool isInt = aSegment1->intersectionWithSegment(aSegment2);

    EXPECT_FALSE(isInt);
}

TEST_F(TestSegment2DIntersection, Segment2DTest5)
{
    SCOPED_TRACE("Begin Test: Segment 2D - parallel lines (Total overlap - different directions).");

    CPoint2D aStartPoint1{0, 0};
    CPoint2D aEndPoint1{0, 1};

    std::shared_ptr<CSegment2D> aSegment1 = std::make_shared<CSegment2D>(aStartPoint1, aEndPoint1);

    CPoint2D aStartPoint2{0, 2};
    CPoint2D aEndPoint2{0, 0};

    std::shared_ptr<CSegment2D> aSegment2 = std::make_shared<CSegment2D>(aStartPoint2, aEndPoint2);

    bool isInt = aSegment1->intersectionWithSegment(aSegment2);

    EXPECT_FALSE(isInt);
}

TEST_F(TestSegment2DIntersection, Segment2DTest6)
{
    SCOPED_TRACE("Begin Test: Segment 2D - parallel lines (Total overlap - same directions, "
                 "different lengths).");

    CPoint2D aStartPoint1{0, 10};
    CPoint2D aEndPoint1{10, 0};

    std::shared_ptr<CSegment2D> aSegment1 = std::make_shared<CSegment2D>(aStartPoint1, aEndPoint1);

    CPoint2D aStartPoint2{0, 10};
    CPoint2D aEndPoint2{5, 5};

    std::shared_ptr<CSegment2D> aSegment2 = std::make_shared<CSegment2D>(aStartPoint2, aEndPoint2);

    bool isInt = aSegment1->intersectionWithSegment(aSegment2);

    EXPECT_FALSE(isInt);
}
