#ifndef TARCOGNODEINTERFACE_H
#define TARCOGNODEINTERFACE_H

#include <memory>
#include <list>

namespace Tarcog {
  
  class CLayerNode {
  public:
    CLayerNode();
    
    void tearDownConnections();
    std::shared_ptr< CLayerNode > getNextNode() const;
    std::shared_ptr< CLayerNode > getPreviousNode() const;

    void connectToBackSide( const std::shared_ptr< CLayerNode >& t_Node );
    void connectToFrontSide( const std::shared_ptr< CLayerNode >& t_Node );

  private:
    void nullifyNodes();

    std::shared_ptr< CLayerNode > m_PreviousNode;
    std::shared_ptr< CLayerNode > m_NextNode;

  };

  class CLayerNodes {
  public:
    CLayerNodes();
    ~CLayerNodes();

    void addToFront( const std::shared_ptr< CLayerNode>& t_Node );
    void addToBack( const std::shared_ptr< CLayerNode>& t_Node );
    void tearDownConnections();

    std::list< std::shared_ptr< CLayerNode > >::iterator iterator();

  protected:
    std::list< std::shared_ptr< CLayerNode > > m_Layers;
  };

  class CBaseLayer;

  class CThermalNode : CLayerNode {
  public:
    explicit CThermalNode( const std::shared_ptr< CBaseLayer >& t_HeatFlowLayer );

    std::shared_ptr< CBaseLayer > getHeatFlowLayer() const;
    void setHeatFlowLayer( const std::shared_ptr< CBaseLayer >& t_HeatFlowLayer );

  protected:
    std::shared_ptr< CBaseLayer > m_HeatFlowLayer;
  };

}

#endif