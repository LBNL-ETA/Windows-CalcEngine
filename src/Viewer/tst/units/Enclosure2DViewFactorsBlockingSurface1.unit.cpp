 #include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "WCEViewer.hpp"
#include "WCECommon.hpp"


using namespace Viewer;
using namespace FenestrationCommon;

class TestEnclosure2DViewFactorsBlockingSurface1 : public testing::Test
{
private:
    std::shared_ptr<CGeometry2D> m_Enclosure2D;

protected:
    virtual void SetUp()
    {
        m_Enclosure2D = std::make_shared<CGeometry2D>();

        // Segment 1
        CPoint2D aStartPoint1{10.0, 0.0};
        CPoint2D aEndPoint1{0.0, 0.0};

        CViewSegment2D aSegment1(aStartPoint1, aEndPoint1);
        m_Enclosure2D->appendSegment(aSegment1);

        // Segment 2
        CPoint2D aStartPoint2{0.0, 5.0};
        CPoint2D aEndPoint2{10.0, 5.0};

        CViewSegment2D aSegment2(aStartPoint2, aEndPoint2);
        m_Enclosure2D->appendSegment(aSegment2);

        // Segment 3
        CPoint2D aStartPoint3{5.0, 2.0};
        CPoint2D aEndPoint3{0.0, 2.0};

        CViewSegment2D aSegment3(aStartPoint3, aEndPoint3);
        m_Enclosure2D->appendSegment(aSegment3);
    }

public:
    std::shared_ptr<CGeometry2D> getEnclosure()
    {
        return m_Enclosure2D;
    };
};

TEST_F(TestEnclosure2DViewFactorsBlockingSurface1, Enclosure2DViewFactors)
{
    SCOPED_TRACE("Begin Test: 2D Enclosure - View Factors (blocking surface).");

    std::shared_ptr<CGeometry2D> aEnclosure = getEnclosure();

    SquareMatrix viewFactors{aEnclosure->viewFactors()};

    EXPECT_NEAR(0.000000000, viewFactors(0, 0), 1e-6);
    EXPECT_NEAR(0.309016994, viewFactors(0, 1), 1e-6);
    EXPECT_NEAR(0.000000000, viewFactors(0, 2), 1e-6);

    EXPECT_NEAR(0.309016994, viewFactors(1, 0), 1e-6);
    EXPECT_NEAR(0.000000000, viewFactors(1, 1), 1e-6);
    EXPECT_NEAR(0.372015325, viewFactors(1, 2), 1e-6);

    EXPECT_NEAR(0.000000000, viewFactors(2, 0), 1e-6);
    EXPECT_NEAR(0.744030651, viewFactors(2, 1), 1e-6);
    EXPECT_NEAR(0.000000000, viewFactors(2, 2), 1e-6);
}
