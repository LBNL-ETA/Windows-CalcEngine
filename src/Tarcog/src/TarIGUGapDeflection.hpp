#ifndef TARIGUGAPLAYERDEFLECTION_H
#define TARIGUGAPLAYERDEFLECTION_H

#include <memory>

#include "TarIGUGapLayer.hpp"

namespace Tarcog {
  
  class CTarIGUGapLayerDeflection : public CTarIGUGapLayer {
  public:
    CTarIGUGapLayerDeflection( std::shared_ptr< CTarIGUGapLayer > t_GapLayer,
      const double t_Tini, const double t_Pini );

    double getPressure();

  private:
    // std::shared_ptr< CTarIGUGapLayer > m_GapLayer;

    // Windows is produced under given temperature and pressure.
    // That is used for deflection calculations.
    double m_Tini;
    double m_Pini;
  };
  
}

#endif