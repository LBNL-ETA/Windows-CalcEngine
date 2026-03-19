#include <memory>
#include <stdexcept>
#include <iostream>
#include <cmath>

#include <gtest/gtest.h>

#include "WCETarcog.hpp"

class TestVenetianSlats : public testing::Test
{};

TEST_F(TestVenetianSlats, RiseZeroCurvature)
{
    constexpr double curvature = 0;
    constexpr double slatWidth = 10;
    EXPECT_NEAR(0.0, ThermalPermeability::Venetian::calculateRise(curvature, slatWidth), 1e-6);
}

TEST_F(TestVenetianSlats, RiseNegativeCurvature)
{
    constexpr double curvature = -10;
    constexpr double slatWidth = 10;
    EXPECT_NEAR(0.0, ThermalPermeability::Venetian::calculateRise(curvature, slatWidth), 1e-6);
}

TEST_F(TestVenetianSlats, RiseStandardCondition)
{
    constexpr double curvature = 5;
    constexpr double slatWidth = 10;
    EXPECT_NEAR(5.0, ThermalPermeability::Venetian::calculateRise(curvature, slatWidth), 1e-6);
}

TEST_F(TestVenetianSlats, CurvatureZeroRise)
{
    constexpr double rise = 0;
    constexpr double slatWidth = 10;
    EXPECT_NEAR(0.0, ThermalPermeability::Venetian::calculateCurvature(rise, slatWidth), 1e-6);
}

TEST_F(TestVenetianSlats, CurvatureNegativeRise)
{
    constexpr double rise = -10;
    constexpr double slatWidth = 10;
    EXPECT_NEAR(0.0, ThermalPermeability::Venetian::calculateCurvature(rise, slatWidth), 1e-6);
}

TEST_F(TestVenetianSlats, CurvatureStandardCondition)
{
    constexpr double rise = 2.5;
    constexpr double slatWidth = 10;
    EXPECT_NEAR(6.25, ThermalPermeability::Venetian::calculateCurvature(rise, slatWidth), 1e-6);
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
