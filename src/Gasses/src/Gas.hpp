#ifndef GAS_H
#define GAS_H

#include <memory>
#include <vector>

namespace Gasses
{

  class CGasItem;
  class CGasData;
  struct GasProperties;

	class CGas
	{
	public:
    CGas();
		void addGasItem( std::shared_ptr< CGasItem > t_GasItem );
		double totalPercent();
		std::shared_ptr< GasProperties > getSimpleGasProperties();
    std::shared_ptr< GasProperties > getGasProperties();
		void setTemperatureAndPressure( double t_Temperature, double t_Pressure );

		CGas& operator=( const CGas& t_Gas );

	private:
		void molecularWeight();

    double viscTwoGasses(std::shared_ptr< GasProperties > t_Gas1Properties,
      std::shared_ptr< GasProperties > t_Gas2Properties);
    double viscDenomTwoGasses(std::shared_ptr< CGasItem > t_GasItem1,
      std::shared_ptr< CGasItem > t_GasItem2);

    double lambdaPrimTwoGasses(std::shared_ptr< GasProperties > t_Gas1Properties,
      std::shared_ptr< GasProperties > t_Gas2Properties);
     double lambdaSecondTwoGasses(std::shared_ptr< GasProperties > t_Gas1Properties,
      std::shared_ptr< GasProperties > t_Gas2Properties);
    double lambdaPrimDenomTwoGasses(std::shared_ptr< CGasItem > t_GasItem1,
      std::shared_ptr< CGasItem > t_GasItem2);
    double lambdaSecondDenomTwoGasses(std::shared_ptr< CGasItem > t_GasItem1,
      std::shared_ptr< CGasItem > t_GasItem2);

		std::vector< std::shared_ptr< CGasItem > > m_GasItem;
    std::shared_ptr< GasProperties > m_SimpleProperties;
    std::shared_ptr< GasProperties > m_Properties;

    bool m_DefaultGas;
	};

};

#endif