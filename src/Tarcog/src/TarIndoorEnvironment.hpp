
#ifndef TARINDOORENVIRONMENT_H
#define TARINDOORENVIRONMENT_H

#include "TarEnvironment.hpp"
//#include "LayerInterfaces.hpp"

namespace Tarcog {

  class CTarIndoorEnvironment : public CTarEnvironment {
  public:
    CTarIndoorEnvironment( const double t_AirTemperature, const double t_Pressure );
    CTarIndoorEnvironment( const CTarIndoorEnvironment& t_Indoor );

    void connectToIGULayer( const std::shared_ptr< CBaseTarcogLayer >& t_IGULayer );

    void setRoomRadiationTemperature( double const t_RadiationTemperature );

    virtual std::shared_ptr< CBaseTarcogLayer > clone() const;
    virtual std::shared_ptr< CTarEnvironment > cloneEnvironment() const;

  private:
    double getGasTemperature();
    double calculateIRFromVariables();
    void calculateConvectionOrConductionFlow();
    
    void calculateHc();
    double getHr();

    void setIRFromEnvironment( const double t_IR );
    double getIRFromEnvironment() const;

    double getRadiationTemperature() const;

    double m_RoomRadiationTemperature;
  };

}

#endif