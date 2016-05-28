#ifndef UniformDiffuseCell_H
#define UniformDiffuseCell_H

#include <memory>
#include <vector>

#include "BaseCell.hpp"

namespace FenestrationCommon {

  enum class Side;
  enum class Property;

}

namespace LayerOptics {

  class CCellDescription;
  class CBeamDirection;
  class CMaterialBand;

  // Ray that hits uniformly diffusing cell material will distribute equally in every direction
  class CUniformDiffuseCell : public virtual CBaseCell {
  public:
    CUniformDiffuseCell( std::shared_ptr< CMaterialBand > t_MaterialProperties,
      std::shared_ptr< CCellDescription > t_Cell );

    // Direct to diffuse components
    virtual double T_dir_dif( const FenestrationCommon::Side t_Side, 
      std::shared_ptr< const CBeamDirection > t_Direction );

    virtual double R_dir_dif( const FenestrationCommon::Side t_Side, 
      std::shared_ptr< const CBeamDirection > t_Direction );

    // Property of the cell over the range
    virtual std::shared_ptr< std::vector< double > > T_dir_dif_band( const FenestrationCommon::Side t_Side, 
      std::shared_ptr< const CBeamDirection > t_Direction );

    virtual std::shared_ptr< std::vector< double > > R_dir_dif_band( const FenestrationCommon::Side t_Side, 
      std::shared_ptr< const CBeamDirection > t_Direction );

  private:
    double getMaterialProperty( const FenestrationCommon::Property t_Property, 
      const FenestrationCommon::Side t_Side, std::shared_ptr< const CBeamDirection > t_Direction );
    std::shared_ptr< std::vector< double > > getMaterialProperties( const FenestrationCommon::Property t_Property, 
      const FenestrationCommon::Side t_Side, std::shared_ptr< const CBeamDirection > t_Direction );
    
  };

}

#endif