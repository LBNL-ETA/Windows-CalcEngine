#include "SpecularCellDescription.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace LayerOptics {

  CSpecularCellDescription::CSpecularCellDescription() {
    
  }

  double CSpecularCellDescription::T_dir_dir( const Side, const CBeamDirection& ) {
   return 0;
  }

  double CSpecularCellDescription::Rspecular( const Side, const shared_ptr< const CBeamDirection >& ) {
   return 0;
  }

}