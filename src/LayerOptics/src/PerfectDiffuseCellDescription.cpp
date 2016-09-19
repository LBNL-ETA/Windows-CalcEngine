#include "PerfectDiffuseCellDescription.hpp"

using namespace FenestrationCommon;

namespace LayerOptics {

  CPerfectDiffuseCellDescription::CPerfectDiffuseCellDescription() : CCellDescription() {
  
  }

  double CPerfectDiffuseCellDescription::T_dir_dir( const Side t_Side, const CBeamDirection& t_Direction ) {
    return 0;
  }

}