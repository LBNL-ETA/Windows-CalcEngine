#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

class TestVacuumCellArea : public testing::Test
{
protected:
    static constexpr double tolerance = 1e-6;
};

using Tarcog::ISO15099::CellSpacingType;

// Test case for CellSpacingType::Square
TEST_F(TestVacuumCellArea, SquareCellType)
{
    double radius = 0.5; // Example radius
    double expectedArea = radius * radius;
    double calculatedArea = Tarcog::ISO15099::cellArea(CellSpacingType::Square, radius);

    EXPECT_NEAR(expectedArea, calculatedArea, tolerance);
}

// Test case for CellSpacingType::ShiftedSquare
TEST_F(TestVacuumCellArea, ShiftedSquareCellType)
{
    double radius = 0.5; // Example radius
    double expectedArea = FenestrationCommon::nTagonArea(5, radius);
    double calculatedArea = Tarcog::ISO15099::cellArea(CellSpacingType::ShiftedSquare, radius);

    EXPECT_NEAR(expectedArea, calculatedArea, tolerance);
}

// Test case for CellSpacingType::ShiftedRotatedSquare
TEST_F(TestVacuumCellArea, ShiftedRotatedSquareCellType)
{
    double radius = 0.5; // Example radius
    double expectedArea = FenestrationCommon::nTagonArea(5, radius);
    double calculatedArea = Tarcog::ISO15099::cellArea(CellSpacingType::ShiftedRotatedSquare, radius);

    EXPECT_NEAR(expectedArea, calculatedArea, tolerance);
}

// Test case for an invalid CellSpacingType (if applicable)
TEST_F(TestVacuumCellArea, InvalidCellType)
{
    double radius = 0.5; // Example radius
    double calculatedArea = Tarcog::ISO15099::cellArea(static_cast<Tarcog::ISO15099::CellSpacingType>(-1), radius); // Using an invalid enum value

    EXPECT_EQ(0, calculatedArea); // Assuming the function returns 0 for invalid types
}
