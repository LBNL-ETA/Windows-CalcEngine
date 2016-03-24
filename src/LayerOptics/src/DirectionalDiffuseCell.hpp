#ifndef DIRECTIONALDIFFUSECELL_H
#define DIRECTIONALDIFFUSECELL_H

#include <memory>
#include <vector>

#include "BaseCell.hpp"

namespace FenestrationCommon {

  enum class Side;

}

namespace LayerOptics {

  class CBaseCellDescription;
  class CBeamDirection;
  class CMaterial;

  class CDirectionalDiffuseCell : public virtual CBaseCell {
  public:
    CDirectionalDiffuseCell( std::shared_ptr< CMaterial > t_MaterialProperties,
      std::shared_ptr< CBaseCellDescription > t_Cell );

    virtual double T_dir_dif( const FenestrationCommon::Side t_Side, 
      std::shared_ptr< const CBeamDirection > t_IncomingDirection,
      std::shared_ptr< const CBeamDirection > t_OutgoingDirection ) = 0;

    virtual double R_dir_dif( const FenestrationCommon::Side t_Side, 
      std::shared_ptr< const CBeamDirection > t_IncomingDirection,
      std::shared_ptr< const CBeamDirection > t_OutgoingDirection ) = 0;

    virtual std::shared_ptr< std::vector< double > > T_dir_dif_band( const FenestrationCommon::Side t_Side, 
      std::shared_ptr< const CBeamDirection > t_IncomingDirection,
      std::shared_ptr< const CBeamDirection > t_OutgoingDirection ) = 0;

    virtual std::shared_ptr< std::vector< double > > R_dir_dif_band( const FenestrationCommon::Side t_Side, 
      std::shared_ptr< const CBeamDirection > t_IncomingDirection,
      std::shared_ptr< const CBeamDirection > t_OutgoingDirection ) = 0;

  };

}

#endif