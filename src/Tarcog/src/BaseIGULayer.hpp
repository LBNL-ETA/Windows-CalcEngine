#ifndef BASEIGUTARCOGLAYER_H
#define BASEIGUTARCOGLAYER_H

#include <memory>
#include "BaseLayer.hpp"

namespace FenestrationCommon {

  enum class Side;

}

namespace Tarcog {

  class CBaseIGULayer : public CBaseLayer {
  public:
    CBaseIGULayer( double const t_Thickness );
    CBaseIGULayer( const CBaseIGULayer& t_Layer );

    double getThickness() const;
    double getTemperature( const FenestrationCommon::Side t_Position ) const;
    double J( const FenestrationCommon::Side t_Position ) const;
    double getMaxDeflection() const;
    double getMeanDeflection() const;

    double getConductivity();

  protected:
    virtual double layerTemperature();

    double m_Thickness;
  };

}
#endif