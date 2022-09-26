#include <memory>
#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "WCEViewer.hpp"


using namespace Viewer;

class TestSegment2DSubsegments : public testing::Test
{
protected:
    virtual void SetUp()
    {}
};

TEST_F(TestSegment2DSubsegments, Segment2DTest1)
{
    SCOPED_TRACE("Begin Test: Segment 2D - subsegments creation.");

    CPoint2D aStartPoint{0.0, 0.0};
    CPoint2D aEndPoint{10.0, 10.0};

    CViewSegment2D aSegment = CViewSegment2D(aStartPoint, aEndPoint);

    std::shared_ptr<std::vector<std::shared_ptr<CViewSegment2D>>> aSubSegments =
      aSegment.subSegments(4);

    std::vector<double> correctStartX = {0, 2.5, 5, 7.5};
    std::vector<double> correctEndX = {2.5, 5, 7.5, 10};

    std::vector<double> correctStartY = {0, 2.5, 5, 7.5};
    std::vector<double> correctEndY = {2.5, 5, 7.5, 10};

    size_t i = 0;
    for(std::shared_ptr<CViewSegment2D> aSubSegment : *aSubSegments)
    {
        double xStart = aSubSegment->startPoint().x();
        double xEnd = aSubSegment->endPoint().x();
        double yStart = aSubSegment->startPoint().y();
        double yEnd = aSubSegment->endPoint().y();

        EXPECT_NEAR(correctStartX[i], xStart, 1e-6);
        EXPECT_NEAR(correctEndX[i], xEnd, 1e-6);
        EXPECT_NEAR(correctStartY[i], yStart, 1e-6);
        EXPECT_NEAR(correctEndY[i], yEnd, 1e-6);

        ++i;
    }
}
