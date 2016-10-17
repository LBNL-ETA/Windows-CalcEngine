#ifndef BASECELLDESCRIPTION_H
#define BASECELLDESCRIPTION_H

#include <memory>

namespace FenestrationCommon {

  enum class Side;

}

namespace SingleLayerOptics {

  class CBeamDirection;

  // Base class for cell description. It is used to describe direct-direct component of the cell without
  // using any of the materials.

  class CCellDescription {
  public:
    CCellDescription();

    virtual double T_dir_dir( const FenestrationCommon::Side t_Side, const CBeamDirection& t_Direction ) = 0;

    virtual double R_dir_dir( const FenestrationCommon::Side t_Side, const CBeamDirection& t_Direction );

  };
}

#endif