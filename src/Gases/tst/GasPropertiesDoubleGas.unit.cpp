#include <memory>
#include <gtest/gtest.h>

#include "WCEGases.hpp"

using namespace Gases;

class TestGasPropertiesDoubleGas : public testing::Test
{

private:
  std::shared_ptr< CGas > Gas;

protected:
  virtual void SetUp()
  {
    // Create coefficients for Air
    std::shared_ptr< CIntCoeff > AirCon = std::make_shared< CIntCoeff >( 2.8733e-03, 7.76e-05, 0.0 );
    std::shared_ptr< CIntCoeff > AirCp = std::make_shared< CIntCoeff >( 1.002737e+03, 1.2324e-02, 0.0 );
    std::shared_ptr< CIntCoeff > AirVisc = std::make_shared< CIntCoeff >( 3.7233e-06, 4.94e-08, 0.0 );

    std::shared_ptr< CGasData > AirData = std::make_shared< CGasData >( "Air", 28.97, 1.4, AirCp, AirCon, AirVisc );

    // Create coefficients for Argon
    std::shared_ptr< CIntCoeff > ArgonCon = std::make_shared< CIntCoeff >( 2.2848e-03, 5.1486e-05, 0.0 );
    std::shared_ptr< CIntCoeff > ArgonCp = std::make_shared< CIntCoeff >( 5.21929e+02, 0.0, 0.0 );
    std::shared_ptr< CIntCoeff > ArgonVisc = std::make_shared< CIntCoeff >( 3.3786e-06, 6.4514e-08, 0.0 );

    std::shared_ptr< CGasData > ArgonData = std::make_shared< CGasData >( "Argon", 39.948, 1.67, ArgonCp, ArgonCon, ArgonVisc );

    std::shared_ptr< CGasItem > Air = std::make_shared< CGasItem >( 0.1, AirData );
    std::shared_ptr< CGasItem > Argon = std::make_shared< CGasItem >( 0.9, ArgonData );

    // Create gas mixture
    Gas = std::make_shared< CGas >();

    Gas->addGasItem( Air );
    Gas->addGasItem( Argon );

  }

public:
  std::shared_ptr< CGas > GetGas() { return Gas; };

};

TEST_F( TestGasPropertiesDoubleGas, TestSimpleProperties )
{
  SCOPED_TRACE( "Begin Test: Gas Properties (Air 10% / Argon 90%) simple mix - Temperature = 300 [K], Pressure = 101325 [Pa]" );
  
  std::shared_ptr< GasProperties > aProperties;
  std::shared_ptr< CGas > aGas;
  
  aGas = GetGas();

  aGas->setTemperatureAndPressure( 300, 101325 );
  aProperties = aGas->getSimpleGasProperties();

  EXPECT_NEAR( 38.8502, aProperties->m_MolecularWeight, 0.0001 );
  EXPECT_NEAR( 1.85728700E-02, aProperties->m_ThermalConductivity, 1e-6 );
  EXPECT_NEAR( 2.23138500E-05, aProperties->m_Viscosity, 1e-6 );
  EXPECT_NEAR( 570.37952, aProperties->m_SpecificHeat, 0.0001 );
  EXPECT_NEAR( 1.578172439, aProperties->m_Density, 0.0001 );
  EXPECT_NEAR( 2.06329175E-05, aProperties->m_Alpha, 1e-6 );
  EXPECT_NEAR( 0.685266362, aProperties->m_PrandlNumber, 0.0001 );

}

TEST_F( TestGasPropertiesDoubleGas, TestSimplePropertiesRepeat )
{
  SCOPED_TRACE( "Begin Test: Gas Properties (Air 10% / Argon 90%) simple mix - Temperature = 300 [K], Pressure = 101325 [Pa] (Repeatability)" );
  
  std::shared_ptr< GasProperties > aProperties;
  std::shared_ptr< CGas > aGas;
  
  aGas = GetGas();

  aGas->setTemperatureAndPressure( 300, 101325 );
  aProperties = aGas->getSimpleGasProperties();

  EXPECT_NEAR( 38.8502, aProperties->m_MolecularWeight, 0.0001 );
  EXPECT_NEAR( 1.85728700E-02, aProperties->m_ThermalConductivity, 1e-6 );
  EXPECT_NEAR( 2.23138500E-05, aProperties->m_Viscosity, 1e-6 );
  EXPECT_NEAR( 570.37952, aProperties->m_SpecificHeat, 0.0001 );
  EXPECT_NEAR( 1.578172439, aProperties->m_Density, 0.0001 );
  EXPECT_NEAR( 2.06329175E-05, aProperties->m_Alpha, 1e-6 );
  EXPECT_NEAR( 0.685266362, aProperties->m_PrandlNumber, 0.0001 );

}

TEST_F( TestGasPropertiesDoubleGas, TestRealProperties )
{
  SCOPED_TRACE( "Begin Test: Gas Properties (Air 10% / Argon 90%) real mix - Temperature = 300 [K], Pressure = 101325 [Pa]" );
  
  std::shared_ptr< GasProperties > aProperties;
  std::shared_ptr< CGas > aGas;
  
  aGas = GetGas();

  aGas->setTemperatureAndPressure( 300, 101325 );
  aProperties = aGas->getGasProperties();

  EXPECT_NEAR( 38.8502, aProperties->m_MolecularWeight, 0.0001 );
  EXPECT_NEAR( 1.850941662E-02, aProperties->m_ThermalConductivity, 1e-6 );
  EXPECT_NEAR( 2.235785737E-05, aProperties->m_Viscosity, 1e-6 );
  EXPECT_NEAR( 558.0578118, aProperties->m_SpecificHeat, 0.0001 );
  EXPECT_NEAR( 1.578172439, aProperties->m_Density, 0.0001 );
  EXPECT_NEAR( 2.10164367E-05, aProperties->m_Alpha, 1e-6 );
  EXPECT_NEAR( 0.674088072, aProperties->m_PrandlNumber, 0.0001 );

}

TEST_F( TestGasPropertiesDoubleGas, TestRealPropertiesRepeat )
{
  SCOPED_TRACE( "Begin Test: Gas Properties (Air 10% / Argon 90%) real mix - Temperature = 300 [K], Pressure = 101325 [Pa] (Repeatability)" );
  
  std::shared_ptr< GasProperties > aProperties;
  std::shared_ptr< CGas > aGas;
  
  aGas = GetGas();

  aGas->setTemperatureAndPressure( 300, 101325 );
  aProperties = aGas->getGasProperties();

  EXPECT_NEAR( 38.8502, aProperties->m_MolecularWeight, 0.0001 );
  EXPECT_NEAR( 1.850941662E-02, aProperties->m_ThermalConductivity, 1e-6 );
  EXPECT_NEAR( 2.235785737E-05, aProperties->m_Viscosity, 1e-6 );
  EXPECT_NEAR( 558.0578118, aProperties->m_SpecificHeat, 0.0001 );
  EXPECT_NEAR( 1.578172439, aProperties->m_Density, 0.0001 );
  EXPECT_NEAR( 2.10164367E-05, aProperties->m_Alpha, 1e-6 );
  EXPECT_NEAR( 0.674088072, aProperties->m_PrandlNumber, 0.0001 );

}

TEST_F( TestGasPropertiesDoubleGas, TestRealPropertiesLowPressure )
{
  SCOPED_TRACE( "Begin Test: Gas Properties (Air 10% / Argon 90%) real mix - Temperature = 300 [K], Pressure = 90,000 [Pa]" );
  
  std::shared_ptr< GasProperties > aProperties;
  std::shared_ptr< CGas > aGas;
  
  aGas = GetGas();

  aGas->setTemperatureAndPressure( 300, 90000 );
  aProperties = aGas->getGasProperties();

  EXPECT_NEAR( 38.8502, aProperties->m_MolecularWeight, 0.0001 );
  EXPECT_NEAR( 1.850941662E-02, aProperties->m_ThermalConductivity, 1e-6 );
  EXPECT_NEAR( 2.235785737E-05, aProperties->m_Viscosity, 1e-6 );
  EXPECT_NEAR( 558.0578118, aProperties->m_SpecificHeat, 0.0001 );
  EXPECT_NEAR( 1.401781589, aProperties->m_Density, 0.0001 );
  EXPECT_NEAR( 2.36610050E-05, aProperties->m_Alpha, 1e-6 );
  EXPECT_NEAR( 0.674088072, aProperties->m_PrandlNumber, 0.0001 );

}

TEST_F( TestGasPropertiesDoubleGas, TestRealPropertiesLowPressureRepeat )
{
  SCOPED_TRACE( "Begin Test: Gas Properties (Air 10% / Argon 90%) real mix - Temperature = 300 [K], Pressure = 90,000 [Pa] (Repeatability)" );
  
  std::shared_ptr< GasProperties > aProperties;
  std::shared_ptr< CGas > aGas;
  
  aGas = GetGas();

  aGas->setTemperatureAndPressure( 300, 90000 );
  aProperties = aGas->getGasProperties();

  EXPECT_NEAR( 38.8502, aProperties->m_MolecularWeight, 0.0001 );
  EXPECT_NEAR( 1.850941662E-02, aProperties->m_ThermalConductivity, 1e-6 );
  EXPECT_NEAR( 2.235785737E-05, aProperties->m_Viscosity, 1e-6 );
  EXPECT_NEAR( 558.0578118, aProperties->m_SpecificHeat, 0.0001 );
  EXPECT_NEAR( 1.401781589, aProperties->m_Density, 0.0001 );
  EXPECT_NEAR( 2.36610050E-05, aProperties->m_Alpha, 1e-6 );
  EXPECT_NEAR( 0.674088072, aProperties->m_PrandlNumber, 0.0001 );

}

TEST_F( TestGasPropertiesDoubleGas, TotalPercents )
{
  SCOPED_TRACE( "Begin Test: Gas Properties (Air 10% / Argon 90%) - Total percents." );

  std::shared_ptr< CGas > aGas;

  aGas = GetGas();

  double percents = aGas->totalPercent();

  ASSERT_EQ( 1.0, percents );
}