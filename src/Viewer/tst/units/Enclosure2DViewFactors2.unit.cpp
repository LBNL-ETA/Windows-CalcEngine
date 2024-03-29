#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "WCEViewer.hpp"
#include "WCECommon.hpp"


using namespace Viewer;
using namespace FenestrationCommon;

class TestEnclosure2DViewFactors2 : public testing::Test
{
private:
    std::shared_ptr<CGeometry2D> m_Enclosure2D;

protected:
    void SetUp() override
    {
        m_Enclosure2D = std::make_shared<CGeometry2D>();

        // Segment 1
        CPoint2D aStartPoint1{0.0, 0.0};
        CPoint2D aEndPoint1{0.5, 3.0};

        CViewSegment2D aSegment1{aStartPoint1, aEndPoint1};
        m_Enclosure2D->appendSegment(aSegment1);

        // Segment 2
        CPoint2D aStartPoint2{0.5, 3.0};
        CPoint2D aEndPoint2{1.5, 2.0};

        CViewSegment2D aSegment2{aStartPoint2, aEndPoint2};
        m_Enclosure2D->appendSegment(aSegment2);

        // Segment 3
        CPoint2D aStartPoint3{1.5, 2.0};
        CPoint2D aEndPoint3{2.0, 0.5};

        CViewSegment2D aSegment3{aStartPoint3, aEndPoint3};
        m_Enclosure2D->appendSegment(aSegment3);

        // Segment 4
        CPoint2D aStartPoint4{2.0, 0.5};
        CPoint2D aEndPoint4{0.0, 0.0};

        CViewSegment2D aSegment4{aStartPoint4, aEndPoint4};
        m_Enclosure2D->appendSegment(aSegment4);
    }

public:
    std::shared_ptr<CGeometry2D> getEnclosure() const
    {
        return m_Enclosure2D;
    };
};

TEST_F(TestEnclosure2DViewFactors2, Enclosure2DViewFactors)
{
    SCOPED_TRACE("Begin Test: 2D Enclosure - View Factors (no blocking).");

    std::shared_ptr<CGeometry2D> aEnclosure = getEnclosure();

    SquareMatrix viewFactors{aEnclosure->viewFactors()};

    EXPECT_NEAR(0.000000000, viewFactors(0, 0), 1e-6);
    EXPECT_NEAR(0.321497809, viewFactors(0, 1), 1e-6);
    EXPECT_NEAR(0.318886289, viewFactors(0, 2), 1e-6);
    EXPECT_NEAR(0.359615901, viewFactors(0, 3), 1e-6);

    EXPECT_NEAR(0.691407182, viewFactors(1, 0), 1e-6);
    EXPECT_NEAR(0.000000000, viewFactors(1, 1), 1e-6);
    EXPECT_NEAR(0.028240588, viewFactors(1, 2), 1e-6);
    EXPECT_NEAR(0.280352230, viewFactors(1, 3), 1e-6);

    EXPECT_NEAR(0.613390025, viewFactors(2, 0), 1e-6);
    EXPECT_NEAR(0.025259150, viewFactors(2, 1), 1e-6);
    EXPECT_NEAR(0.000000000, viewFactors(2, 2), 1e-6);
    EXPECT_NEAR(0.361350825, viewFactors(2, 3), 1e-6);

    EXPECT_NEAR(0.530536525, viewFactors(3, 0), 1e-6);
    EXPECT_NEAR(0.192320043, viewFactors(3, 1), 1e-6);
    EXPECT_NEAR(0.277143432, viewFactors(3, 2), 1e-6);
    EXPECT_NEAR(0.000000000, viewFactors(3, 3), 1e-6);
}
