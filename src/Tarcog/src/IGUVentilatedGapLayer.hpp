#ifndef TARIGUVENTILATEDGAPLAYER_H
#define TARIGUVENTILATEDGAPLAYER_H

#include <memory>

#include "IGUGapLayer.hpp"

namespace Gasses {

  class CGas;

}

namespace Tarcog {
  
  class CIGUVentilatedGapLayer : public CIGUGapLayer {
  public:
    CIGUVentilatedGapLayer( const std::shared_ptr< CIGUGapLayer >& t_Layer );
    CIGUVentilatedGapLayer( const CIGUVentilatedGapLayer& t_Layer );

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

    std::shared_ptr< CIGUGapLayer > m_Layer;
    std::shared_ptr< Gases::CGas > m_ReferenceGas;

    double m_inTemperature;
    double m_outTemperature;
    double m_Zin;
    double m_Zout;

  };
  
}

#endif
