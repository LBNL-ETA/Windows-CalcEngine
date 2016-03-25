#include <assert.h>

#include "DirectionalDiffuseCell.hpp"
#include "MaterialDescription.hpp"

using namespace std;

namespace LayerOptics {

  CDirectionalDiffuseCell::CDirectionalDiffuseCell( std::shared_ptr< CMaterial > t_MaterialProperties,
    std::shared_ptr< CBaseCellDescription > t_Cell ) : CBaseCell( t_MaterialProperties, t_Cell ) {

  };

}