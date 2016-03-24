#ifndef SPECULARBSDFLAYER_H
#define SPECULARBSDFLAYER_H

#include <memory>
#include <vector>

#include "BaseBSDFLayerMultiWL.hpp"

namespace LayerOptics {

  class CSpecularCell;

  // BSDF creation for specular layers.
  class CSpecularBSDFLayer : public CBaseBSDFLayerMultiWL {
  public:
    CSpecularBSDFLayer( std::shared_ptr< CSpecularCell > t_Cell, 
      std::shared_ptr< CBSDFHemisphere > t_Hemisphere );

  protected:
    std::shared_ptr< CSpecularCell > cellAsSpecular() const;
    void calcDiffuseDistribution( const FenestrationCommon::Side aSide, 
      std::shared_ptr< const CBeamDirection > t_Direction,
      const size_t t_DirectionIndex );
    void calcDiffuseDistribution_wv( const FenestrationCommon::Side aSide, 
      std::shared_ptr< const CBeamDirection > t_Direction,
      const size_t t_DirectionIndex );

  
  };

}

#endif