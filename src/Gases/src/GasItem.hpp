#ifndef GASITEM_H
#define GASITEM_H

#include <memory>
#include <string>

namespace Gases
{

  class CGasData;
  struct GasProperties;

  const double DefaultPressure = 101325;
  const double DefaultTemperature = 273.15;

  class CGasItem
  {
  public:
    CGasItem();
    CGasItem( double aFraction, const std::shared_ptr< const CGasData >& t_GasData );
    CGasItem& operator=( const CGasItem& t_GasItem );
    void setTemperature( double t_Temperature );
    void setPressure( double t_Pressure );
    double getFraction() const;
    std::shared_ptr< GasProperties > getFractionalGasProperties();
    std::shared_ptr< GasProperties > getGasProperties();

  private:
    void initialize();
    void resetCalculatedProperties();
    double m_Temperature; // unit in Kelvins
    double m_Pressure; // unit in Pa
    double m_Fraction; // value between 0 and 1
    std::shared_ptr< GasProperties > m_GasProperties;
    std::shared_ptr< GasProperties > m_FractionalGasProperties;
    std::shared_ptr< const CGasData > m_GasData;
  };

}

#endif