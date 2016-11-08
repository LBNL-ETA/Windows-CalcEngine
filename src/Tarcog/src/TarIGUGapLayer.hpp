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
    CTarIGUGapLayer( const CTarIGUGapLayer& t_Layer );

    void connectToBackSide( const std::shared_ptr< CBaseTarcogLayer >& t_Layer );

    virtual double layerTemperature();

    double averageTemperature();

    virtual double getPressure();

    virtual std::shared_ptr< CBaseTarcogLayer > clone() const;


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