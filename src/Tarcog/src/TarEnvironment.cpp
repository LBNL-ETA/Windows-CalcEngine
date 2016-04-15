#include "TarEnvironment.hpp"
#include "TarcogConstants.hpp"
#include "BaseTarcogLayer.hpp"

using namespace std;

namespace Tarcog {

  using namespace TarcogConstants;

  CTarEnvironment::CTarEnvironment( double t_AirTemperature, double t_Pressure, double t_AirSpeed, 
    AirHorizontalDirection t_AirDirection ) : CGasLayer( t_Pressure, t_AirSpeed, t_AirDirection ),
    m_AirTemperature( t_AirTemperature ),
    m_Emissivity( DEFAULT_ENV_EMISSIVITY )
  {
    m_Hr = 0;
    m_EnvironmentRadiosity = 0.0;
    m_ForcedVentilation = ForcedVentilation(); // Creates forced ventilation with zero values
    m_HInput = 0;
    m_InfraredRadiation = 0;
    m_HCoefficientModel = BoundaryConditionsCoeffModel::CalculateH;
  }

  CTarEnvironment::~CTarEnvironment() {
    tearDownConnections();
  }

  double CTarEnvironment::getGasTemperature() {
    return m_AirTemperature;
  }

  void CTarEnvironment::setHCoeffModel( BoundaryConditionsCoeffModel const t_BCModel ) {
    m_HCoefficientModel = t_BCModel;
    resetCalculated();
  }

  void CTarEnvironment::setForcedVentilation( ForcedVentilation const &t_ForcedVentilation ) {
    m_ForcedVentilation = t_ForcedVentilation;
    resetCalculated();
  }

  void CTarEnvironment::setPrescribedConvection( double const t_HInput ) {
    m_HInput = t_HInput;
    resetCalculated();
  }

  void CTarEnvironment::setInfraredRadiation( double const t_InfraRed ) {
    m_InfraredRadiation = t_InfraRed;
    resetCalculated();
  }

  void CTarEnvironment::setEmissivity( double const t_Emissivity ) {
    m_Emissivity = t_Emissivity;
    resetCalculated();
  }

  double CTarEnvironment::getAirTemperature() const {
    return m_AirTemperature;
  }

  double CTarEnvironment::getEnvironmentRadiosity() {
    calculateLayerState();
    return m_EnvironmentRadiosity;
  }

  void CTarEnvironment::connectToIGULayer( shared_ptr< CBaseTarcogLayer > ) {
    //
  }

  void CTarEnvironment::initializeStateVariables() {
    CGasLayer::initializeStateVariables();
  }

}