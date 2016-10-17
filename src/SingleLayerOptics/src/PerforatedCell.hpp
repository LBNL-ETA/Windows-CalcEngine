#ifndef PERFORATEDCELL_H
#define PERFORATEDCELL_H

#include <memory>

#include "UniformDiffuseCell.hpp"

namespace FenestrationCommon {

  enum class Side;

}

namespace SingleLayerOptics {

  class CCellDescription;

  class CPerforatedCell : public CUniformDiffuseCell {
  public:
    CPerforatedCell( const std::shared_ptr< CMaterialBand >& t_MaterialProperties, 
        const std::shared_ptr< CCellDescription >& t_Cell );

  };

}

#endif