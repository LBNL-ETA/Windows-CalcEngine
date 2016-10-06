#ifndef BASETARCOGLAYER_H
#define BASETARCOGLAYER_H

#include "LayerInterfaces.hpp"

#include <memory>

namespace FenestrationCommon {
  enum class Side;
}

namespace Tarcog {

  class CTarSurface;

  // Base description for any tarcog layer. This includes interior and exterior environments as well.
  // It must contain base definition of 2D geometry (Width and Height) and definition of heat flow
  // that is divided in three categories (convection, conduction and radiation). Every layer can
  // contain only Conduction + Radiation or Convection + Radiation.
  class CBaseTarcogLayer : public CLayerGeometry, public CLayerHeatFlow,
    public std::enable_shared_from_this< CBaseTarcogLayer > {
  public:
    CBaseTarcogLayer();
    CBaseTarcogLayer( const CBaseTarcogLayer& t_Layer );
    
    std::shared_ptr< CBaseTarcogLayer > getPreviousLayer() const;
    std::shared_ptr< CBaseTarcogLayer > getNextLayer() const;

    virtual void connectToBackSide( const std::shared_ptr< CBaseTarcogLayer >& t_Layer );

    void tearDownConnections();

  protected:
    virtual void calculateRadiationState();
    virtual void calculateConvectionConductionState();

    std::shared_ptr< CBaseTarcogLayer > m_PreviousLayer;
    std::shared_ptr< CBaseTarcogLayer > m_NextLayer;

  };

}

#endif