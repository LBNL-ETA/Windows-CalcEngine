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

    // set active state to calculate deflection
    virtual void setDeflectionProperties( const double t_Tini, const double t_Pini );

  protected:
    virtual double layerTemperature();

    double m_Thickness;
    bool m_CalcDeflection;
  };

}
#endif