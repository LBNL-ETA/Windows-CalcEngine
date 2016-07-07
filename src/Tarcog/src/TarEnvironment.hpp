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

  class CTarEnvironment : public CBaseTarcogLayer, public CGasLayer {
  public:
    CTarEnvironment( double t_Pressure, double t_AirSpeed, AirHorizontalDirection t_AirDirection );

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
    
    virtual void connectToIGULayer( std::shared_ptr< CBaseTarcogLayer > t_IGULayer );

  protected:
    void initializeStateVariables();
    void calculateRadiationState();
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
