#include "PerfectDiffuseCellDescription.hpp"

using namespace FenestrationCommon;

namespace LayerOptics {

  CPerfectDiffuseCellDescription::CPerfectDiffuseCellDescription() : CCellDescription() {
  
  }

  double CPerfectDiffuseCellDescription::T_dir_dir( const Side, const CBeamDirection& ) {
    return 0;
  }

}