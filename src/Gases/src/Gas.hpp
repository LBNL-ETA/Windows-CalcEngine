#ifndef GAS_H
#define GAS_H

#include <memory>
#include <vector>

namespace Gases
{

  class CGasItem;
  class CGasData;
  struct GasProperties;

  class CGas {
  public:
    CGas();
    CGas( const CGas& t_Gas );
    void addGasItem( std::shared_ptr< CGasItem > const & t_GasItem );
    double totalPercent();
    std::shared_ptr< GasProperties > getSimpleGasProperties();
    std::shared_ptr< GasProperties > getGasProperties();
    void setTemperatureAndPressure( double const t_Temperature, double const t_Pressure );

    CGas& operator=( CGas const & t_Gas );

  private:

    std::shared_ptr< GasProperties > getStandardPressureGasProperties();
    std::shared_ptr< GasProperties > getVacuumPressureGasProperties();

    double viscTwoGases( GasProperties const & t_Gas1Properties, GasProperties const & t_Gas2Properties ) const;
    double viscDenomTwoGases( CGasItem& t_GasItem1, CGasItem& t_GasItem2 );

    double lambdaPrimTwoGases( GasProperties const & t_Gas1Properties, GasProperties const & t_Gas2Properties);
    double lambdaSecondTwoGases( GasProperties const & t_Gas1Properties, GasProperties const & t_Gas2Properties);

    double lambdaPrimDenomTwoGases( CGasItem& t_GasItem1, CGasItem& t_GasItem2 );
    double lambdaSecondDenomTwoGases( CGasItem& t_GasItem1, CGasItem& t_GasItem2);

    std::vector< std::shared_ptr< CGasItem > > m_GasItem;
    std::shared_ptr< GasProperties > m_SimpleProperties;
    std::shared_ptr< GasProperties > m_Properties;

    bool m_DefaultGas;
    double m_Pressure;
  };

}

#endif
