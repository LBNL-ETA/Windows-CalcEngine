#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "WCESingleLayerOptics.hpp"


using namespace SingleLayerOptics;

class TestBSDFDirectionsClosestIndex : public testing::Test
{
private:
    BSDFHemisphere m_BSDFHemisphere{BSDFHemisphere::create(BSDFBasis::Quarter)};

public:
    const BSDFDirections & GetDirections(const BSDFDirection t_Side) const
    {
        return m_BSDFHemisphere.getDirections(t_Side);
    };
};

TEST_F(TestBSDFDirectionsClosestIndex, TestClosestIndex1)
{
    SCOPED_TRACE("Begin Test: Find closest index 1.");

    const auto & aDirections = GetDirections(BSDFDirection::Incoming);

    double theta = 15;
    double phi = 270;

    const size_t beamIndex = aDirections.getNearestBeamIndex(theta, phi);

    EXPECT_EQ(7, int(beamIndex));
}

TEST_F(TestBSDFDirectionsClosestIndex, TestClosestIndex2)
{
    SCOPED_TRACE("Begin Test: Find closest index 2.");

    const auto & aDirections = GetDirections(BSDFDirection::Incoming);

    double theta = 70;
    double phi = 175;

    size_t beamIndex = aDirections.getNearestBeamIndex(theta, phi);

    EXPECT_EQ(37, int(beamIndex));
}

TEST_F(TestBSDFDirectionsClosestIndex, TestClosestIndex3)
{
    SCOPED_TRACE("Begin Test: Find closest index 3.");

    const auto & aDirections = GetDirections(BSDFDirection::Incoming);

    double theta = 55;
    double phi = 60;

    size_t beamIndex = aDirections.getNearestBeamIndex(theta, phi);

    EXPECT_EQ(23, int(beamIndex));
}

TEST_F(TestBSDFDirectionsClosestIndex, TestClosestIndex4)
{
    SCOPED_TRACE("Begin Test: Find closest index 4.");

    const auto & aDirections = GetDirections(BSDFDirection::Incoming);

    double theta = 0;
    double phi = 0;

    size_t beamIndex = aDirections.getNearestBeamIndex(theta, phi);

    EXPECT_EQ(0, int(beamIndex));
}

TEST_F(TestBSDFDirectionsClosestIndex, TestClosestIndex5)
{
    SCOPED_TRACE("Begin Test: Find closest index 5.");

    const auto & aDirections = GetDirections(BSDFDirection::Incoming);

    double theta = 71.2163;
    double phi = 349.744251;

    size_t beamIndex = aDirections.getNearestBeamIndex(theta, phi);

    EXPECT_EQ(33, int(beamIndex));
}
