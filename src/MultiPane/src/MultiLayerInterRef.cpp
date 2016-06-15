#include <assert.h>

#include "MultiLayerInterRef.hpp"
#include "OpticalLayer.hpp"

using namespace std;
using namespace FenestrationCommon;
using namespace LayerOptics;

namespace MultiPane {

  CInterRef::CInterRef( shared_ptr< CLayer > t_Layer ) : m_StateCalculated( false ) {
    m_Layers.push_back( t_Layer );
  }

  void CInterRef::addLayer( shared_ptr< CLayer > t_Layer, const Side t_Side ) {
    switch( t_Side ) {
    case Side::Front:
      m_Layers.insert( m_Layers.begin(), t_Layer );
      break;
    case Side::Back:
      m_Layers.push_back( t_Layer );
      break;
    default:
      assert( "Impossible side selection when adding new layer." );
      break;
    }
    m_StateCalculated = false;
  }

  double CInterRef::getEnergyToSurface( const size_t Index, const Side t_SurfaceSide,
    const Side t_EnergySide, const Scattering t_Scattering ) {
    return 0;
  }

}