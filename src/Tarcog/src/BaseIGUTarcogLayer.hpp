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
    CBaseIGUTarcogLayer( double const t_Thickness );
    CBaseIGUTarcogLayer( const CBaseIGUTarcogLayer& t_Layer );

    double getThickness() const;
    double getTemperature( const FenestrationCommon::Side t_Position ) const;
    double J( const FenestrationCommon::Side t_Position ) const;

    double getConductivity();

  protected:
    virtual double layerTemperature();

    double m_Thickness;
  };

}
#endif