#ifndef TAROUTDOORENVIRONEMENT_H
#define TAROUTDOORENVIRONEMENT_H

#include "TarcogConstants.hpp"
#include "Environment.hpp"

namespace Tarcog {

  enum SkyModel { AllSpecified, TSkySpecified, Swinbank };

  class COutdoorEnvironment : public CEnvironment {
  public:
    COutdoorEnvironment( const double t_Temperature, const double t_Pressure, const double t_AirSpeed, 
      const double t_DirectSolarRadiation, const AirHorizontalDirection t_AirDirection, 
      const double t_SkyTemperature, const SkyModel t_Model, 
      const double t_FractClearSky = TarcogConstants::DEFAULT_FRACTION_OF_CLEAR_SKY );

    COutdoorEnvironment( const COutdoorEnvironment& t_Outdoor );

    void connectToIGULayer( const std::shared_ptr< CBaseLayer >& t_IGULayer );

    virtual std::shared_ptr< CBaseLayer > clone() const;
    virtual std::shared_ptr< CEnvironment > cloneEnvironment() const;

    void setSolarRadiation( const double t_SolarRadiation );
    double getSolarRadiation() const;

  private:
    double getGasTemperature();
    double calculateIRFromVariables();
    void calculateConvectionOrConductionFlow();

    void calculateHc();
    double getHr();
    double getRadiationTemperature() const;

    void setIRFromEnvironment( const double t_IR );
    double getIRFromEnvironment() const;

    double m_Tsky;
    double m_FractionOfClearSky;
    SkyModel m_SkyModel;
  };

}

#endif