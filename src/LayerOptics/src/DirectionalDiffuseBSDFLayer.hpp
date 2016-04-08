#ifndef DIRECTIONALDIFFUSEBSDFLAYER_H
#define DIRECTIONALDIFFUSEBSDFLAYER_H

#include <memory>

#include "BSDFLayer.hpp"

namespace LayerOptics {

  class CDirectionalDiffuseCell;

  // All outgoing directions are calculated
  class CDirectionalDiffuseBSDFLayer : public CBSDFLayer {
  public:
    CDirectionalDiffuseBSDFLayer( std::shared_ptr< CDirectionalDiffuseCell > t_Cell, 
      std::shared_ptr< const CBSDFHemisphere > t_Hemisphere );

  protected:
    std::shared_ptr< CDirectionalDiffuseCell > cellAsDirectionalDiffuse() const;
    void calcDiffuseDistribution( const FenestrationCommon::Side aSide, 
      std::shared_ptr< const CBeamDirection > t_Direction,
      const size_t t_DirectionIndex );
    void calcDiffuseDistribution_wv( const FenestrationCommon::Side aSide,
      std::shared_ptr< const CBeamDirection > t_Direction,
      const size_t t_DirectionIndex );

  };

}

#endif