#ifndef TARIGUGAPLAYER_H
#define TARIGUGAPLAYER_H

#include <memory>
#include "BaseIGUTarcogLayer.hpp"
#include "Gas.hpp"

namespace Gases {
  class CGas;
}

namespace Tarcog {

  double const ReferenceTemperature = 273.15;

  class CTarIGUGapLayer :
    public CBaseIGUTarcogLayer, public CGasLayer {
  public:
    CTarIGUGapLayer( double t_Thickness, double t_Pressure );
    CTarIGUGapLayer( double t_Thickness, double t_Pressure, const std::shared_ptr< Gases::CGas >& t_Gas );

    void connectToBackSide( const std::shared_ptr< CBaseTarcogLayer >& t_Layer );

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
    double averageTemperature();
    void smoothEnergy( double const qv1, double const qv2 );

    void setDeflectionProperties( const double t_Tini, const double t_Pini );
    double getPressure();

  protected:
    void initializeStateVariables();

  private:
    void calculateConvectionConductionState();
    double calculateRayleighNumber();
    double aspectRatio();
    double convectiveH();
    void ventilatedFlow();

    double getGasTemperature();

    double characteristicHeight();

    double calcImpedance( double const t_A );

    void checkNextLayer();

    double m_inTemperature;
    double m_outTemperature;
    double m_Zin;
    double m_Zout;

    std::shared_ptr< Gases::CGas > m_ReferenceGas;

    double m_Tini;
    double m_Pini;
  };
 
}

#endif