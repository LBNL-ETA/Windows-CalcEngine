
#include "CellDescription.hpp"
// #include "BeamDirection.hpp"

namespace SingleLayerOptics {

  CCellDescription::CCellDescription() {
    
  }

  double CCellDescription::R_dir_dir( const FenestrationCommon::Side, const CBeamDirection& ) {
    return 0; 
  }

}