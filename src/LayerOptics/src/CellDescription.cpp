
#include "CellDescription.hpp"
// #include "BeamDirection.hpp"

namespace LayerOptics {

  CCellDescription::CCellDescription() {
    
  }

  double CCellDescription::R_dir_dir( const FenestrationCommon::Side,
    std::shared_ptr< const CBeamDirection > ) {
    return 0; 
  }

}