#include <memory>
#include <gtest/gtest.h>

#include "WCEGases.hpp"

using namespace Gases;

class TestGasPropertiesSingleGas : public testing::Test
{
private:
    CGas Gas;   // Default gas is 100% air

public:
    CGas & GetGas()
    {
        return Gas;
    };
};

TEST_F(TestGasPropertiesSingleGas, TestSimpleProperties)
{
    SCOPED_TRACE("Begin Test: Gas Properties (Air) simple properties - Temperature = 300 [K], "
                 "Pressure = 101325 [Pa]");

    auto aGas = GetGas();

    aGas.setTemperatureAndPressure(300, 101325);
    auto aProperties = aGas.getSimpleGasProperties();

    EXPECT_NEAR(28.97, aProperties.m_MolecularWeight, 0.0001);
    EXPECT_NEAR(2.61533000E-02, aProperties.m_ThermalConductivity, 1e-6);
    EXPECT_NEAR(1.85433000E-05, aProperties.m_Viscosity, 1e-6);
    EXPECT_NEAR(1006.4342, aProperties.m_SpecificHeat, 0.0001);
    EXPECT_NEAR(1.176819053, aProperties.m_Density, 0.0001);
    EXPECT_NEAR(0.713585333, aProperties.m_PrandlNumber, 0.0001);
}

TEST_F(TestGasPropertiesSingleGas, TestSimplePropertiesRepeat)
{
    SCOPED_TRACE("Begin Test: Gas Properties (Air) simple properties - Temperature = 300 [K], "
                 "Pressure = 101325 [Pa] (Repeatability)");

    auto aGas = GetGas();

    aGas.setTemperatureAndPressure(300, 101325);
    auto aProperties = aGas.getSimpleGasProperties();

    EXPECT_NEAR(28.97, aProperties.m_MolecularWeight, 0.0001);
    EXPECT_NEAR(2.61533000E-02, aProperties.m_ThermalConductivity, 1e-6);
    EXPECT_NEAR(1.85433000E-05, aProperties.m_Viscosity, 1e-6);
    EXPECT_NEAR(1006.4342, aProperties.m_SpecificHeat, 0.0001);
    EXPECT_NEAR(1.176819053, aProperties.m_Density, 0.0001);
    EXPECT_NEAR(0.713585333, aProperties.m_PrandlNumber, 0.0001);
}

TEST_F(TestGasPropertiesSingleGas, TestRealProperties)
{
    SCOPED_TRACE("Begin Test: Gas Properties (Air) real properties - Temperature = 300 [K], "
                 "Pressure = 101325 [Pa]");

    auto aGas = GetGas();

    aGas.setTemperatureAndPressure(300, 101325);
    auto aProperties = aGas.getGasProperties();

    EXPECT_NEAR(28.97, aProperties.m_MolecularWeight, 0.0001);
    EXPECT_NEAR(2.61533000E-02, aProperties.m_ThermalConductivity, 1e-6);
    EXPECT_NEAR(1.85433000E-05, aProperties.m_Viscosity, 1e-6);
    EXPECT_NEAR(1006.4342, aProperties.m_SpecificHeat, 0.0001);
    EXPECT_NEAR(1.176819053, aProperties.m_Density, 0.0001);
    EXPECT_NEAR(0.713585333, aProperties.m_PrandlNumber, 0.0001);
}

TEST_F(TestGasPropertiesSingleGas, TestRealPropertiesRepeat)
{
    SCOPED_TRACE("Begin Test: Gas Properties (Air) real properties - Temperature = 300 [K], "
                 "Pressure = 101325 [Pa] (Repeatability)");

    auto aGas = GetGas();

    aGas.setTemperatureAndPressure(300, 101325);
    auto aProperties = aGas.getGasProperties();

    EXPECT_NEAR(28.97, aProperties.m_MolecularWeight, 0.0001);
    EXPECT_NEAR(2.61533000E-02, aProperties.m_ThermalConductivity, 1e-6);
    EXPECT_NEAR(1.85433000E-05, aProperties.m_Viscosity, 1e-6);
    EXPECT_NEAR(1006.4342, aProperties.m_SpecificHeat, 0.0001);
    EXPECT_NEAR(1.176819053, aProperties.m_Density, 0.0001);
    EXPECT_NEAR(0.713585333, aProperties.m_PrandlNumber, 0.0001);
}

TEST_F(TestGasPropertiesSingleGas, TestRealPropertiesLowPressure)
{
    SCOPED_TRACE("Begin Test: Gas Properties (Air) real properties - Temperature = 300 [K], "
                 "Pressure = 90,000 [Pa]");

    auto aGas = GetGas();

    aGas.setTemperatureAndPressure(300, 90000);
    auto aProperties = aGas.getGasProperties();

    EXPECT_NEAR(28.97, aProperties.m_MolecularWeight, 0.0001);
    EXPECT_NEAR(2.61533000E-02, aProperties.m_ThermalConductivity, 1e-6);
    EXPECT_NEAR(1.85433000E-05, aProperties.m_Viscosity, 1e-6);
    EXPECT_NEAR(1006.4342, aProperties.m_SpecificHeat, 0.0001);
    EXPECT_NEAR(1.045287093, aProperties.m_Density, 0.0001);
    EXPECT_NEAR(0.713585333, aProperties.m_PrandlNumber, 0.0001);
}

TEST_F(TestGasPropertiesSingleGas, TestRealPropertiesLowPressureRepeat)
{
    SCOPED_TRACE("Begin Test: Gas Properties (Air) real properties - Temperature = 300 [K], "
                 "Pressure = 90,000 [Pa] (Repeatability)");

    auto aGas = GetGas();

    aGas.setTemperatureAndPressure(300, 90000);
    auto aProperties = aGas.getGasProperties();

    EXPECT_NEAR(28.97, aProperties.m_MolecularWeight, 0.0001);
    EXPECT_NEAR(2.61533000E-02, aProperties.m_ThermalConductivity, 1e-6);
    EXPECT_NEAR(1.85433000E-05, aProperties.m_Viscosity, 1e-6);
    EXPECT_NEAR(1006.4342, aProperties.m_SpecificHeat, 0.0001);
    EXPECT_NEAR(1.045287093, aProperties.m_Density, 0.0001);
    EXPECT_NEAR(0.713585333, aProperties.m_PrandlNumber, 0.0001);
}

TEST_F(TestGasPropertiesSingleGas, TotalPercents)
{
    SCOPED_TRACE("Begin Test: Gas Properties (Air) - Total percents.");

    auto aGas = GetGas();

    double percents = aGas.totalPercent();

    ASSERT_EQ(1.0, percents);
}
