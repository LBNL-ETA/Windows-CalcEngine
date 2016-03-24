#include "NodeInterface.hpp"
#include "BaseTarcogLayer.hpp"

using namespace std;

namespace Tarcog {

  //////////////////////////////////////////////////////////////////////////
  //      CLayerNode
  //////////////////////////////////////////////////////////////////////////

  CLayerNode::CLayerNode() {
    nullifyNodes();
  }

  void CLayerNode::tearDownConnections() {
    nullifyNodes();
  }

  void CLayerNode::nullifyNodes() {
    m_PreviousNode = nullptr;
    m_NextNode = nullptr;
  }

  shared_ptr< CLayerNode > CLayerNode::getNextNode() const {
    return m_NextNode;
  }

  shared_ptr< CLayerNode > CLayerNode::getPreviousNode() const {
    return m_PreviousNode;
  }

  void CLayerNode::connectToBackSide( shared_ptr< CLayerNode > t_Node ) {
    m_PreviousNode = t_Node->m_NextNode;
  }

  void CLayerNode::connectToFrontSide( shared_ptr< CLayerNode > t_Node ) {
    m_NextNode = t_Node->m_PreviousNode;
  }

  //////////////////////////////////////////////////////////////////////////
  //      CLayerNodes
  //////////////////////////////////////////////////////////////////////////

  CLayerNodes::CLayerNodes() {}

  void CLayerNodes::addToFront( shared_ptr< CLayerNode > t_Node ) {
    shared_ptr< CLayerNode > aNode = m_Layers.front();
    m_Layers.push_front( t_Node );
    if( aNode != nullptr ) {
      aNode->connectToFrontSide( t_Node );
      t_Node->connectToBackSide( aNode );
    }
  };

  void CLayerNodes::addToBack( shared_ptr< CLayerNode > t_Node ) {
    shared_ptr< CLayerNode > aNode = m_Layers.front();
    m_Layers.push_back( t_Node );
    if( aNode != nullptr ) {
      aNode->connectToBackSide( t_Node );
      t_Node->connectToFrontSide( aNode );
    }
  };

  void CLayerNodes::tearDownConnections() {
    for( shared_ptr< CLayerNode > layer : m_Layers  ) {
      layer->tearDownConnections();
    }
  };

  list< shared_ptr< CLayerNode > >::iterator CLayerNodes::iterator() {
    return m_Layers.begin();
  }

  //////////////////////////////////////////////////////////////////////////
  //      CThermalNode
  //////////////////////////////////////////////////////////////////////////

  CThermalNode::CThermalNode( shared_ptr< CBaseTarcogLayer > t_HeatFlowLayer ) :
    m_HeatFlowLayer( t_HeatFlowLayer ) {

  };

  shared_ptr< CBaseTarcogLayer > CThermalNode::getHeatFlowLayer() const {
    return m_HeatFlowLayer;
  };

  void CThermalNode::setHeatFlowLayer( shared_ptr< CBaseTarcogLayer > const t_Layer ) {
    m_HeatFlowLayer = t_Layer;
  };

}