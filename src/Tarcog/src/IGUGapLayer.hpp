#ifndef TARIGUGAPLAYER_H
#define TARIGUGAPLAYER_H

#include <memory>
#include "BaseIGULayer.hpp"
#include "Gas.hpp"

namespace Gases {
  class CGas;
}

namespace Tarcog {

  double const ReferenceTemperature = 273.15;

  class CIGUGapLayer :
    public CBaseIGULayer, public CGasLayer {
  public:
    CIGUGapLayer( double t_Thickness, double t_Pressure );
    CIGUGapLayer( double t_Thickness, double t_Pressure, const std::shared_ptr< Gases::CGas >& t_Gas );
    CIGUGapLayer( const CIGUGapLayer& t_Layer );

    void connectToBackSide( const std::shared_ptr< CBaseLayer >& t_Layer );

    virtual double layerTemperature();

    double averageTemperature();

    virtual double getPressure();

    virtual std::shared_ptr< CBaseLayer > clone() const;


  protected:
    void initializeStateVariables();
    virtual void calculateConvectionOrConductionFlow();

  private:
    double calculateRayleighNumber();
    double aspectRatio();
    double convectiveH();

    double getGasTemperature();

    void checkNextLayer();

    

  };
 
}

#endif