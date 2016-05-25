#include <stdexcept>
#include <assert.h>

#include "LayerSingleComponent.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace LayerOptics {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Surface
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  Surface::Surface( const double t_T, const double t_R ) : T( t_T ), R( t_R ), A( 1 - t_T - t_R ) {
    if( T + R > 1 ) {
      throw runtime_error("Sum of transmittance and reflectance is greater than one.");
    }
  }

  double Surface::getProperty( const Property t_Property ) {
    double aProperty = 0;
    switch (t_Property) {
    case Property::T:
      aProperty = T;
      break;
    case Property::R:
      aProperty = R;
      break;
    case Property::Abs:
      aProperty = A;
      break;
    default:
      assert("Impossible selection of surface property.");
      break;
    }
    return aProperty;
  }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLayerSingleComponent
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  CLayerSingleComponent::CLayerSingleComponent( const double t_Tf, const double t_Rf, const double t_Tb, const double t_Rb ) :
    m_Front( nullptr ), m_Back( nullptr ) {
    m_Front = make_shared< Surface >( t_Tf, t_Rf );
    m_Back = make_shared< Surface >( t_Tb, t_Rb );
  }

  double CLayerSingleComponent::getProperty( const Property t_Property, const Side t_Side ) const {
    shared_ptr< Surface > aSurface = getSurface( t_Side );
    assert( aSurface != nullptr );
    return aSurface->getProperty( t_Property );
  }

  shared_ptr< Surface > CLayerSingleComponent::getSurface( const Side t_Side ) const {
    shared_ptr< Surface > aSurface = nullptr;
    switch( t_Side ) {
    case Side::Front:
      aSurface = m_Front;
      break;
    case Side::Back:
      aSurface = m_Back;
      break;
    default:
      assert("Impossible selection of surface.");
      break;
    }
    return aSurface;
  }

}