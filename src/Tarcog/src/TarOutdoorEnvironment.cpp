#define _USE_MATH_DEFINES
#include <math.h>
#include <stdexcept>
#include <assert.h>

#include "Constants.hpp"
#include "TarSurface.hpp"
#include "TarcogConstants.hpp"
#include "TarOutdoorEnvironment.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace ConstantsData;
using namespace FenestrationCommon;

namespace Tarcog {

  using namespace TarcogConstants;

  CTarOutdoorEnvironment::CTarOutdoorEnvironment( const double t_AirTemperature, const double t_Pressure, 
    const double t_AirSpeed, const double t_DirectSolarRadiation, const AirHorizontalDirection t_AirDirection, 
    const double t_SkyTemperature, const SkyModel t_Model, const double t_FractionClearSky ) : 
    CTarEnvironment( t_Pressure, t_AirSpeed, t_AirDirection ), m_Tsky( t_SkyTemperature ), 
    m_FractionOfClearSky( t_FractionClearSky ), m_SkyModel( t_Model ) {
    m_FrontSurface = make_shared< CTarSurface >();
    m_FrontSurface->setTemperature( t_AirTemperature );
    m_DirectSolarRadiation = t_DirectSolarRadiation;
  }

  double CTarOutdoorEnvironment::calculateIRFromVariables() {
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
    if( m_HCoefficientModel == HPrescribed ) {
      radiationTemperature = getAirTemperature();
    } else {
      double fSky = ( 1 + cos( m_Tilt * M_PI / 180 ) ) / 2;
      double fGround = 1 - fSky;
      double eZero = fGround + ( 1 - m_FractionOfClearSky ) * fSky + fSky * m_FractionOfClearSky * aEmissivity;
      radiationTemperature = getAirTemperature() * pow( eZero, 0.25 );
    }

    return STEFANBOLTZMANN * pow( radiationTemperature, 4 );
  }

  void CTarOutdoorEnvironment::connectToIGULayer( shared_ptr< CBaseTarcogLayer > t_IGULayer ) {
    this->connectToBackSide( t_IGULayer );
    m_BackSurface = t_IGULayer->getSurface( Side::Front );
  }

  double CTarOutdoorEnvironment::getGasTemperature() {
    assert( m_FrontSurface != nullptr );
    return m_FrontSurface->getTemperature();
  }

  void CTarOutdoorEnvironment::calculateConvectionConductionState() {
    switch ( m_HCoefficientModel ) {
      case Tarcog::CalculateH: {
        calculateHc();
        break;
      }
      case Tarcog::HPrescribed: {
        assert( m_BackSurface != nullptr );
        assert( m_FrontSurface != nullptr );
        double hr = getHr();
        m_ConductiveConvectiveCoeff = m_HInput - hr;
        break;
      }
      case Tarcog::HcPrescribed: {
        m_ConductiveConvectiveCoeff = m_HInput;
        break;
      }
      default: {
        throw runtime_error( "Incorrect definition for convection model (Outdoor environment)." );
      }
    }
  }

  void CTarOutdoorEnvironment::calculateHc() {
    m_ConductiveConvectiveCoeff = 4 + 4 * m_AirSpeed;
  }

  double CTarOutdoorEnvironment::getHr() {
    return getRadiationFlow() / ( m_BackSurface->getTemperature() - m_FrontSurface->getTemperature() );
  }

  void CTarOutdoorEnvironment::setIRFromEnvironment( const double t_IR ) {
    assert( m_FrontSurface != nullptr );
    m_FrontSurface->setJ( t_IR );
  }

  double CTarOutdoorEnvironment::getIRFromEnvironment() const {
    assert( m_FrontSurface != nullptr );
    return m_FrontSurface->J();
  }

}
