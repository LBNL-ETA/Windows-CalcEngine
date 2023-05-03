#include <memory>
#include <string>
#include <gtest/gtest.h>

#include "WCEGases.hpp"

using namespace Gases;

// Example that uses built in gas properties

class TestGasPropertiesQuadrupleGas : public testing::Test
{};

TEST_F(TestGasPropertiesQuadrupleGas, TestSimpleProperties)
{
    SCOPED_TRACE("Begin Test: Gas Properties (quadruple gas) simple mix - Temperature = 300 [K], "
                 "Pressure = 101325 [Pa]");

    const std::vector<Gases::CGasItem> aGasItems = {
      {0.1, GasDef::Air}, {0.3, GasDef::Argon}, {0.3, GasDef::Krypton}, {0.3, GasDef::Xenon}};
    CGas aGas(aGasItems);

    aGas.setTemperatureAndPressure(300, 101325);
    auto aProperties = aGas.getSimpleGasProperties();

    EXPECT_NEAR(79.4114, aProperties.m_MolecularWeight, 0.0001);
    EXPECT_NEAR(1.24480400E-02, aProperties.m_ThermalConductivity, 1e-6);
    EXPECT_NEAR(2.33306700E-05, aProperties.m_Viscosity, 1e-6);
    EXPECT_NEAR(379.15142, aProperties.m_SpecificHeat, 0.001);
    EXPECT_NEAR(3.225849103, aProperties.m_Density, 0.0001);
    EXPECT_NEAR(0.710622448, aProperties.m_PrandlNumber, 0.0001);
}

TEST_F(TestGasPropertiesQuadrupleGas, TestSimplePropertiesRepeat)
{
    SCOPED_TRACE("Begin Test: Gas Properties (quadruple gas) simple mix - Temperature = 300 [K], "
                 "Pressure = 101325 [Pa] (Repeatability)");

    const std::vector<Gases::CGasItem> aGasItems = {
      {0.1, GasDef::Air}, {0.3, GasDef::Argon}, {0.3, GasDef::Krypton}, {0.3, GasDef::Xenon}};
    CGas aGas(aGasItems);

    aGas.setTemperatureAndPressure(300, 101325);
    auto aProperties = aGas.getSimpleGasProperties();

    EXPECT_NEAR(79.4114, aProperties.m_MolecularWeight, 0.0001);
    EXPECT_NEAR(1.24480400E-02, aProperties.m_ThermalConductivity, 1e-6);
    EXPECT_NEAR(2.33306700E-05, aProperties.m_Viscosity, 1e-6);
    EXPECT_NEAR(379.15142, aProperties.m_SpecificHeat, 0.001);
    EXPECT_NEAR(3.225849103, aProperties.m_Density, 0.0001);
    EXPECT_NEAR(0.710622448, aProperties.m_PrandlNumber, 0.0001);

    aGas.setTemperatureAndPressure(300, 101325);
    auto aPropertiesR = aGas.getSimpleGasProperties();

    EXPECT_NEAR(79.4114, aPropertiesR.m_MolecularWeight, 0.0001);
    EXPECT_NEAR(1.24480400E-02, aPropertiesR.m_ThermalConductivity, 1e-6);
    EXPECT_NEAR(2.33306700E-05, aPropertiesR.m_Viscosity, 1e-6);
    EXPECT_NEAR(379.15142, aPropertiesR.m_SpecificHeat, 0.001);
    EXPECT_NEAR(3.225849103, aPropertiesR.m_Density, 0.0001);
    EXPECT_NEAR(0.710622448, aPropertiesR.m_PrandlNumber, 0.0001);
}

TEST_F(TestGasPropertiesQuadrupleGas, TestRealProperties)
{
    SCOPED_TRACE("Begin Test: Gas Properties (quadruple gas) real mix - Temperature = 300 [K], "
                 "Pressure = 101325 [Pa]");

    const std::vector<Gases::CGasItem> aGasItems = {
      {0.1, GasDef::Air}, {0.3, GasDef::Argon}, {0.3, GasDef::Krypton}, {0.3, GasDef::Xenon}};
    CGas aGas(aGasItems);

    aGas.setTemperatureAndPressure(300, 101325);
    auto aProperties = aGas.getGasProperties();

    EXPECT_NEAR(79.4114, aProperties.m_MolecularWeight, 0.0001);
    EXPECT_NEAR(1.108977555E-02, aProperties.m_ThermalConductivity, 1e-6);
    EXPECT_NEAR(2.412413749E-05, aProperties.m_Viscosity, 1e-6);
    EXPECT_NEAR(272.5637141, aProperties.m_SpecificHeat, 0.001);
    EXPECT_NEAR(3.225849103, aProperties.m_Density, 0.0001);
    EXPECT_NEAR(0.592921334, aProperties.m_PrandlNumber, 0.0001);
}

TEST_F(TestGasPropertiesQuadrupleGas, TestRealPropertiesReplacingGases)
{
    SCOPED_TRACE("Begin Test: Gas Properties (quadruple gas) real mix - Temperature = 300 [K], "
                 "Pressure = 101325 [Pa] (Replacing gases)");

    CGas aGas;

    aGas.addGasItem(0.1, GasDef::Air);
    aGas.addGasItem(0.3, GasDef::Argon);
    aGas.addGasItem(0.3, GasDef::Krypton);
    aGas.addGasItem(0.3, GasDef::Xenon);

    aGas.setTemperatureAndPressure(300, 101325);
    auto aProperties = aGas.getGasProperties();

    EXPECT_NEAR(79.4114, aProperties.m_MolecularWeight, 0.0001);
    EXPECT_NEAR(1.108977555E-02, aProperties.m_ThermalConductivity, 1e-6);
    EXPECT_NEAR(2.412413749E-05, aProperties.m_Viscosity, 1e-6);
    EXPECT_NEAR(272.5637141, aProperties.m_SpecificHeat, 0.001);
    EXPECT_NEAR(3.225849103, aProperties.m_Density, 0.0001);
    EXPECT_NEAR(0.592921334, aProperties.m_PrandlNumber, 0.0001);
}

TEST_F(TestGasPropertiesQuadrupleGas, TestRealPropertiesLowPressure)
{
    SCOPED_TRACE("Begin Test: Gas Properties (quadruple gas) real mix - Temperature = 300 [K], "
                 "Pressure = 90,000 [Pa]");

    const std::vector<Gases::CGasItem> aGasItems = {
      {0.1, GasDef::Air}, {0.3, GasDef::Argon}, {0.3, GasDef::Krypton}, {0.3, GasDef::Xenon}};
    CGas aGas(aGasItems);

    aGas.setTemperatureAndPressure(300, 90000);
    auto aProperties = aGas.getGasProperties();

    EXPECT_NEAR(79.4114, aProperties.m_MolecularWeight, 0.0001);
    EXPECT_NEAR(1.108977555E-02, aProperties.m_ThermalConductivity, 1e-6);
    EXPECT_NEAR(2.412413749E-05, aProperties.m_Viscosity, 1e-6);
    EXPECT_NEAR(272.5637141, aProperties.m_SpecificHeat, 0.001);
    EXPECT_NEAR(2.865298981, aProperties.m_Density, 0.0001);
    EXPECT_NEAR(0.592921334, aProperties.m_PrandlNumber, 0.0001);
}

TEST_F(TestGasPropertiesQuadrupleGas, TestRealPropertiesLowPressureRepeat)
{
    SCOPED_TRACE("Begin Test: Gas Properties (quadruple gas) real mix - Temperature = 300 [K], "
                 "Pressure = 90,000 [Pa] (Repeatability)");

    const std::vector<Gases::CGasItem> aGasItems = {
      {0.1, GasDef::Air}, {0.3, GasDef::Argon}, {0.3, GasDef::Krypton}, {0.3, GasDef::Xenon}};
    CGas aGas(aGasItems);

    aGas.setTemperatureAndPressure(300, 90000);
    auto aProperties = aGas.getGasProperties();

    EXPECT_NEAR(79.4114, aProperties.m_MolecularWeight, 0.0001);
    EXPECT_NEAR(1.108977555E-02, aProperties.m_ThermalConductivity, 1e-6);
    EXPECT_NEAR(2.412413749E-05, aProperties.m_Viscosity, 1e-6);
    EXPECT_NEAR(272.5637141, aProperties.m_SpecificHeat, 0.001);
    EXPECT_NEAR(2.865298981, aProperties.m_Density, 0.0001);
    EXPECT_NEAR(0.592921334, aProperties.m_PrandlNumber, 0.0001);

    aGas.setTemperatureAndPressure(300, 90000);
    auto aPropertiesR = aGas.getGasProperties();

    EXPECT_NEAR(79.4114, aPropertiesR.m_MolecularWeight, 0.0001);
    EXPECT_NEAR(1.108977555E-02, aPropertiesR.m_ThermalConductivity, 1e-6);
    EXPECT_NEAR(2.412413749E-05, aPropertiesR.m_Viscosity, 1e-6);
    EXPECT_NEAR(272.5637141, aPropertiesR.m_SpecificHeat, 0.001);
    EXPECT_NEAR(2.865298981, aPropertiesR.m_Density, 0.0001);
    EXPECT_NEAR(0.592921334, aPropertiesR.m_PrandlNumber, 0.0001);
}

TEST_F(TestGasPropertiesQuadrupleGas, TotalPercents)
{
    SCOPED_TRACE("Begin Test: Gas Properties (quadruple gas) - Total percents.");

    const std::vector<Gases::CGasItem> aGasItems = {
      {0.1, GasDef::Air}, {0.3, GasDef::Argon}, {0.3, GasDef::Krypton}, {0.3, GasDef::Xenon}};
    CGas aGas(aGasItems);

    double percents = aGas.totalPercent();

    EXPECT_NEAR(1.0, percents, 1e-6);
}
