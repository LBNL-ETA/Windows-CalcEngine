#ifndef BASECELL_H
#define BASECELL_H

#include <memory>
#include <vector>

namespace FenestrationCommon {

  enum class Side;

}

namespace LayerOptics {

  class CMaterial;
  class CCellDescription;
  class CBeamDirection;
  
  // Handles optical layer "cell". Base behavior is to calculate specular (direct-direct) component of a light
  // beam. Inherit from this class when want to create new shading type.
  class CBaseCell {
  public:
    CBaseCell( std::shared_ptr< CMaterial > t_Material,
      std::shared_ptr< CCellDescription > t_CellDescription );

    // Direct to direct component of transmitted ray
    virtual double T_dir_dir( const FenestrationCommon::Side t_Side, 
      std::shared_ptr< const CBeamDirection > t_Direction );

    // Specularly reflected ray
    virtual double R_dir_dir( const FenestrationCommon::Side t_Side, 
      std::shared_ptr< const CBeamDirection > t_Direction );

    virtual std::shared_ptr< std::vector< double > > T_dir_dir_band( const FenestrationCommon::Side t_Side, 
      std::shared_ptr< const CBeamDirection > t_Direction );

    virtual std::shared_ptr< std::vector< double > > R_dir_dir_band( const FenestrationCommon::Side t_Side, 
      std::shared_ptr< const CBeamDirection > t_Direction );

    std::shared_ptr< std::vector< double > > getBandWavelengths() const;
    int getBandIndex( const double t_Wavelength );
    size_t getBandSize() const;

  protected:
    std::shared_ptr< CMaterial > m_Material;
    std::shared_ptr< CCellDescription > m_CellDescription;

  };
}

#endif