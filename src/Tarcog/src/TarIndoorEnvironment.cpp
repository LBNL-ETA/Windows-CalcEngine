#define _USE_MATH_DEFINES
#include <math.h>
#include <stdexcept>
#include <assert.h>

#include "TarIndoorEnvironment.hpp"

#include "Constants.hpp"
#include "TarSurface.hpp"
#include "GasProperties.hpp"

using namespace std;
using namespace Gasses;

namespace Tarcog {

  CTarIndoorEnvironment::CTarIndoorEnvironment( double t_AirTemperature, double t_Pressure, double t_AirSpeed, 
    AirHorizontalDirection t_AirDirection ) : 
    CTarEnvironment( t_AirTemperature, t_Pressure, t_AirSpeed, t_AirDirection )
  {
    using ConstantsData::STEFANBOLTZMANN;

    m_RoomRadiationTemperature = t_AirTemperature; // Radiation temperature is by default air
    double roomRadiosity = STEFANBOLTZMANN * m_Emissivity * pow( m_RoomRadiationTemperature, 4 );
    m_BackSurface = make_shared< CTarSurface >( m_Emissivity, 0 );
    m_BackSurface->setJ( roomRadiosity );
    m_BackSurface->setTemperature( t_AirTemperature );
  }

  void CTarIndoorEnvironment::connectToIGULayer( shared_ptr< CBaseTarcogLayer > t_IGULayer ) {
    t_IGULayer->connectToBackSide( shared_from_this() );
  };

  void CTarIndoorEnvironment::setRoomRadiationTemperature( double const t_RadiationTemperature ) {
    m_RoomRadiationTemperature = t_RadiationTemperature;
    resetCalculated();
  };

  void CTarIndoorEnvironment::calculateRadiationState() {
    using ConstantsData::STEFANBOLTZMANN;

    CTarEnvironment::calculateRadiationState();
    m_EnvironmentRadiosity = STEFANBOLTZMANN * m_Emissivity * pow( m_RoomRadiationTemperature, 4 );
  };

  void CTarIndoorEnvironment::calculateConvectionConductionState() {
    CTarEnvironment::calculateConvectionConductionState();
    switch (m_HCoefficientModel )
    {
      case Tarcog::CalculateH: {
        calculateHc();
        break;
      }
      case Tarcog::HPrescribed: {
        assert( m_FrontSurface == nullptr );
        double hr = getRadiationFlow() / ( m_AirTemperature - m_FrontSurface->getTemperature() );
        m_ConductiveConvectiveCoeff = m_HInput - hr;
        break;
      }
      case Tarcog::HcPrescribed: {
        m_ConductiveConvectiveCoeff = m_HInput;
        break;
      }
      default: {
        throw runtime_error( "Incorrect definition for convection model (Indoor environment)." );
        break;
      }
    }
  };

  void CTarIndoorEnvironment::calculateHc() {
    if( m_AirSpeed > 0 ) {
      m_ConductiveConvectiveCoeff = 4 + 4 * m_AirSpeed;
    } else {
      using ConstantsData::GRAVITYCONSTANT;

      assert( m_FrontSurface != nullptr );
      assert( m_BackSurface != nullptr );

      double tiltRadians = m_Tilt * M_PI / 180;
      double tMean = m_AirTemperature + 0.25 * ( m_FrontSurface->getTemperature() - m_AirTemperature );
      double deltaTemp = fabs( m_FrontSurface->getTemperature() - m_AirTemperature );
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
  };
}