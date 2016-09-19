#ifndef PERFECTDIFFUSECELLDESRIPTION_H
#define PERFECTDIFFUSECELLDESRIPTION_H

#include "CellDescription.hpp"

namespace LayerOptics {
  
  // Cell descritpion that needs to be used for perfect diffusers. Specular components are
  // set to zero
  class CPerfectDiffuseCellDescription : public CCellDescription {
  public:
    CPerfectDiffuseCellDescription();
    
    double T_dir_dir( const FenestrationCommon::Side t_Side, const CBeamDirection& t_Direction );
      
  };
  
}

#endif