#ifndef TAROUTDOORENVIRONEMENT_H
#define TAROUTDOORENVIRONEMENT_H

#include "TarEnvironment.hpp"

namespace Tarcog {

  enum SkyModel { AllSpecified, TSkySpecified, Swinbank };

  class CTarOutdoorEnvironment :

    public CTarEnvironment
  {
  public:
    CTarOutdoorEnvironment( double t_Temperature, double t_Pressure, double t_AirSpeed, 
      AirHorizontalDirection t_AirDirection );

    void setSkyModel( SkyModel const t_SkyModel );
    void setDirectSolarRadiation( double const t_SolarRadiation );
    void setSkyTemperature( double const t_SkyTemperature );
    void setFractionOfClearSky( double const t_FractionOfClearSky );

    double getDirectSolarRadiation() const;

    void connectToIGULayer( std::shared_ptr< CBaseTarcogLayer > t_IGULayer );

  private:
    void calculateRadiationState();
    void calculateConvectionConductionState();

    void calculateHc();

    double m_DirectSolarRadiation;
    double m_Tsky;
    double m_FractionOfClearSky;
    SkyModel m_SkyModel;
  };

}

#endif