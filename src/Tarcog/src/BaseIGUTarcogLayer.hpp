#ifndef BASEIGUTARCOGLAYER_H
#define BASEIGUTARCOGLAYER_H

#include <memory>
#include "BaseTarcogLayer.hpp"

namespace Tarcog {

  enum class SurfacePosition;
  class CTarSurface;

  class CBaseIGUTarcogLayer : public CBaseTarcogLayer {
  public:
    explicit CBaseIGUTarcogLayer( double const t_Thickness );

    double getThickness() const;
    double getTemperature( const SurfacePosition t_Position ) const;
    double J( const SurfacePosition t_Position ) const;

  protected:
    virtual double layerTemperature();

    double m_Thickness;
  //private:
  //  void initialize();
  };

}
#endif