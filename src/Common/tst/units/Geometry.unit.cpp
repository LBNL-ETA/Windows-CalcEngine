#include <memory>
#include <gtest/gtest.h>

#include "WCECommon.hpp"

class TestGeometry : public testing::Test
{
protected:
    void SetUp() override
    {}
};

TEST_F(TestGeometry, Square)
{
    SCOPED_TRACE("Begin Test: Geometry Square.");

    const FenestrationCommon::GeometryType geometry{FenestrationCommon::GeometryType::Square};
    const double length{1};
    const double height{0.2};

    const auto area{FenestrationCommon::area(length, height, geometry)};

    EXPECT_NEAR(0.2, area, 1e-6);
}

TEST_F(TestGeometry, Trapezoid)
{
    SCOPED_TRACE("Begin Test: Geometry Square.");

    const FenestrationCommon::GeometryType geometry{FenestrationCommon::GeometryType::Trapezoid};
    const double length{1};
    const double height{0.2};

    const auto area{FenestrationCommon::area(length, height, geometry)};

    EXPECT_NEAR(0.16, area, 1e-6);
}

TEST_F(TestGeometry, HalfTrapezoid)
{
    SCOPED_TRACE("Begin Test: Geometry Square.");

    const FenestrationCommon::GeometryType geometry{FenestrationCommon::GeometryType::HalfTrapezoid};
    const double length{1};
    const double height{0.2};

    const auto area{FenestrationCommon::area(length, height, geometry)};

    EXPECT_NEAR(0.18, area, 1e-6);
}