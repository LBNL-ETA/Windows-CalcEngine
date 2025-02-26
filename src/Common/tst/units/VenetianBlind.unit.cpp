#include <memory>
#include <gtest/gtest.h>

#include <WCECommon.hpp>

TEST(TestVenetianGeometry, ThicknessOfFlatSlat)
{
    FenestrationCommon::Venetian::Geometry aGeometry;
    aGeometry.SlatWidth = 0.1;
    aGeometry.SlatTiltAngle = 0;

    EXPECT_NEAR(0.1, FenestrationCommon::Venetian::thickness(aGeometry), 1e-6);
}

TEST(TestVenetianGeometry, ThicknessOfTiltedSlat)
{
    FenestrationCommon::Venetian::Geometry aGeometry;
    aGeometry.SlatWidth = 0.1;
    aGeometry.SlatTiltAngle = 45;

    EXPECT_NEAR(0.070711, FenestrationCommon::Venetian::thickness(aGeometry), 1e-6);
}

TEST(TestVenetianGeometry, ThicknessOfTiltedSlat2)
{
    FenestrationCommon::Venetian::Geometry aGeometry;
    aGeometry.SlatWidth = 0.1;
    aGeometry.SlatTiltAngle = 60;

    EXPECT_NEAR(0.05, FenestrationCommon::Venetian::thickness(aGeometry), 1e-6);
}