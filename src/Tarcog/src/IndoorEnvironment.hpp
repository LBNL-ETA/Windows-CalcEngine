#ifndef TARINDOORENVIRONMENT_H
#define TARINDOORENVIRONMENT_H

#include "Environment.hpp"

namespace Tarcog {

  class CIndoorEnvironment : public CEnvironment {
  public:
    CIndoorEnvironment( const double t_AirTemperature, const double t_Pressure );
    CIndoorEnvironment( const CIndoorEnvironment& t_Indoor );

    void connectToIGULayer( const std::shared_ptr< CBaseLayer >& t_IGULayer );

    void setRoomRadiationTemperature( double const t_RadiationTemperature );

    virtual std::shared_ptr< CBaseLayer > clone() const;
    virtual std::shared_ptr< CEnvironment > cloneEnvironment() const;

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