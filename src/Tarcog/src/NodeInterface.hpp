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

    void connectToBackSide( std::shared_ptr< CLayerNode> t_Node );
    void connectToFrontSide( std::shared_ptr< CLayerNode > t_Node );

  private:
    void nullifyNodes();

    std::shared_ptr< CLayerNode > m_PreviousNode;
    std::shared_ptr< CLayerNode > m_NextNode;

  };

  class CLayerNodes {
  public:
    CLayerNodes();
    ~CLayerNodes();

    void addToFront( std::shared_ptr< CLayerNode> t_Node );
    void addToBack( std::shared_ptr< CLayerNode> t_Node );
    void tearDownConnections();

    std::list< std::shared_ptr< CLayerNode > >::iterator iterator();

  protected:
    std::list< std::shared_ptr< CLayerNode > > m_Layers;
  };

  class CBaseTarcogLayer;

  class CThermalNode : CLayerNode {
  public:
    CThermalNode( std::shared_ptr< CBaseTarcogLayer > const t_HeatFlowLayer );

    std::shared_ptr< CBaseTarcogLayer > getHeatFlowLayer() const;
    void setHeatFlowLayer( std::shared_ptr< CBaseTarcogLayer > t_HeatFlowLayer );

  protected:
    std::shared_ptr< CBaseTarcogLayer > m_HeatFlowLayer;
  };

}

#endif