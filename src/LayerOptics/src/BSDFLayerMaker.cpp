#include <stdexcept>

#include "BSDFLayerMaker.hpp"
#include "UniformDiffuseCell.hpp"
#include "DirectionalDiffuseCell.hpp"
#include "UniformDiffuseBSDFLayer.hpp"
#include "DirectionalDiffuseBSDFLayer.hpp"
#include "CellDescription.hpp"
#include "SpecularCellDescription.hpp"
#include "SpecularCell.hpp"
#include "SpecularBSDFLayer.hpp"
#include "VenetianCellDescription.hpp"
#include "VenetianCell.hpp"
#include "PerforatedCellDescription.hpp"
#include "PerforatedCell.hpp"
#include "WovenCellDescription.hpp"
#include "WovenCell.hpp"

using namespace std;

namespace LayerOptics {
  
  CBSDFLayerMaker::CBSDFLayerMaker( shared_ptr< CMaterial > t_Material,
    shared_ptr< const CBSDFHemisphere > t_BSDF, shared_ptr< CCellDescription > t_Description, 
    const DistributionMethod t_Method ) {

    if( t_Material == nullptr ) {
      throw runtime_error("Material for BSDF layer must be defined.");
    }

    if( t_BSDF == nullptr ) {
      throw runtime_error("BSDF Definition for BSDF layer must be defined.");
    }

    // Specular BSDF layer is considered to be default. Default is used if t_Description is null pointer
    if( t_Description == nullptr ) {
      t_Description = make_shared< CSpecularCellDescription >();
    }

    // Specular cell
    if( dynamic_pointer_cast< CSpecularCellDescription >( t_Description ) != NULL ) {
      shared_ptr< CSpecularCell > aCell = make_shared< CSpecularCell >( t_Material, t_Description );
      m_Layer = make_shared< CSpecularBSDFLayer >( aCell, t_BSDF );
    }

    // Venetian cell
    if( dynamic_pointer_cast< CVenetianCellDescription >( t_Description ) != NULL ) {
      if( t_Method == DistributionMethod::UniformDiffuse ) {
        shared_ptr< CUniformDiffuseCell > aCell = make_shared< CVenetianCell >( t_Material, t_Description );
        m_Layer = make_shared< CUniformDiffuseBSDFLayer >( aCell, t_BSDF );
      } else {
        shared_ptr< CDirectionalDiffuseCell > aCell = make_shared< CVenetianCell >( t_Material, t_Description );
        m_Layer = make_shared< CDirectionalDiffuseBSDFLayer >( aCell, t_BSDF );
      }
    }

    // Perforated cell
    if( dynamic_pointer_cast< CPerforatedCellDescription >( t_Description ) != NULL ) {
      // Perforated shades do not work with directional diffuse algorithm
      shared_ptr< CUniformDiffuseCell > aCell = make_shared< CPerforatedCell >( t_Material, t_Description );
      m_Layer = make_shared< CUniformDiffuseBSDFLayer >( aCell, t_BSDF );
    }

    // Woven cell
    if( dynamic_pointer_cast< CWovenCellDescription >( t_Description ) != NULL ) {
      // Woven shades do not work with directional diffuse algorithm
      shared_ptr< CUniformDiffuseCell > aCell = make_shared< CWovenCell >( t_Material, t_Description );
      m_Layer = make_shared< CUniformDiffuseBSDFLayer >( aCell, t_BSDF );
    }

  }

  shared_ptr< CBSDFLayer > CBSDFLayerMaker::getLayer() const {
    return m_Layer; 
  }

}