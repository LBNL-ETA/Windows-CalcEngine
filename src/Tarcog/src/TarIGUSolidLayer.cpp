#define _USE_MATH_DEFINES
#include <math.h>
#include <stdexcept>
#include <assert.h>

#include "TarIGUSolidLayer.hpp"
#include "BaseTarcogLayer.hpp"
#include "TarSurface.hpp"
#include "FenestrationCommon.hpp"
#include "TarcogConstants.hpp"
#include "LayerInterfaces.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace Tarcog {

  CTarIGUSolidLayer::CTarIGUSolidLayer( double const t_Thickness, double const t_Conductivity, 
    shared_ptr< CTarSurface > t_FrontSurface, shared_ptr< CTarSurface > t_BackSurface )
    : CBaseIGUTarcogLayer( t_Thickness ), m_Conductivity( t_Conductivity ), m_SolarAbsorptance( 0 ) {
    if ( t_FrontSurface != nullptr && t_BackSurface != nullptr ) {
      m_Surface[ Side::Front ] = t_FrontSurface;
      m_Surface[ Side::Back ] = t_BackSurface;
    } else {
      m_Surface[ Side::Front ] = make_shared< CTarSurface >();
      m_Surface[ Side::Back ] = make_shared< CTarSurface >();
    }
  }

  CTarIGUSolidLayer::CTarIGUSolidLayer( double const t_Thickness, double const t_Conductivity,
    double const t_FrontEmissivity, double const t_FrontIRTransmittance,
    double const t_BackEmissivity, double const t_BackIRTransmittance ) : CBaseIGUTarcogLayer( t_Thickness ),
    m_Conductivity( t_Conductivity ), m_SolarAbsorptance( 0 ) {
    m_Surface[ Side::Front ] = make_shared< CTarSurface >( t_FrontEmissivity, t_FrontIRTransmittance );
    m_Surface[ Side::Back ] = make_shared< CTarSurface >( t_BackEmissivity, t_BackIRTransmittance );
  }

  CTarIGUSolidLayer::CTarIGUSolidLayer( const CTarIGUSolidLayer& t_Layer ) : 
    CBaseIGUTarcogLayer( t_Layer ) {
    m_Conductivity = t_Layer.m_Conductivity;
    m_SolarAbsorptance = t_Layer.m_SolarAbsorptance;
    m_Surface = t_Layer.m_Surface;
  }

  void CTarIGUSolidLayer::connectToBackSide( const shared_ptr< CBaseTarcogLayer >& t_Layer ) {
    CBaseTarcogLayer::connectToBackSide( t_Layer );
    t_Layer->setSurface( m_Surface[ Side::Back ], Side::Front );
  }

  double CTarIGUSolidLayer::getConductivity() const {
    return m_Conductivity;
  }

  void CTarIGUSolidLayer::calculateConvectionConductionState() {
    if ( m_Thickness == 0 ) {
      throw runtime_error( "Solid layer thickness is set to zero." );
    }

    m_ConductiveConvectiveCoeff = m_Conductivity / m_Thickness;

  }

  void CTarIGUSolidLayer::setLayerState( double const t_Tf, double const t_Tb, 
    double const t_Jf, double const t_Jb ) {
    setSurfaceState( t_Tf, t_Jf, Side::Front );
    setSurfaceState( t_Tb, t_Jb, Side::Back );
    if ( m_NextLayer != nullptr ) {
      m_NextLayer->resetCalculated();
    }
    if( m_PreviousLayer != nullptr ) {
      m_PreviousLayer->resetCalculated();
    }
  }

  void CTarIGUSolidLayer::setSurfaceState( double const t_Temperature, double const t_J, Side const t_Position ) {
    shared_ptr< CTarSurface > aSurface = m_Surface.at( t_Position );
    aSurface->setTemperature( t_Temperature );
    aSurface->setJ( t_J );

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