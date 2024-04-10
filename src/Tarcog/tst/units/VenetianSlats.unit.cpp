#include <memory>
#include <stdexcept>
#include <iostream>
#include <cmath>

#include <gtest/gtest.h>

#include "WCETarcog.hpp"

class TestVenetianSlats : public testing::Test
{};

// Testing calculateRise function
TEST_F(TestVenetianSlats, RiseZeroCurvature)
{
    EXPECT_NEAR(0.0, ThermalPermeability::Venetian::calculateRise(0, 10), 1e-6);
}

TEST_F(TestVenetianSlats, RiseNegativeCurvature)
{
    // Assuming negative curvature is not expected, so result should be 0. Adjust if your
    // implementation differs.
    EXPECT_NEAR(0.0, ThermalPermeability::Venetian::calculateRise(-10, 10), 1e-6);
}

TEST_F(TestVenetianSlats, RiseStandardCondition)
{
    double expectedRise = 5.0;
    EXPECT_NEAR(expectedRise, ThermalPermeability::Venetian::calculateRise(5, 10), 1e-6);
}

// Testing calculateCurvature function
TEST_F(TestVenetianSlats, CurvatureZeroRise)
{
    EXPECT_NEAR(0.0, ThermalPermeability::Venetian::calculateCurvature(0, 10), 1e-6);
}

TEST_F(TestVenetianSlats, CurvatureNegativeRise)
{
    EXPECT_NEAR(0.0, ThermalPermeability::Venetian::calculateCurvature(-10, 10), 1e-6);
}

TEST_F(TestVenetianSlats, CurvatureStandardCondition)
{
    double expectedCurvature = 6.25;
    EXPECT_NEAR(
      expectedCurvature, ThermalPermeability::Venetian::calculateCurvature(2.5, 10), 1e-6);
}

TEST_F(TestVenetianSlats, FunctionConsistencyCheck)
{
    double slatWidth = 10;
    double rise = 2.5;
    double calculatedCurvature = ThermalPermeability::Venetian::calculateCurvature(rise, slatWidth);
    double calculatedRise =
      ThermalPermeability::Venetian::calculateRise(calculatedCurvature, slatWidth);

    EXPECT_NEAR(rise, calculatedRise, 1e-6);
}
