#define _USE_MATH_DEFINES
#include <math.h>
#include <stdexcept>
#include <assert.h>

#include "Constants.hpp"
#include "TarSurface.hpp"
#include "TarcogConstants.hpp"
#include "TarOutdoorEnvironment.hpp"

using namespace std;
using namespace ConstantsData;

namespace Tarcog {

  using namespace TarcogConstants;

  CTarOutdoorEnvironment::CTarOutdoorEnvironment( double t_AirTemperature, double t_Pressure, double t_AirSpeed, 
    AirHorizontalDirection t_AirDirection ) : 
    CTarEnvironment( t_AirTemperature, t_Pressure, t_AirSpeed, t_AirDirection ), 
    m_DirectSolarRadiation( 0 ), m_Tsky( t_AirTemperature ), m_FractionOfClearSky( DEFAULT_FRACTION_OF_CLEAR_SKY ),
    m_SkyModel( AllSpecified ) {
    m_FrontSurface = make_shared< CTarSurface >();
    m_FrontSurface->setTemperature( t_AirTemperature );
  }

  void CTarOutdoorEnvironment::calculateRadiationState() {
    double aEmissivity = 0;
    switch ( m_SkyModel ) {
    case AllSpecified:
      aEmissivity = m_Emissivity * pow( m_Tsky, 4 ) / pow( m_AirTemperature, 4 );
      break;
    case TSkySpecified:
      aEmissivity = pow( m_Tsky, 4 ) / pow( m_AirTemperature, 4 );
      break;
    case Swinbank:
      aEmissivity = 5.31e-13 * pow( m_AirTemperature, 6 ) / ( STEFANBOLTZMANN * pow( m_AirTemperature, 4 ) );
      break;
    default:
      throw runtime_error( "Incorrect sky model specified." );
      break;
    }

    double radiationTemperature = 0;
    if ( m_HCoefficientModel == HPrescribed ) {
      radiationTemperature = m_AirTemperature;
    }
    else {
      double fSky = ( 1 + cos( m_Tilt * M_PI / 180 ) ) / 2;
      double fGround = 1 - fSky;
      double eZero = fGround + ( 1 - m_FractionOfClearSky ) * fSky + fSky * m_FractionOfClearSky * aEmissivity;
      radiationTemperature = m_AirTemperature * pow( eZero, 0.25 );
    }

    m_EnvironmentRadiosity = STEFANBOLTZMANN * pow( radiationTemperature, 4 );
    assert( m_FrontSurface != nullptr );
    m_FrontSurface->setJ( m_EnvironmentRadiosity );

  };

  void CTarOutdoorEnvironment::setSkyModel( SkyModel const t_SkyModel ) {
    m_SkyModel = t_SkyModel;
    resetCalculated();
  };

  void CTarOutdoorEnvironment::setDirectSolarRadiation( double const t_SolarRadiation ) {
    m_DirectSolarRadiation = t_SolarRadiation;
    resetCalculated();
  };

  void CTarOutdoorEnvironment::setSkyTemperature( double const t_SkyTemperature ) {
    m_Tsky = t_SkyTemperature;
    resetCalculated();
  };

  void CTarOutdoorEnvironment::setFractionOfClearSky( double const t_FractionOfClearSky ) {
    m_FractionOfClearSky = t_FractionOfClearSky;
    resetCalculated();
  };

  double CTarOutdoorEnvironment::getDirectSolarRadiation() const {
    return m_DirectSolarRadiation;
  };

  void CTarOutdoorEnvironment::connectToIGULayer( shared_ptr< CBaseTarcogLayer > t_IGULayer ) {
    this->connectToBackSide( t_IGULayer );
    m_BackSurface = t_IGULayer->getSurface( SurfacePosition::Front );
  };

  void CTarOutdoorEnvironment::calculateConvectionConductionState() {
    switch ( m_HCoefficientModel )
    {
      case Tarcog::CalculateH: {
        calculateHc();
        break;
      }
      case Tarcog::HPrescribed: {
        assert( m_BackSurface != nullptr );
        assert( m_FrontSurface != nullptr );
        double hr = getRadiationFlow() / ( m_BackSurface->getTemperature() - m_FrontSurface->getTemperature() );
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
  };

  void CTarOutdoorEnvironment::calculateHc() {
    m_ConductiveConvectiveCoeff = 4 + 4 * m_AirSpeed;
  };

}