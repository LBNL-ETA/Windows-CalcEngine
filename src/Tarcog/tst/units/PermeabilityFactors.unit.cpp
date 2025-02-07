#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

class TestPermeabilityFactors : public testing::Test
{
protected:
    void SetUp() override
    {}
};

TEST_F(TestPermeabilityFactors, TestVenetianPermeability)
{
    SCOPED_TRACE("Begin Test: Venetian layer thermal permeability.");

    const auto matThickness{0.0001};   // m

    const FenestrationCommon::Venetian::Geometry geometry{0.0148, 0.0127, 0.0, 0.03313057};

    const auto permeabilityOpenness{
      ThermalPermeability::Venetian::permeabilityFactor(matThickness, geometry)};

    EXPECT_NEAR(0.9921875, permeabilityOpenness, 1e-6);
}

TEST_F(TestPermeabilityFactors, TestVenetianPermeability1)
{
    SCOPED_TRACE("Begin Test: Venetian layer thermal permeability.");

    const auto matThickness{0.0029};   // m

    const FenestrationCommon::Venetian::Geometry geometry{0.0508, 0.0432, 0.0, 0.0};

    const auto permeabilityOpenness{
      ThermalPermeability::Venetian::permeabilityFactor(matThickness, geometry)};

    EXPECT_NEAR(0.937093, permeabilityOpenness, 1e-6);
}

TEST_F(TestPermeabilityFactors, TestPerforatedCircularPermeability)
{
    SCOPED_TRACE("Begin Test: Circular perforated layer thermal permeability.");

    const FenestrationCommon::Perforated::Geometry geometry{
      FenestrationCommon::Perforated::Type::Circular, 0.01905, 0.01905, 0.00635, 0.00635};

    const auto permeabilityOpenness{ThermalPermeability::Perforated::permeabilityFactor(geometry)};

    EXPECT_NEAR(0.087266, permeabilityOpenness, 1e-6);
}

TEST_F(TestPermeabilityFactors, TestPerforatedRectangularPermeability)
{
    SCOPED_TRACE("Begin Test: Rectangular perforated layer thermal permeability.");

    const FenestrationCommon::Perforated::Geometry geometry{
      FenestrationCommon::Perforated::Type::Rectangular, 0.01905, 0.01905, 0.00635, 0.00635};

    const auto permeabilityOpenness{ThermalPermeability::Perforated::permeabilityFactor(geometry)};

    EXPECT_NEAR(0.111111, permeabilityOpenness, 1e-6);
}

TEST_F(TestPermeabilityFactors, TestPerforatedSquarePermeability)
{
    SCOPED_TRACE("Begin Test: Square perforated layer thermal permeability.");

    const FenestrationCommon::Perforated::Geometry geometry{
      FenestrationCommon::Perforated::Type::Square, 0.01905, 0.01905, 0.00635, 0.00635};

    const auto permeabilityOpenness{ThermalPermeability::Perforated::permeabilityFactor(geometry)};

    EXPECT_NEAR(0.111111, permeabilityOpenness, 1e-6);
}

TEST_F(TestPermeabilityFactors, TestWovenPermeability)
{
    SCOPED_TRACE("Begin Test: Woven layer thermal permeability.");

    const FenestrationCommon::Woven::Geometry geometry{0.001, 0.002};

    const auto permeabilityOpenness{ThermalPermeability::Woven::permeabilityFactor(geometry)};

    EXPECT_NEAR(0.25, permeabilityOpenness, 1e-6);
}

TEST_F(TestPermeabilityFactors, TestLouveredShutterPermeability)
{
    SCOPED_TRACE("Begin Test: Louvered shutter thermal permeability.");

    constexpr FenestrationCommon::LouveredShutter::Geometry geometry{0.0889, 0.01, 87, 0.0762};

    const auto permeabilityOpenness{
      ThermalPermeability::LouveredShutter::permeabilityFactor(geometry)};

    EXPECT_NEAR(0.295494779044715, permeabilityOpenness, 1e-6);
}