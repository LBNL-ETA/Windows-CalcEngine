#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "WCEViewer.hpp"
#include "WCECommon.hpp"


using namespace Viewer;
using namespace FenestrationCommon;

class TestEnclosure2DViewFactors3 : public testing::Test
{
private:
    std::shared_ptr<CGeometry2D> m_Enclosure2D;

protected:
    virtual void SetUp()
    {
        m_Enclosure2D = std::make_shared<CGeometry2D>();

        // Segment 1
        CPoint2D aStartPoint1{0, 0};
        CPoint2D aEndPoint1{0, 10};

        CViewSegment2D aSegment1(aStartPoint1, aEndPoint1);
        m_Enclosure2D->appendSegment(aSegment1);

        // Segment 2
        CPoint2D aStartPoint2{0, 10};
        CPoint2D aEndPoint2{10, 10};

        CViewSegment2D aSegment2(aStartPoint2, aEndPoint2);
        m_Enclosure2D->appendSegment(aSegment2);

        // Segment 3
        CPoint2D aStartPoint3{10, 10};
        CPoint2D aEndPoint3{20, 10};

        CViewSegment2D aSegment3(aStartPoint3, aEndPoint3);
        m_Enclosure2D->appendSegment(aSegment3);

        // Segment 4
        CPoint2D aStartPoint4{20, 10};
        CPoint2D aEndPoint4{0, 0};

        CViewSegment2D aSegment4(aStartPoint4, aEndPoint4);
        m_Enclosure2D->appendSegment(aSegment4);
    }

public:
    std::shared_ptr<CGeometry2D> getEnclosure()
    {
        return m_Enclosure2D;
    };
};

TEST_F(TestEnclosure2DViewFactors3, Enclosure2DViewFactors)
{
    SCOPED_TRACE("Begin Test: 2D Enclosure - View Factors (no blocking, two surfaces collinear).");

    std::shared_ptr<CGeometry2D> aEnclosure = getEnclosure();

    SquareMatrix viewFactors{aEnclosure->viewFactors()};

    EXPECT_NEAR(0.000000000, viewFactors(0, 0), 1e-6);
    EXPECT_NEAR(0.292893219, viewFactors(0, 1), 1e-6);
    EXPECT_NEAR(0.089072792, viewFactors(0, 2), 1e-6);
    EXPECT_NEAR(0.618033989, viewFactors(0, 3), 1e-6);

    EXPECT_NEAR(0.292893219, viewFactors(1, 0), 1e-6);
    EXPECT_NEAR(0.000000000, viewFactors(1, 1), 1e-6);
    EXPECT_NEAR(0.000000000, viewFactors(1, 2), 1e-6);
    EXPECT_NEAR(0.707106781, viewFactors(1, 3), 1e-6);

    EXPECT_NEAR(0.089072792, viewFactors(2, 0), 1e-6);
    EXPECT_NEAR(0.000000000, viewFactors(2, 1), 1e-6);
    EXPECT_NEAR(0.000000000, viewFactors(2, 2), 1e-6);
    EXPECT_NEAR(0.910927208, viewFactors(2, 3), 1e-6);

    EXPECT_NEAR(0.276393202, viewFactors(3, 0), 1e-6);
    EXPECT_NEAR(0.316227766, viewFactors(3, 1), 1e-6);
    EXPECT_NEAR(0.407379032, viewFactors(3, 2), 1e-6);
    EXPECT_NEAR(0.000000000, viewFactors(3, 3), 1e-6);
}
