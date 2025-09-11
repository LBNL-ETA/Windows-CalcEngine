#include <memory>
#include <gtest/gtest.h>

#include "WCESingleLayerOptics.hpp"

class TestBeamDirections : public testing::Test
{
protected:
    virtual void SetUp()
    {}
};

TEST_F(TestBeamDirections, TestBeamDirectionProfileAngle1)
{
    using namespace SingleLayerOptics;

    const auto hemisphere{BSDFHemisphere::create(BSDFBasis::Small)};
    const std::vector<double> profileAngles{hemisphere.profileAngles(BSDFDirection::Incoming)};
    const std::vector<double> correctProfileAngles{0, 0, 0, 0, 0, 0, 0};

    EXPECT_EQ(7u, profileAngles.size());
    for(size_t i = 0u; i < correctProfileAngles.size(); ++i)
    {
        EXPECT_NEAR(correctProfileAngles[i], profileAngles[i], 1e-6);
    }
}
