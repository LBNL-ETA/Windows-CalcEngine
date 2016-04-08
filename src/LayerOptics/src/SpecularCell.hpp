#ifndef SPECULARCELL_H
#define SPECULARCELL_H

#include <memory>
#include <vector>

#include "BaseCell.hpp"

namespace FenestrationCommon {

  enum class Side;

}

namespace LayerOptics {

  class CMaterialSample;
  class CSpecularCellDescription;
  class CCellDescription;
  class CBeamDirection;

  // Calculates spectral properties of specular layer over the given wavelength range and it also calculates
  // spectral properties over given wavelengths
  class CSpecularCell : public CBaseCell {
  public:
    CSpecularCell( std::shared_ptr< CMaterial > t_MaterialProperties,
      std::shared_ptr< CCellDescription > t_Cell );
    
    // Transmittance averaged over entire wavelength spectrum
    double T_dir_dir( const FenestrationCommon::Side t_Side,
      std::shared_ptr< const CBeamDirection > t_Direction );

    // Reflectance averaged over entire wavelength spectrum
    double R_dir_dir( const FenestrationCommon::Side t_Side,
      std::shared_ptr< const CBeamDirection > t_Direction );

    // Transmittance of specular material for enire wavelength spectrum
    std::shared_ptr< std::vector< double > > T_dir_dir_band( 
      const FenestrationCommon::Side t_Side,
      std::shared_ptr< const CBeamDirection > t_Direction );

    // Reflectance of specular material over entire wavelength spectrum
    std::shared_ptr< std::vector< double > > R_dir_dir_band( 
      const FenestrationCommon::Side t_Side,
      std::shared_ptr< const CBeamDirection > t_Direction );

  protected:
    std::shared_ptr< CSpecularCellDescription > getCellAsSpecular() const;

  };

}

#endif