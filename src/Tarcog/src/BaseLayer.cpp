#include "BaseLayer.hpp"
using namespace std;

namespace Tarcog {

	CBaseLayer::CBaseLayer() : CState(), CLayerGeometry(), CLayerHeatFlow(),
	                           m_PreviousLayer( nullptr ), m_NextLayer( nullptr ) {

	}

	CBaseLayer::CBaseLayer( CBaseLayer const& t_Layer ) :
		CState( t_Layer ), CLayerGeometry( t_Layer ), CLayerHeatFlow( t_Layer ) {

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

	void CBaseLayer::connectToBackSide( shared_ptr< CBaseLayer > const& t_Layer ) {
		m_NextLayer = t_Layer;
		t_Layer->m_PreviousLayer = shared_from_this();
	}

	void CBaseLayer::calculateRadiationFlow() {

	}

}
