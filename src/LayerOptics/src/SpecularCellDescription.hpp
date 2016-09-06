#ifndef SPECULARCELLDESCRIPTION_H
#define SPECULARCELLDESCRIPTION_H

#include "CellDescription.hpp"

namespace FenestrationCommon {

  enum class Side;
  enum class SpecularMaterialType;

}

namespace LayerOptics {

  class CSpecularCellDescription : public CCellDescription {
  public:
    CSpecularCellDescription();
    
    double T_dir_dir( const FenestrationCommon::Side t_Side, const CBeamDirection& t_Direction );
    double Rspecular( const FenestrationCommon::Side t_Side, 
      const std::shared_ptr< const CBeamDirection >& t_Direction );

  };

}

#endif