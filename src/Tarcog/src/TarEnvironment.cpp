#include "TarEnvironment.hpp"
#include "TarcogConstants.hpp"
#include "BaseTarcogLayer.hpp"

using namespace std;

namespace Tarcog {

  using namespace TarcogConstants;

  CTarEnvironment::CTarEnvironment( double t_Pressure, double t_AirSpeed, 
    AirHorizontalDirection t_AirDirection ) : CGasLayer( t_Pressure, t_AirSpeed, t_AirDirection ),
    m_DirectSolarRadiation( 0 ), m_Emissivity( DEFAULT_ENV_EMISSIVITY ),
    m_HInput( 0 ), m_HCoefficientModel( BoundaryConditionsCoeffModel::CalculateH  ),
	m_IRCalculatedOutside( false ) {
    m_ForcedVentilation = ForcedVentilation(); // Creates forced ventilation with zero values
  }

  CTarEnvironment::CTarEnvironment( const CTarEnvironment & t_Environment ) : 
    CBaseTarcogLayer( t_Environment ), CGasLayer( t_Environment ) {
    m_DirectSolarRadiation = t_Environment.m_DirectSolarRadiation;
    m_Emissivity = t_Environment.m_Emissivity;
    m_HInput = t_Environment.m_HInput;
    m_HCoefficientModel = t_Environment.m_HCoefficientModel;
    m_IRCalculatedOutside = t_Environment.m_IRCalculatedOutside;
  }

  CTarEnvironment::~CTarEnvironment() {
    tearDownConnections();
  }

  void CTarEnvironment::setHCoeffModel( const BoundaryConditionsCoeffModel t_BCModel, const double t_HCoeff ) {
    m_HCoefficientModel = t_BCModel;
    m_HInput = t_HCoeff;
    resetCalculated();
  }

  void CTarEnvironment::setForcedVentilation( const ForcedVentilation &t_ForcedVentilation ) {
    m_ForcedVentilation = t_ForcedVentilation;
    resetCalculated();
  }

  void CTarEnvironment::setPrescribedConvection( double const t_HInput ) {
    m_HInput = t_HInput;
    resetCalculated();
  }

  void CTarEnvironment::setEnvironmentIR( double const t_InfraRed ) {
    setIRFromEnvironment( t_InfraRed );
    m_IRCalculatedOutside = true;
    resetCalculated();
  }

  void CTarEnvironment::setEmissivity( double const t_Emissivity ) {
    m_Emissivity = t_Emissivity;
    resetCalculated();
  }

  double CTarEnvironment::getEnvironmentIR() {
    calculateLayerHeatFlow();
    return getIRFromEnvironment();
  }

  double CTarEnvironment::getHc()
  {
    return getConductionConvectionCoefficient();
  }

  double CTarEnvironment::getAirTemperature() {
    return getGasTemperature();
  }

  double CTarEnvironment::getAmbientTemperature() {
    double hc = getHc();
    double hr = getHr();
    double Tair = getAirTemperature();
    double Trad = getRadiationTemperature();
    return ( hc * Tair + hr * Trad ) / ( hc + hr );
  }

  double CTarEnvironment::getDirectSolarRadiation() const {
    return m_DirectSolarRadiation;
  }

  void CTarEnvironment::connectToIGULayer( const shared_ptr< CBaseTarcogLayer >& ) {
    //
  }

  void CTarEnvironment::initializeStateVariables() {
    CGasLayer::initializeStateVariables();
  }

  void CTarEnvironment::calculateRadiationFlow() {
    // In case of environments, there is no need to calculate radiation
    // if radiation is provided from outside calculations
    if( !m_IRCalculatedOutside ) {
      setIRFromEnvironment( calculateIRFromVariables() );
    }
  }

}
