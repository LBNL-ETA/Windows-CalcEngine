#ifndef LAYERINTERFACES_H
#define LAYERINTERFACES_H

#include <memory>
#include <map>

namespace FenestrationCommon {
  enum class Side;
}

namespace Gases {
  class CGas;
}

namespace Tarcog {

  class CTarSurface;

  struct ForcedVentilation {
    ForcedVentilation() : Speed( 0 ), Temperature( 0 ) {};
    ForcedVentilation( double t_Speed, double t_Temperature ) : 
      Speed( t_Speed ), Temperature( t_Temperature ) {};
    double Speed;
    double Temperature;
  };

  // class CLayerState is used to keep validity of object state. In case of thermal layers this class is used to keep information if state 
  // already have valid results with current input parameters
  class CLayerState {
  public:
    CLayerState();
    virtual void resetCalculated() final; // to reset state to non-calculated
    virtual void setCalculated() final; // calculations are up to date and set state to valid state
    virtual bool isCalculated() final; // check if state have valid results

  protected:
    virtual void initializeStateVariables() = 0; // some intermediate state variables need to pick up new input parameters.

  private:
    bool m_StateCalculated;
  };

  class CLayerGeometry : public virtual CLayerState {
  public:
    CLayerGeometry();
    virtual void setWidth( double const t_Width ) final;
    virtual void setHeight( double const t_Height ) final;
    virtual void setTilt( double const t_Tilt ) final;

  protected:
    double m_Width;
    double m_Height;
    double m_Tilt;
  };

  class CLayerHeatFlow : public virtual CLayerState {
  public:
    CLayerHeatFlow();
    CLayerHeatFlow( const std::shared_ptr< CTarSurface >& t_FrontSurface, 
      const std::shared_ptr< CTarSurface >& t_BackSurface );
    virtual double getHeatFlow() final;
    virtual double getGainFlow() final;
    virtual double getConductionConvectionCoefficient() final;
    virtual double getRadiationFlow() final;
    virtual double getConvectionConductionFlow() final;
    virtual std::shared_ptr< CTarSurface > getSurface( FenestrationCommon::Side const t_Position ) const final;
    virtual void setSurface( std::shared_ptr< CTarSurface > t_Surface, 
      FenestrationCommon::Side const t_Position ) final;
  protected:
    virtual void calculateLayerState() final;
    virtual void calculateRadiationState() = 0;
    virtual void calculateConvectionConductionState() = 0;

    std::map< FenestrationCommon::Side, std::shared_ptr< CTarSurface > > m_Surface;
    double m_ConductiveConvectiveCoeff;
    double m_LayerGainFlow;
  };

  enum class AirVerticalDirection { None, Up, Down };
  enum class AirHorizontalDirection { None, Leeward, Windward };

  class CGasLayer : public virtual CLayerState {
  public:
    CGasLayer();
    explicit CGasLayer( double const t_Pressure );
    CGasLayer( double const t_Pressure, double const t_AirSpeed, AirVerticalDirection const t_AirDirection );
    CGasLayer( double const t_Pressure, double const t_AirSpeed, AirHorizontalDirection const t_AirDirection );
    CGasLayer( double const t_Pressure, std::shared_ptr< Gases::CGas > t_Gas );

    virtual double getPressure();

    virtual double getGasTemperature() = 0;

  protected:
    void initializeStateVariables();

    double m_Pressure;
    double m_AirSpeed;
    AirVerticalDirection m_AirVerticalDirection;
    AirHorizontalDirection m_AirHorizontalDirection;
    ForcedVentilation m_ForcedVentilation;

    std::shared_ptr< Gases::CGas > m_Gas;

  private:
    void onCreate();
  };

}

#endif