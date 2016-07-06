#ifndef TAROUTDOORENVIRONEMENT_H
#define TAROUTDOORENVIRONEMENT_H

#include "TarcogConstants.hpp"
#include "TarEnvironment.hpp"

namespace Tarcog {

  enum SkyModel { AllSpecified, TSkySpecified, Swinbank };

  class CTarOutdoorEnvironment : public CTarEnvironment {
  public:
    CTarOutdoorEnvironment( const double t_Temperature, const double t_Pressure, const double t_AirSpeed, 
      const double t_DirectSolarRadiation, const AirHorizontalDirection t_AirDirection, 
      const double t_SkyTemperature, const SkyModel t_Model, 
      const double t_FractClearSky = TarcogConstants::DEFAULT_FRACTION_OF_CLEAR_SKY );

    void connectToIGULayer( std::shared_ptr< CBaseTarcogLayer > t_IGULayer );

    double getAirTemperature();

  private:
    double calculateIRFromVariables();
    void storeRadiationAtSurface();
    void calculateConvectionConductionState();

    void calculateHc();
    double getHr();

    double m_Tsky;
    double m_FractionOfClearSky;
    SkyModel m_SkyModel;
  };

}

#endif