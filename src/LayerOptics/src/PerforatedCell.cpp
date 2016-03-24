#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>

#include "PerforatedCell.hpp"
#include "PerforatedCellDescription.hpp"
#include "MaterialDescription.hpp"
#include "FenestrationCommon.hpp"
#include "BeamDirection.hpp"
#include "MathFunctions.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace LayerOptics {

  ////////////////////////////////////////////////////////////////////////////////////////////
  //  CPerforatedCell
  ////////////////////////////////////////////////////////////////////////////////////////////
  CPerforatedCell::CPerforatedCell( shared_ptr< CMaterial > t_MaterialProperties, 
    shared_ptr< CPerforatedCellDescription > t_Cell ) : 
    CUniformDiffuseCell( t_MaterialProperties, t_Cell ), CBaseCell( t_MaterialProperties, t_Cell ) {
    
  };

}