#include <assert.h>

#include "SpecularCell.hpp"
#include "MaterialDescription.hpp"
#include "SpecularCellDescription.hpp"
#include "SpectralSample.hpp"
#include "BeamDirection.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;
using namespace SpectralAveraging;

namespace LayerOptics {

  CSpecularCell::CSpecularCell( shared_ptr< CMaterialBand > t_MaterialProperties,
    shared_ptr< CCellDescription > t_Cell ) : CBaseCell( t_MaterialProperties, t_Cell ) {

  }

  double CSpecularCell::T_dir_dir( const Side t_Side, shared_ptr< const CBeamDirection > t_Direction ) {
    // Specular glass transmittance is same for front and back sides
    return m_Material->getPropertyAtAngle( Property::T, t_Side, t_Direction->theta() );
  }

  double CSpecularCell::R_dir_dir( const Side t_Side, shared_ptr< const CBeamDirection > t_Direction ) {
    return m_Material->getPropertyAtAngle( Property::R, t_Side, t_Direction->theta() );
  }

  // Transmittance of specular material for each of material range. In case of sample measured material, that are
  // properties at each wavelength. In case of some other material type, that will be properties for each of material
  // ranges
  shared_ptr< vector< double > > CSpecularCell::T_dir_dir_band( const Side t_Side, 
    shared_ptr< const CBeamDirection > t_Direction ) {
    return m_Material->getBandPropertiesAtAngle( Property::T, t_Side, t_Direction->theta() );
  }

  // Reflectance of specular material over entire material range
  shared_ptr< vector< double > > CSpecularCell::R_dir_dir_band( const Side t_Side, 
    shared_ptr< const CBeamDirection > t_Direction ) {
    return m_Material->getBandPropertiesAtAngle( Property::R, t_Side, t_Direction->theta() );
  }

  shared_ptr< CSpecularCellDescription > CSpecularCell::getCellAsSpecular() const {
    if( dynamic_pointer_cast< CSpecularCellDescription >( m_CellDescription ) == NULL ) {
      assert("Incorrectly assigned cell description.");
    }

    shared_ptr< CSpecularCellDescription > aCell = 
      dynamic_pointer_cast< CSpecularCellDescription >( m_CellDescription );

    return aCell;
  }

}