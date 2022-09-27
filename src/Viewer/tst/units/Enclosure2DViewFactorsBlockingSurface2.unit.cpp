#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "WCEViewer.hpp"
#include "WCECommon.hpp"


using namespace Viewer;
using namespace FenestrationCommon;

class TestEnclosure2DViewFactorsBlockingSurface2 : public testing::Test
{
private:
    std::shared_ptr<CGeometry2D> m_Enclosure2D;

protected:
    virtual void SetUp()
    {
        m_Enclosure2D = std::make_shared<CGeometry2D>();

        // Segment 1
        CPoint2D aStartPoint1{0.0, 0.0};
        CPoint2D aEndPoint1{0.0, 10.0};

        CViewSegment2D aSegment1{aStartPoint1, aEndPoint1};
        m_Enclosure2D->appendSegment(aSegment1);

        // Segment 2
        CPoint2D aStartPoint2{10.0, 10.0};
        CPoint2D aEndPoint2{10.0, 0.0};

        CViewSegment2D aSegment2{aStartPoint2, aEndPoint2};
        m_Enclosure2D->appendSegment(aSegment2);

        // Segment 3
        CPoint2D aStartPoint3{10.0, 0.0};
        CPoint2D aEndPoint3{9.0, 1.0};

        CViewSegment2D aSegment3{aStartPoint3, aEndPoint3};
        m_Enclosure2D->appendSegment(aSegment3);
    }

public:
    std::shared_ptr<CGeometry2D> getEnclosure()
    {
        return m_Enclosure2D;
    };
};

TEST_F(TestEnclosure2DViewFactorsBlockingSurface2, Enclosure2DViewFactors)
{
    SCOPED_TRACE("Begin Test: 2D Enclosure - View Factors (blocking surface sharp angle).");

    std::shared_ptr<CGeometry2D> aEnclosure = getEnclosure();

    SquareMatrix viewFactors{aEnclosure->viewFactors()};

    EXPECT_NEAR(0.000000000, viewFactors(0, 0), 1e-6);
    EXPECT_NEAR(0.391219268, viewFactors(0, 1), 1e-6);
    EXPECT_NEAR(0.000000000, viewFactors(0, 2), 1e-6);

    EXPECT_NEAR(0.391219268, viewFactors(1, 0), 1e-6);
    EXPECT_NEAR(0.000000000, viewFactors(1, 1), 1e-6);
    EXPECT_NEAR(0.117941421, viewFactors(1, 2), 1e-6);

    EXPECT_NEAR(0.000000000, viewFactors(2, 0), 1e-6);
    EXPECT_NEAR(0.833971787, viewFactors(2, 1), 1e-6);
    EXPECT_NEAR(0.000000000, viewFactors(2, 2), 1e-6);
}
