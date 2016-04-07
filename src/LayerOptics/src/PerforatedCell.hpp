#ifndef PERFORATEDCELL_H
#define PERFORATEDCELL_H

#include <memory>

#include "UniformDiffuseCell.hpp"

namespace FenestrationCommon {

  enum class Side;

}

namespace LayerOptics {

  class CCellDescription;

  class CPerforatedCell : public CUniformDiffuseCell {
  public:
    CPerforatedCell( std::shared_ptr< CMaterial > t_MaterialProperties, 
        std::shared_ptr< CCellDescription > t_Cell );

  // protected:
  //   std::shared_ptr< CPerforatedCellDescription > getCellAsPerforated() const;
  };

}

#endif