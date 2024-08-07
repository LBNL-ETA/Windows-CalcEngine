#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include <WCEViewer.hpp>

#include "WCESingleLayerOptics.hpp"

using namespace SingleLayerOptics;

class TestBeamDirection : public testing::Test
{
protected:
    virtual void SetUp()
    {}
};

TEST_F(TestBeamDirection, TestBeamDirectionProfileAngle1)
{
    SCOPED_TRACE("Begin Test: Beam direction profile angles.");

    CBeamDirection aDirection(0, 0);

    double profileAngle = aDirection.profileAngle();
    EXPECT_NEAR(0, profileAngle, 1e-6);

    const auto unitVector = aDirection.unitVector();
    const auto correctUnitVector = Viewer::CSegment2D({0, 0}, {1, 0});
    EXPECT_TRUE(correctUnitVector == unitVector);
}

TEST_F(TestBeamDirection, TestBeamDirectionProfileAngle2)
{
    SCOPED_TRACE("Begin Test: Beam direction profile angles.");

    CBeamDirection aDirection(18, 90);

    double profileAngle = aDirection.profileAngle();
    EXPECT_NEAR(-18, profileAngle, 1e-6);

    const auto unitVector{aDirection.unitVector()};
    const auto correctUnitVector{
      Viewer::CSegment2D({0, 0}, {0.95105651629515353, 0.3090169943749474})};
    EXPECT_TRUE(correctUnitVector == unitVector);
}

TEST_F(TestBeamDirection, TestBeamDirectionProfileAngle3)
{
    SCOPED_TRACE("Begin Test: Beam direction profile angles.");

    CBeamDirection aDirection(18, 270);

    double profileAngle = aDirection.profileAngle();
    EXPECT_NEAR(18, profileAngle, 1e-6);

    const auto unitVector{aDirection.unitVector()};
    const Viewer::CSegment2D correctUnitVector{{0, 0}, {0.95105651629515353, -0.3090169943749474}};
    EXPECT_TRUE(correctUnitVector == unitVector);
}

TEST_F(TestBeamDirection, TestBeamDirectionAssignment)
{
    SCOPED_TRACE("Begin Test: Copying beam direction.");

    CBeamDirection aDirection(18, 90);
    CBeamDirection aCopyDirection(0, 0);

    aCopyDirection = aDirection;

    EXPECT_NEAR(18, aCopyDirection.theta(), 1e-6);
    EXPECT_NEAR(90, aCopyDirection.phi(), 1e-6);
}

TEST_F(TestBeamDirection, TestBeamDirectionRotation)
{
    SCOPED_TRACE("Begin Test: Rotation of beam direction.");

    CBeamDirection direction1{18, 90};
    const double rotationAngle{45};
    CBeamDirection direction{direction1.rotate(rotationAngle)};

    EXPECT_NEAR(135, direction.phi(), 1e-6);
}
