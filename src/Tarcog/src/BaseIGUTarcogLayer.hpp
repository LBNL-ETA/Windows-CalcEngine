#ifndef BASEIGUTARCOGLAYER_H
#define BASEIGUTARCOGLAYER_H

#include <memory>
#include "BaseTarcogLayer.hpp"

namespace FenestrationCommon {

  enum class Side;

}

namespace Tarcog {

  class CTarSurface;

  class CBaseIGUTarcogLayer : public CBaseTarcogLayer {
  public:
    explicit CBaseIGUTarcogLayer( double const t_Thickness );

    double getThickness() const;
    double getTemperature( const FenestrationCommon::Side t_Position ) const;
    double J( const FenestrationCommon::Side t_Position ) const;

  protected:
    virtual double layerTemperature();

    double m_Thickness;
  };

}
#endif