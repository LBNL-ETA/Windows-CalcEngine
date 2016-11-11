#define _USE_MATH_DEFINES
#include <math.h>
#include <stdexcept>
#include <assert.h>

#include "Constants.hpp"
#include "Surface.hpp"
#include "TarcogConstants.hpp"
#include "OutdoorEnvironment.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace ConstantsData;
using namespace FenestrationCommon;

namespace Tarcog {

  using namespace TarcogConstants;

  COutdoorEnvironment::COutdoorEnvironment( const double t_AirTemperature, const double t_Pressure, 
    const double t_AirSpeed, const double t_DirectSolarRadiation, const AirHorizontalDirection t_AirDirection, 
    const double t_SkyTemperature, const SkyModel t_Model, const double t_FractionClearSky ) : 
    CEnvironment( t_Pressure, t_AirSpeed, t_AirDirection ), m_Tsky( t_SkyTemperature ), 
    m_FractionOfClearSky( t_FractionClearSky ), m_SkyModel( t_Model ) {
    m_Surface[ Side::Front ] = make_shared< CSurface >();
    m_Surface.at( Side::Front )->setTemperature( t_AirTemperature );
    m_DirectSolarRadiation = t_DirectSolarRadiation;
  }

  COutdoorEnvironment::COutdoorEnvironment( const COutdoorEnvironment& t_Outdoor ) :
    CEnvironment( t_Outdoor ) {
    m_Tsky = t_Outdoor.m_Tsky;
    m_FractionOfClearSky = t_Outdoor.m_FractionOfClearSky;
    m_SkyModel = t_Outdoor.m_SkyModel;
  }

  double COutdoorEnvironment::calculateIRFromVariables() {
    double aEmissivity = 0;
    switch( m_SkyModel ) {
    case AllSpecified:
      aEmissivity = m_Emissivity * pow( m_Tsky, 4 ) / pow( getAirTemperature(), 4 );
      break;
    case TSkySpecified:
      aEmissivity = pow( m_Tsky, 4 ) / pow( getAirTemperature(), 4 );
      break;
    case Swinbank:
      aEmissivity = 5.31e-13 * pow( getAirTemperature(), 6 ) / ( STEFANBOLTZMANN * pow( getAirTemperature(), 4 ) );
      break;
    default:
      throw runtime_error( "Incorrect sky model specified." );
      break;
    }

    double radiationTemperature = 0;
    if( m_HCoefficientModel == BoundaryConditionsCoeffModel::HPrescribed ) {
      radiationTemperature = getAirTemperature();
    } else {
      double fSky = ( 1 + cos( m_Tilt * M_PI / 180 ) ) / 2;
      double fGround = 1 - fSky;
      double eZero = fGround + ( 1 - m_FractionOfClearSky ) * fSky + fSky * m_FractionOfClearSky * aEmissivity;
      radiationTemperature = getAirTemperature() * pow( eZero, 0.25 );
    }

    return STEFANBOLTZMANN * pow( radiationTemperature, 4 );
  }

  void COutdoorEnvironment::connectToIGULayer( const shared_ptr< CBaseLayer >& t_IGULayer ) {
    this->connectToBackSide( t_IGULayer );
    m_Surface[ Side::Back ] = t_IGULayer->getSurface( Side::Front );
  }

  shared_ptr< CBaseLayer > COutdoorEnvironment::clone() const {
    return cloneEnvironment();
  }

  shared_ptr< CEnvironment > COutdoorEnvironment::cloneEnvironment() const {
    return make_shared< COutdoorEnvironment >( *this );
  }

  void COutdoorEnvironment::setSolarRadiation( const double t_SolarRadiation ) {
    m_DirectSolarRadiation = t_SolarRadiation;
  }

  double COutdoorEnvironment::getSolarRadiation() const {
    return m_DirectSolarRadiation;
  }

  double COutdoorEnvironment::getGasTemperature() {
    assert( m_Surface.at( Side::Front ) != nullptr );
    return m_Surface.at( Side::Front )->getTemperature();
  }

  void COutdoorEnvironment::calculateConvectionOrConductionFlow() {
    switch ( m_HCoefficientModel ) {
      case BoundaryConditionsCoeffModel::CalculateH: {
        calculateHc();
        break;
      }
      case BoundaryConditionsCoeffModel::HPrescribed: {
        double hr = getHr();
        m_ConductiveConvectiveCoeff = m_HInput - hr;
        break;
      }
      case BoundaryConditionsCoeffModel::HcPrescribed: {
        m_ConductiveConvectiveCoeff = m_HInput;
        break;
      }
      default: {
        throw runtime_error( "Incorrect definition for convection model (Outdoor environment)." );
      }
    }
  }

  void COutdoorEnvironment::calculateHc() {
    m_ConductiveConvectiveCoeff = 4 + 4 * m_AirSpeed;
  }

  double COutdoorEnvironment::getHr() {
    assert( m_Surface.at( Side::Back ) != nullptr );
    assert( m_Surface.at( Side::Front ) != nullptr );
    // return getRadiationFlow() / ( m_Surface.at( Side::Back )->getTemperature() - m_Surface.at( Side::Front )->getTemperature() );
    return getRadiationFlow() / ( m_Surface.at( Side::Back )->getTemperature() - getRadiationTemperature() );
  }

  double COutdoorEnvironment::getRadiationTemperature() const {
    assert( m_Surface.at( Side::Front ) != nullptr );
    return pow( m_Surface.at( Side::Front )->J() / STEFANBOLTZMANN, 0.25 );
  }

  void COutdoorEnvironment::setIRFromEnvironment( const double t_IR ) {
    assert( m_Surface.at( Side::Front ) != nullptr );
    m_Surface.at( Side::Front )->setJ( t_IR );
  }

  double COutdoorEnvironment::getIRFromEnvironment() const {
    assert( m_Surface.at( Side::Front ) != nullptr );
    return m_Surface.at( Side::Front )->J();
  }

}
