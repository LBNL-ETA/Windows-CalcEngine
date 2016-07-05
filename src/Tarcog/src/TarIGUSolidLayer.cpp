#include <stdexcept>
#include <assert.h>

#include "TarIGUSolidLayer.hpp"
#include "BaseTarcogLayer.hpp"
#include "TarSurface.hpp"

using namespace std;

namespace Tarcog {

  CTarIGUSolidLayer::CTarIGUSolidLayer( double const t_Thickness, double const t_Conductivity, 
    shared_ptr< CTarSurface > t_FrontSurface, shared_ptr< CTarSurface > t_BackSurface )
    : CBaseIGUTarcogLayer( t_Thickness ), m_Conductivity( t_Conductivity ), m_SolarAbsorptance( 0 ) {
    if ( t_FrontSurface != nullptr && t_BackSurface != nullptr ) {
      m_FrontSurface = t_FrontSurface;
      m_BackSurface = t_BackSurface;
    } else {
      m_FrontSurface = make_shared< CTarSurface >();
      m_BackSurface = make_shared< CTarSurface >();
    }
  }

  CTarIGUSolidLayer::CTarIGUSolidLayer( double const t_Thickness, double const t_Conductivity,
    double const t_FrontEmissivity, double const t_FrontIRTransmittance,
    double const t_BackEmissivity, double const t_BackIRTransmittance ) : CBaseIGUTarcogLayer( t_Thickness ),
    m_Conductivity( t_Conductivity ), m_SolarAbsorptance( 0 ) {
    m_FrontSurface = make_shared< CTarSurface >( t_FrontEmissivity, t_FrontIRTransmittance );
    m_BackSurface = make_shared< CTarSurface >( t_BackEmissivity, t_BackIRTransmittance );
  }

  void CTarIGUSolidLayer::connectToBackSide( shared_ptr< CBaseTarcogLayer > t_Layer ) {
    CBaseTarcogLayer::connectToBackSide( t_Layer );
    t_Layer->setSurface( m_BackSurface, SurfacePosition::Front );
  }

  void CTarIGUSolidLayer::calculateConvectionConductionState() {
    if ( m_Thickness == 0 ) {
      throw runtime_error( "Solid layer thickness is set to zero." );
    }

    m_ConductiveConvectiveCoeff = m_Conductivity / m_Thickness;
  }

  void CTarIGUSolidLayer::setLayerState(double const t_Tf, double const t_Tb, double const t_Jf, double const t_Jb ) {
    setSurfaceState( t_Tf, t_Jf, SurfacePosition::Front );
    setSurfaceState( t_Tb, t_Jb, SurfacePosition::Back );
    if ( m_NextLayer != nullptr ) {
      m_NextLayer->resetCalculated();
    }
    if( m_PreviousLayer != nullptr ) {
      m_PreviousLayer->resetCalculated();
    }
  }

  void CTarIGUSolidLayer::setSurfaceState( double const t_Temperature, double const t_J, SurfacePosition const t_Position ) {
    switch ( t_Position )
    {
    case SurfacePosition::Front:
      assert( m_FrontSurface != nullptr );
      m_FrontSurface->setTemperature( t_Temperature );
      m_FrontSurface->setJ( t_J );
      break;
    case SurfacePosition::Back:
      assert( m_BackSurface != nullptr );
      m_BackSurface->setTemperature( t_Temperature );
      m_BackSurface->setJ( t_J );
      break;
    default:
      assert( t_Position == SurfacePosition::Front || t_Position == SurfacePosition::Back );
      break;
    }

    resetCalculated();
  }

  void CTarIGUSolidLayer::setSolarRadiation( double const t_SolarRadiation ) {
    m_LayerGainFlow = t_SolarRadiation * m_SolarAbsorptance;
    resetCalculated();
  }

  void CTarIGUSolidLayer::setSolarAbsorptance( double const t_SolarAbsorptance ) {
    m_SolarAbsorptance = t_SolarAbsorptance;
    resetCalculated();
  }

  void CTarIGUSolidLayer::initializeStateVariables() {
    // Initialization of intermediate calculation variables goes here
  }

}