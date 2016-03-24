#include <memory>
#include <gtest\gtest.h>

#include "Gas.hpp"
#include "GasData.hpp"
#include "GasItem.hpp"
#include "GasProperties.hpp"

using namespace Gasses;

class TestGasPropertiesQuadrupleGas : public testing::Test
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

		// Create coefficients for Krypton
		std::shared_ptr< CIntCoeff > KryptonCon = std::make_shared< CIntCoeff >( 9.443e-04, 2.8260e-5, 0.0 );
		std::shared_ptr< CIntCoeff > KryptonCp = std::make_shared< CIntCoeff >( 2.4809e+02, 0.0, 0.0 );
		std::shared_ptr< CIntCoeff > KryptonVisc = std::make_shared< CIntCoeff >( 2.213e-6, 7.777e-8, 0.0 );

		std::shared_ptr< CGasData > KryptonData = std::make_shared< CGasData >( "Krypton", 83.8, 1.68, KryptonCp, KryptonCon, KryptonVisc );

		// Create coefficients for Xenon
		std::shared_ptr< CIntCoeff > XenonCon = std::make_shared< CIntCoeff >( 4.538e-04, 1.723e-05, 0.0 );
		std::shared_ptr< CIntCoeff > XenonCp = std::make_shared< CIntCoeff >( 1.5834e+02, 0.0, 0.0 );
		std::shared_ptr< CIntCoeff > XenonVisc = std::make_shared< CIntCoeff >( 1.069e-6, 7.414e-8, 0.0 );

		std::shared_ptr< CGasData > XenonData = std::make_shared< CGasData >( "Xenon", 131.3, 1.66, XenonCp, XenonCon, XenonVisc );

		std::shared_ptr< CGasItem > Air = std::make_shared< CGasItem >( 0.1, AirData );
		std::shared_ptr< CGasItem > Argon = std::make_shared< CGasItem >( 0.3, ArgonData );
		std::shared_ptr< CGasItem > Krypton = std::make_shared< CGasItem >( 0.3, KryptonData );
		std::shared_ptr< CGasItem > Xenon = std::make_shared< CGasItem >( 0.3, XenonData );

		// Create gas mixture
		Gas = std::make_shared< CGas >();

		Gas->addGasItem( Air );
		Gas->addGasItem( Argon );
		Gas->addGasItem( Krypton );
		Gas->addGasItem( Xenon );
	};

public:
	std::shared_ptr< CGas > GetGas() { return Gas; };

};

TEST_F( TestGasPropertiesQuadrupleGas, TestSimpleProperties )
{
	SCOPED_TRACE( "Begin Test: Gas Properties (quadruple gas) simple mix - Temperature = 300 [K], Pressure = 101325 [Pa]" );
	
	std::shared_ptr< GasProperties > aProperties;
	std::shared_ptr< CGas > aGas;
	
	aGas = GetGas();

	aGas->setTemperatureAndPressure( 300, 101325 );
	aProperties = aGas->getSimpleGasProperties();

	EXPECT_NEAR( 79.4114, aProperties->m_MolecularWeight, 0.0001 );
	EXPECT_NEAR( 1.24480400E-02, aProperties->m_ThermalConductivity, 1e-6 );
	EXPECT_NEAR( 2.33306700E-05, aProperties->m_Viscosity, 1e-6 );
	EXPECT_NEAR( 379.15142, aProperties->m_SpecificHeat, 0.001 );
	EXPECT_NEAR( 3.225849103, aProperties->m_Density, 0.0001 );
	EXPECT_NEAR( 1.01775733E-05, aProperties->m_Alpha, 1e-6 );
	EXPECT_NEAR( 0.710622448, aProperties->m_PrandlNumber, 0.0001 );
}

TEST_F( TestGasPropertiesQuadrupleGas, TestSimplePropertiesRepeat )
{
	SCOPED_TRACE( "Begin Test: Gas Properties (quadruple gas) simple mix - Temperature = 300 [K], Pressure = 101325 [Pa] (Repeatability)" );
	
	std::shared_ptr< GasProperties > aProperties;
	std::shared_ptr< CGas > aGas;
	
	aGas = GetGas();

	aGas->setTemperatureAndPressure( 300, 101325 );
	aProperties = aGas->getSimpleGasProperties();

	EXPECT_NEAR( 79.4114, aProperties->m_MolecularWeight, 0.0001 );
	EXPECT_NEAR( 1.24480400E-02, aProperties->m_ThermalConductivity, 1e-6 );
	EXPECT_NEAR( 2.33306700E-05, aProperties->m_Viscosity, 1e-6 );
	EXPECT_NEAR( 379.15142, aProperties->m_SpecificHeat, 0.001 );
	EXPECT_NEAR( 3.225849103, aProperties->m_Density, 0.0001 );
	EXPECT_NEAR( 1.01775733E-05, aProperties->m_Alpha, 1e-6 );
	EXPECT_NEAR( 0.710622448, aProperties->m_PrandlNumber, 0.0001 );
}

TEST_F( TestGasPropertiesQuadrupleGas, TestRealProperties )
{
	SCOPED_TRACE( "Begin Test: Gas Properties (quadruple gas) real mix - Temperature = 300 [K], Pressure = 101325 [Pa]" );
	
	std::shared_ptr< GasProperties > aProperties;
	std::shared_ptr< CGas > aGas;
	
	aGas = GetGas();

	aGas->setTemperatureAndPressure( 300, 101325 );
	aProperties = aGas->getGasProperties();

	EXPECT_NEAR( 79.4114, aProperties->m_MolecularWeight, 0.0001 );
	EXPECT_NEAR( 1.108977555E-02, aProperties->m_ThermalConductivity, 1e-6 );
	EXPECT_NEAR( 2.412413749E-05, aProperties->m_Viscosity, 1e-6 );
	EXPECT_NEAR( 272.5637141, aProperties->m_SpecificHeat, 0.001 );
	EXPECT_NEAR( 3.225849103, aProperties->m_Density, 0.0001 );
	EXPECT_NEAR( 1.26127756E-05, aProperties->m_Alpha, 1e-6 );
	EXPECT_NEAR( 0.592921334, aProperties->m_PrandlNumber, 0.0001 );
}

TEST_F( TestGasPropertiesQuadrupleGas, TestRealPropertiesRepeat )
{
	SCOPED_TRACE( "Begin Test: Gas Properties (quadruple gas) real mix - Temperature = 300 [K], Pressure = 101325 [Pa] (Repeatability)" );
	
	std::shared_ptr< GasProperties > aProperties;
	std::shared_ptr< CGas > aGas;
	
	aGas = GetGas();

	aGas->setTemperatureAndPressure( 300, 101325 );
	aProperties = aGas->getGasProperties();

	EXPECT_NEAR( 79.4114, aProperties->m_MolecularWeight, 0.0001 );
	EXPECT_NEAR( 1.108977555E-02, aProperties->m_ThermalConductivity, 1e-6 );
	EXPECT_NEAR( 2.412413749E-05, aProperties->m_Viscosity, 1e-6 );
	EXPECT_NEAR( 272.5637141, aProperties->m_SpecificHeat, 0.001 );
	EXPECT_NEAR( 3.225849103, aProperties->m_Density, 0.0001 );
	EXPECT_NEAR( 1.26127756E-05, aProperties->m_Alpha, 1e-6 );
	EXPECT_NEAR( 0.592921334, aProperties->m_PrandlNumber, 0.0001 );
}

TEST_F( TestGasPropertiesQuadrupleGas, TestRealPropertiesLowPressure )
{
	SCOPED_TRACE( "Begin Test: Gas Properties (quadruple gas) real mix - Temperature = 300 [K], Pressure = 90,000 [Pa]" );
	
	std::shared_ptr< GasProperties > aProperties;
	std::shared_ptr< CGas > aGas;
	
	aGas = GetGas();

	aGas->setTemperatureAndPressure( 300, 90000 );
	aProperties = aGas->getGasProperties();

	EXPECT_NEAR( 79.4114, aProperties->m_MolecularWeight, 0.0001 );
	EXPECT_NEAR( 1.108977555E-02, aProperties->m_ThermalConductivity, 1e-6 );
	EXPECT_NEAR( 2.412413749E-05, aProperties->m_Viscosity, 1e-6 );
	EXPECT_NEAR( 272.5637141, aProperties->m_SpecificHeat, 0.001 );
	EXPECT_NEAR( 2.865298981, aProperties->m_Density, 0.0001 );
	EXPECT_NEAR( 1.41998832E-05, aProperties->m_Alpha, 1e-6 );
	EXPECT_NEAR( 0.592921334, aProperties->m_PrandlNumber, 0.0001 );
}

TEST_F( TestGasPropertiesQuadrupleGas, TestRealPropertiesLowPressureRepeat )
{
	SCOPED_TRACE( "Begin Test: Gas Properties (quadruple gas) real mix - Temperature = 300 [K], Pressure = 90,000 [Pa] (Repeatability)" );
	
	std::shared_ptr< GasProperties > aProperties;
	std::shared_ptr< CGas > aGas;
	
	aGas = GetGas();

	aGas->setTemperatureAndPressure( 300, 90000 );
	aProperties = aGas->getGasProperties();

	EXPECT_NEAR( 79.4114, aProperties->m_MolecularWeight, 0.0001 );
	EXPECT_NEAR( 1.108977555E-02, aProperties->m_ThermalConductivity, 1e-6 );
	EXPECT_NEAR( 2.412413749E-05, aProperties->m_Viscosity, 1e-6 );
	EXPECT_NEAR( 272.5637141, aProperties->m_SpecificHeat, 0.001 );
	EXPECT_NEAR( 2.865298981, aProperties->m_Density, 0.0001 );
	EXPECT_NEAR( 1.41998832E-05, aProperties->m_Alpha, 1e-6 );
	EXPECT_NEAR( 0.592921334, aProperties->m_PrandlNumber, 0.0001 );
}

TEST_F(TestGasPropertiesQuadrupleGas, TotalPercents)
{
	SCOPED_TRACE( "Begin Test: Gas Properties (quadruple gas) - Total percents." );

	std::shared_ptr< CGas > aGas;

	aGas = GetGas();

	double percents = aGas->totalPercent();

	ASSERT_EQ( 1.0, percents );
}