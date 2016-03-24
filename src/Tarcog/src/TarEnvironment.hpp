#ifndef TARENVIRONMENT_H
#define TARENVIRONMENT_H

#include <memory>

#include "BaseTarcogLayer.hpp"
#include "LayerInterfaces.hpp"
#include "Gas.hpp"

#include "LayerInterfaces.hpp"

namespace Tarcog {

  class CTarSurface;

  enum BoundaryConditionsCoeffModel { CalculateH, HPrescribed, HcPrescribed };

  class CTarEnvironment :
    public CBaseTarcogLayer, public CGasLayer {
  public:
    CTarEnvironment( double t_AirTemperature, double t_Pressure, double t_AirSpeed, AirHorizontalDirection t_AirDirection );

    ~CTarEnvironment();

    void setHCoeffModel( BoundaryConditionsCoeffModel const t_BCModel );
    void setForcedVentilation( ForcedVentilation const &t_ForcedVentilation );
    void setPrescribedConvection( double const t_HInput );
    void setInfraredRadiation( double const t_InfraRed );
    void setEmissivity( double const t_Emissivity );

    double getAirTemperature() const;

    virtual double getEnvironmentRadiosity();
    
    virtual void connectToIGULayer( std::shared_ptr< CBaseTarcogLayer > t_IGULayer );

  protected:
    //virtual void calculateRadiationState();
    //virtual void calculateConvectionConductionState();  
    void initializeStateVariables();

    double m_AirTemperature; // environment temperature [K]
    double m_InfraredRadiation; // Infrared radiation from environemnt [W/m2]
    double m_Emissivity; // Emissivity from the environment
    BoundaryConditionsCoeffModel m_HCoefficientModel; // Model used to calculate BC coefficient
    double m_HInput; // Input convection coefficient which type depends on selected BC model [W/m2*K]
    double m_Hr; // Radiative surface heat transfer coefficient [W/m2*K]

    double m_EnvironmentRadiosity; // Radiation from the environment

  private:
    double getGasTemperature();
    
  };

}

#endif