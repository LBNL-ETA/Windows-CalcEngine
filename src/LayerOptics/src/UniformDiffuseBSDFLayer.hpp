#ifndef UniformDiffuseShade_H
#define UniformDiffuseShade_H

#include <memory>

#include "BSDFLayer.hpp"

namespace LayerOptics {

  class CUniformDiffuseCell;

  // All outgoing directions are uniformly distributed in all directions
  class CUniformDiffuseBSDFLayer : public CBSDFLayer {
  public:
    CUniformDiffuseBSDFLayer( std::shared_ptr< CUniformDiffuseCell > t_Cell, 
      std::shared_ptr< const CBSDFHemisphere > t_Hemisphere );

  protected:
    std::shared_ptr< CUniformDiffuseCell > cellAsUniformDiffuse() const;
    void calcDiffuseDistribution( const FenestrationCommon::Side aSide, 
      std::shared_ptr< const CBeamDirection > t_Direction,
      const size_t t_DirectionIndex );
    void calcDiffuseDistribution_wv( const FenestrationCommon::Side aSide, 
      std::shared_ptr< const CBeamDirection > t_Direction,
      const size_t t_DirectionIndex );
 
  };

}

#endif