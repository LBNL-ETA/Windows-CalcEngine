#include "SpecularCellDescription.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace SingleLayerOptics {

  CSpecularCellDescription::CSpecularCellDescription() {
    
  }

  double CSpecularCellDescription::T_dir_dir( const Side, const CBeamDirection& ) {
   return 0;
  }

  double CSpecularCellDescription::Rspecular( const Side, const CBeamDirection& ) {
   return 0;
  }

}