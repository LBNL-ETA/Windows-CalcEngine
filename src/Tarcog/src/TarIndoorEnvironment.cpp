#define _USE_MATH_DEFINES
#include <math.h>
#include <stdexcept>
#include <assert.h>

#include "TarIndoorEnvironment.hpp"

#include "Constants.hpp"
#include "TarSurface.hpp"
#include "GasProperties.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace Gases;
using namespace FenestrationCommon;

namespace Tarcog {

  // Keep airspeed to zero and airdirection to default windward
  CTarIndoorEnvironment::CTarIndoorEnvironment( const double t_AirTemperature, const double t_Pressure ) : 
    CTarEnvironment( t_Pressure, 0, AirHorizontalDirection::Windward ) { 

    m_RoomRadiationTemperature = t_AirTemperature; // Radiation temperature is by default air
    m_Surface[ Side::Back ] = make_shared< CTarSurface >( m_Emissivity, 0 );
    m_Surface.at( Side::Back )->setTemperature( t_AirTemperature );
  }

  CTarIndoorEnvironment::CTarIndoorEnvironment( const CTarIndoorEnvironment & t_Indoor ) :
    CTarEnvironment( t_Indoor ) {
    m_RoomRadiationTemperature = t_Indoor.m_RoomRadiationTemperature;
  }

  void CTarIndoorEnvironment::connectToIGULayer( const shared_ptr< CBaseTarcogLayer >& t_IGULayer ) {
    t_IGULayer->connectToBackSide( shared_from_this() );
  }

  void CTarIndoorEnvironment::setRoomRadiationTemperature( double const t_RadiationTemperature ) {
    m_RoomRadiationTemperature = t_RadiationTemperature;
    resetCalculated();
  }

  shared_ptr< CBaseTarcogLayer > CTarIndoorEnvironment::clone() const {
    return cloneEnvironment();
  }

  shared_ptr< CTarEnvironment > CTarIndoorEnvironment::cloneEnvironment() const {
    return make_shared< CTarIndoorEnvironment >( *this );
  }

  double CTarIndoorEnvironment::getGasTemperature() {
    assert( m_Surface.at( Side::Back ) != nullptr );
    return m_Surface.at( Side::Back )->getTemperature();
  }

  double CTarIndoorEnvironment::calculateIRFromVariables() {
    using ConstantsData::STEFANBOLTZMANN;
    return STEFANBOLTZMANN * m_Emissivity * pow( m_RoomRadiationTemperature, 4 );
  }

  void CTarIndoorEnvironment::calculateConvectionOrConductionFlow() {
    // CTarEnvironment::calculateConvectionOrConductionFlow();
    switch( m_HCoefficientModel ) {
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
        throw runtime_error( "Incorrect definition for convection model (Indoor environment)." );
      }
    }
  }

  void CTarIndoorEnvironment::calculateHc() {
    if( m_AirSpeed > 0 ) {
      m_ConductiveConvectiveCoeff = 4 + 4 * m_AirSpeed;
    } else {
      using ConstantsData::GRAVITYCONSTANT;

      assert( m_Surface.at( Side::Front ) != nullptr );
      assert( m_Surface.at( Side::Back ) != nullptr );

      double tiltRadians = m_Tilt * M_PI / 180;
      double tMean = getGasTemperature() + 0.25 * ( m_Surface.at( Side::Front )->getTemperature() - getGasTemperature() );
      double deltaTemp = fabs( m_Surface.at( Side::Front )->getTemperature() - getGasTemperature() );
      m_Gas->setTemperatureAndPressure( tMean, m_Pressure );
      shared_ptr< GasProperties > aProperties = m_Gas->getGasProperties();
      double gr = GRAVITYCONSTANT * pow( m_Height, 3 ) * deltaTemp * pow( aProperties->m_Density, 2 ) /
        ( tMean * pow( aProperties->m_Viscosity, 2 ) );
      double RaCrit = 2.5e5 * pow( exp( 0.72 * m_Tilt ) / sin( tiltRadians ), 0.2 );
      double RaL = gr * aProperties->m_PrandlNumber;
      double Gnui = 0;
      if ( ( 0.0 <= m_Tilt ) && ( m_Tilt < 15.0 ) ) {
        Gnui = 0.13 * pow( RaL, 1/3.0 );
      } else if( ( 15.0 <= m_Tilt ) && ( m_Tilt <= 90.0 ) ) {
        if( RaL <= RaCrit) {
          Gnui = 0.56 * pow( RaL * sin( tiltRadians ), 0.25 );
        } else {
          Gnui = 0.13 * pow( RaL, 1/3.0 ) - pow( RaCrit, 1/3.0 ) + 0.56 * pow( RaCrit * sin( tiltRadians ), 0.25 );
        }
      } else if( ( 90.0 < m_Tilt ) && ( m_Tilt <= 179.0 ) ) {
        Gnui = 0.56 * pow( RaL * sin( tiltRadians ), 0.25 );
      } else if( ( 179.0 < m_Tilt ) && ( m_Tilt <= 180.0 ) ) {
        Gnui = 0.58 * pow( RaL, 1/3.0 );
      }
      m_ConductiveConvectiveCoeff = Gnui * aProperties->m_ThermalConductivity / m_Height;
    }
  }

  double CTarIndoorEnvironment::getHr() {
    assert( m_Surface.at( Side::Front ) != nullptr );
    return getRadiationFlow() / ( getGasTemperature() - m_Surface.at( Side::Front )->getTemperature() );
  }

  void CTarIndoorEnvironment::setIRFromEnvironment( const double t_IR ) {
    assert( m_Surface.at( Side::Back ) != nullptr );
    m_Surface.at( Side::Back )->setJ( t_IR );
  }

  double CTarIndoorEnvironment::getIRFromEnvironment() const {
    assert( m_Surface.at( Side::Back ) != nullptr );
    return m_Surface.at( Side::Back )->J();
  }
}
