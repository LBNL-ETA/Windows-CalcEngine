#define _USE_MATH_DEFINES
#include <math.h>
#include <stdexcept>
#include <assert.h>

#include "IGUSolidLayer.hpp"
#include "BaseLayer.hpp"
#include "Surface.hpp"
#include "FenestrationCommon.hpp"
#include "TarcogConstants.hpp"
#include "LayerInterfaces.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace Tarcog {

  CIGUSolidLayer::CIGUSolidLayer( double const t_Thickness, double const t_Conductivity, 
    shared_ptr< CSurface > t_FrontSurface, shared_ptr< CSurface > t_BackSurface )
    : CState(), CBaseIGULayer( t_Thickness ), m_Conductivity( t_Conductivity ), 
    m_SolarAbsorptance( 0 ) {
    if ( t_FrontSurface != nullptr && t_BackSurface != nullptr ) {
      m_Surface[ Side::Front ] = t_FrontSurface;
      m_Surface[ Side::Back ] = t_BackSurface;
    } else {
      m_Surface[ Side::Front ] = make_shared< CSurface >();
      m_Surface[ Side::Back ] = make_shared< CSurface >();
    }
  }

  CIGUSolidLayer::CIGUSolidLayer( double const t_Thickness, double const t_Conductivity,
    double const t_FrontEmissivity, double const t_FrontIRTransmittance,
    double const t_BackEmissivity, double const t_BackIRTransmittance ) : CState(), 
    CBaseIGULayer( t_Thickness ), m_Conductivity( t_Conductivity ), m_SolarAbsorptance( 0 ) {
    m_Surface[ Side::Front ] = make_shared< CSurface >( t_FrontEmissivity, t_FrontIRTransmittance );
    m_Surface[ Side::Back ] = make_shared< CSurface >( t_BackEmissivity, t_BackIRTransmittance );
  }

  CIGUSolidLayer::CIGUSolidLayer( const CIGUSolidLayer& t_Layer ) : 
		CState( t_Layer ), CBaseIGULayer( t_Layer ) {
    m_Conductivity = t_Layer.m_Conductivity;
    m_SolarAbsorptance = t_Layer.m_SolarAbsorptance;
  }

  void CIGUSolidLayer::connectToBackSide( const shared_ptr< CBaseLayer >& t_Layer ) {
    CBaseLayer::connectToBackSide( t_Layer );
    t_Layer->setSurface( m_Surface.at( Side::Back ), Side::Front );
  }

  double CIGUSolidLayer::getConductivity() const {
    return m_Conductivity;
  }

  void CIGUSolidLayer::calculateConvectionOrConductionFlow() {
    if ( m_Thickness == 0 ) {
      throw runtime_error( "Solid layer thickness is set to zero." );
    }

    m_ConductiveConvectiveCoeff = m_Conductivity / m_Thickness;

  }

  void CIGUSolidLayer::setLayerState( double const t_Tf, double const t_Tb, 
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

  void CIGUSolidLayer::setSurfaceState( double const t_Temperature, double const t_J, 
    Side const t_Position ) {
    shared_ptr< ISurface > aSurface = m_Surface.at( t_Position );
    aSurface->setTemperature( t_Temperature );
    aSurface->setJ( t_J );

    resetCalculated();
  }

  void CIGUSolidLayer::setSolarRadiation( double const t_SolarRadiation ) {
    m_LayerGainFlow = t_SolarRadiation * m_SolarAbsorptance;
    resetCalculated();
  }

  void CIGUSolidLayer::setSolarAbsorptance( double const t_SolarAbsorptance ) {
    m_SolarAbsorptance = t_SolarAbsorptance;
    resetCalculated();
  }

  shared_ptr< CBaseLayer > CIGUSolidLayer::clone() const {
    return make_shared< CIGUSolidLayer >( *this );
  }

}
