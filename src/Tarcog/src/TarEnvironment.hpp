#ifndef TARENVIRONMENT_H
#define TARENVIRONMENT_H

#include <memory>

#include "BaseTarcogLayer.hpp"
#include "Gas.hpp"

namespace Tarcog {

  class CTarSurface;

  enum class BoundaryConditionsCoeffModel { CalculateH, HPrescribed, HcPrescribed };
  enum class Environment { Indoor, Outdoor };

  class CTarEnvironment : public CBaseTarcogLayer, public CGasLayer {
  public:
    CTarEnvironment( double t_Pressure, double t_AirSpeed, AirHorizontalDirection t_AirDirection );
    CTarEnvironment( const CTarEnvironment& t_Environment );

    ~CTarEnvironment();

    void setHCoeffModel( const BoundaryConditionsCoeffModel t_BCModel, const double t_HCoeff = 0 );
    void setForcedVentilation( const ForcedVentilation &t_ForcedVentilation );
    void setPrescribedConvection( double const t_HInput );
    void setEnvironmentIR( double const t_InfraRed );
    void setEmissivity( double const t_Emissivity );

    double getDirectSolarRadiation() const;
    double getEnvironmentIR();
    double getHc();

    double getAirTemperature();
    
    virtual void connectToIGULayer( const std::shared_ptr< CBaseTarcogLayer >& t_IGULayer );

    virtual std::shared_ptr< CTarEnvironment > cloneEnvironment() const = 0;

  protected:
    void initializeStateVariables();
    void calculateRadiationFlow();
    virtual double calculateIRFromVariables() = 0;
    virtual double getHr() = 0;
    virtual void setIRFromEnvironment( const double t_IR ) = 0;
    virtual double getIRFromEnvironment() const = 0;

    double m_DirectSolarRadiation;
    double m_Emissivity; // Emissivity from the environment
    // double m_InfraredRadiation; // Infrared radiation from environemnt [W/m2]
    double m_HInput; // Input convection coefficient which type depends on selected BC model [W/m2*K]
    BoundaryConditionsCoeffModel m_HCoefficientModel; // Model used to calculate BC coefficient
    bool m_IRCalculatedOutside; // Keep info if IR radiation is provided (calculated) outside
    
  };

}

#endif
