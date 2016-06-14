#include <stdexcept>

#include "OpticalLayer.hpp"
#include "OpticalSurface.hpp"
#include "FenestrationCommon.hpp"

using namespace std;

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

  shared_ptr< CScatteringSurface > CLayer::getSurface( const Side t_Side ) const {
    return m_Surface.at( t_Side );
  }

  double CLayer::getProperty( const PropertySimple t_Property, const Side t_Side, const Scattering t_Scattering ) {
    shared_ptr< CScatteringSurface > aSurface = getSurface( t_Side );
    return aSurface->getPropertySimple( t_Property, t_Scattering );
  }

}