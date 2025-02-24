#include <memory>
#include <gtest/gtest.h>

#include <WCECommon.hpp>

TEST(TestVenetianGeometry, ThicknessOfFlatSlat)
{
    FenestrationCommon::Venetian::Geometry aGeometry;
    aGeometry.SlatWidth = 0.1;
    aGeometry.SlatTiltAngle = 0;

    auto thickness = FenestrationCommon::Venetian::thickness(aGeometry);

    EXPECT_NEAR(0.1, thickness, 1e-6);
}