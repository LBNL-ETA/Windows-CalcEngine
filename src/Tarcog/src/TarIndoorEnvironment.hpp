
#ifndef TARINDOORENVIRONMENT_H
#define TARINDOORENVIRONMENT_H

#include "TarEnvironment.hpp"
//#include "LayerInterfaces.hpp"

namespace Tarcog {

  class CTarIndoorEnvironment : public CTarEnvironment {
  public:
    CTarIndoorEnvironment( const double t_AirTemperature, const double t_Pressure, const double t_AirSpeed, 
      const AirHorizontalDirection t_AirDirection );

    void connectToIGULayer( std::shared_ptr< CBaseTarcogLayer > t_IGULayer );

    void setRoomRadiationTemperature( double const t_RadiationTemperature );

  private:
    // void calculateRadiationState();
    double calculateIRFromVariables();
    void storeRadiationAtSurface();
    void calculateConvectionConductionState();
    void calculateHc();

    double m_RoomRadiationTemperature;
  };

}

#endif