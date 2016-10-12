#ifndef TARIGUVENTILATEDGAPLAYER_H
#define TARIGUVENTILATEDGAPLAYER_H

#include <memory>

#include "TarIGUGapLayer.hpp"

namespace Gasses {

  class CGas;

}

namespace Tarcog {
  
  class CTarIGUVentilatedGapLayer : public CTarIGUGapLayer {
  public:
    CTarIGUVentilatedGapLayer( const std::shared_ptr< CTarIGUGapLayer >& t_Layer );
    CTarIGUVentilatedGapLayer( const CTarIGUVentilatedGapLayer& t_Layer );

    virtual double layerTemperature();

    void setFlowGeometry( double const t_Atop, double const t_Abot, AirVerticalDirection const &t_Direction );
    void setFlowTemperatures( double const t_topTemp, double const t_botTemp,
      AirVerticalDirection const &t_Direction );
    void setFlowSpeed( double const t_speed );

    double getAirflowReferencePoint( double const t_GapTemperature );

    double bernoullyPressureTerm();
    double hagenPressureTerm();
    double pressureLossTerm();
    double betaCoeff();

    void smoothEnergyGain( double const qv1, double const qv2 );

  private:
    virtual void calculateConvectionOrConductionFlow();
    double characteristicHeight();
    double calcImpedance( double const t_A );
    void ventilatedFlow();

    std::shared_ptr< CTarIGUGapLayer > m_Layer;
    double m_inTemperature;
    double m_outTemperature;
    double m_Zin;
    double m_Zout;

    std::shared_ptr< Gases::CGas > m_ReferenceGas;
  };
  
}

#endif
