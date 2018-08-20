#ifndef GAS_H
#define GAS_H

#include <memory>
#include <vector>
#include "GasProperties.hpp"

namespace Gases {

	class CGasItem;
	class CGasData;

	class CGas {
	public:
		CGas();
		CGas( const CGas& t_Gas );
		void addGasItem( double percent, const CGasData & t_GasData );
		double totalPercent();
		const GasProperties & getSimpleGasProperties();
		const GasProperties & getGasProperties();
		void setTemperatureAndPressure( double t_Temperature, double t_Pressure );

		CGas& operator=( CGas const& t_Gas );
		bool operator==( CGas const& t_Gas ) const;
		bool operator!=( CGas const& t_Gas ) const;

	private:

		const GasProperties & getStandardPressureGasProperties();
		const GasProperties & getVacuumPressureGasProperties();

		double viscTwoGases( GasProperties const& t_Gas1Properties, GasProperties const& t_Gas2Properties ) const;
		double viscDenomTwoGases( CGasItem& t_GasItem1, CGasItem& t_GasItem2 ) const;

		double lambdaPrimTwoGases( GasProperties const& t_Gas1Properties, GasProperties const& t_Gas2Properties ) const;
		double lambdaSecondTwoGases( GasProperties const& t_Gas1Properties, GasProperties const& t_Gas2Properties ) const;

		double lambdaPrimDenomTwoGases( CGasItem& t_GasItem1, CGasItem& t_GasItem2 ) const;
		double lambdaSecondDenomTwoGases( CGasItem& t_GasItem1, CGasItem& t_GasItem2 ) const;

		std::vector< CGasItem > m_GasItem;
		GasProperties m_SimpleProperties;
		GasProperties m_Properties;

		bool m_DefaultGas;
		double m_Pressure;
	};

}

#endif
