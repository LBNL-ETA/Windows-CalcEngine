#include <stdexcept>

#include "OpticalLayer.hpp"
#include "LayerSingleComponent.hpp"
#include "OpticalSurface.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace LayerOptics;
using namespace FenestrationCommon;

namespace LayerOptics {

  CLayer::CLayer( shared_ptr< CScatteringSurface > t_Front, shared_ptr< CScatteringSurface > t_Back ) {
    if( t_Front == nullptr ) {
      throw runtime_error("Front surface must be created.");
    }
    if( t_Back == nullptr ) {
      throw runtime_error("Back surface must be created.");
    }

    m_Surface[ Side::Front ] = t_Front;
    m_Surface[ Side::Back ] = t_Back;
  }

  CLayer::CLayer( std::shared_ptr< const CLayer > t_Layer ) {
    shared_ptr< CScatteringSurface > aFront = t_Layer->getSurface( Side::Front );
    shared_ptr< CScatteringSurface > aBack = t_Layer->getSurface( Side::Back );

    m_Surface[ Side::Front ] = aFront;
    m_Surface[ Side::Back ] = aBack;

  }

  shared_ptr< CScatteringSurface > CLayer::getSurface( const Side t_Side ) const {
    return m_Surface.at( t_Side );
  }

  double CLayer::getProperty( const PropertySimple t_Property, const Side t_Side, const Scattering t_Scattering ) const {
    shared_ptr< CScatteringSurface > aSurface = getSurface( t_Side );
    return aSurface->getPropertySimple( t_Property, t_Scattering );
  }

  shared_ptr< CLayerSingleComponent > CLayer::getLayer( const Scattering t_Scattering ) const {
    double Tf = getProperty( PropertySimple::T, Side::Front, t_Scattering );
    double Rf = getProperty( PropertySimple::R, Side::Front, t_Scattering );
    double Tb = getProperty( PropertySimple::T, Side::Back, t_Scattering );
    double Rb = getProperty( PropertySimple::R, Side::Back, t_Scattering );
    shared_ptr< CLayerSingleComponent > aLayer = make_shared< CLayerSingleComponent >( Tf, Rf, Tb, Rb );
    return aLayer;
  }

}