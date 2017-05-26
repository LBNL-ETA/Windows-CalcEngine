#include <assert.h>

#include "BaseLayer.hpp"
#include "TarcogConstants.hpp"
#include "Surface.hpp"

using namespace std;

namespace Tarcog {

  CBaseLayer::CBaseLayer() : CLayerGeometry(), CLayerHeatFlow(), 
    m_PreviousLayer( nullptr ), m_NextLayer( nullptr ) {
  
  }

  CBaseLayer::CBaseLayer( const CBaseLayer& t_Layer ) : 
    CLayerGeometry( t_Layer ), CLayerHeatFlow( t_Layer ) {
    // if( t_Layer.m_PreviousLayer != nullptr ) {
    //   m_PreviousLayer = t_Layer.m_PreviousLayer->clone();
    // }
    // if( t_Layer.m_NextLayer != nullptr ) {
    //   m_NextLayer = t_Layer.m_NextLayer->clone();
    // }
  }

  shared_ptr< CBaseLayer > CBaseLayer::getPreviousLayer() const {
    return m_PreviousLayer;
  }

  shared_ptr< CBaseLayer > CBaseLayer::getNextLayer() const {
    return m_NextLayer;
  }

  void CBaseLayer::tearDownConnections() {
    m_PreviousLayer = nullptr;
    m_NextLayer = nullptr;
  }

  void CBaseLayer::connectToBackSide( const shared_ptr< CBaseLayer >& t_Layer ) {
    m_NextLayer = t_Layer;
    t_Layer->m_PreviousLayer = shared_from_this();
  }

  void CBaseLayer::calculateRadiationFlow() {
    
  }

}
