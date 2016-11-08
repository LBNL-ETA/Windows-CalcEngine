#ifndef TARIGUSOLIDLAYER_H
#define TARIGUSOLIDLAYER_H

#include <memory>
#include "BaseIGUTarcogLayer.hpp"

namespace FenestrationCommon {
  enum class Side;
}

namespace Tarcog {

  class CTarSurface;

  class CTarIGUSolidLayer : public CBaseIGUTarcogLayer {
  public:
    CTarIGUSolidLayer( double const t_Thickness, double const t_Conductivity, 
      std::shared_ptr< CTarSurface > t_FrontSurface = nullptr, 
      std::shared_ptr< CTarSurface > t_BackSurface = nullptr );

    CTarIGUSolidLayer( double const t_Thickness, double const t_Conductivity,
      double const t_FrontEmissivity, double const t_FrontIRTransmittance,
      double const t_BackEmissivity, double const t_BackIRTransmittance );

    CTarIGUSolidLayer( const CTarIGUSolidLayer& t_Layer );

    void connectToBackSide( const std::shared_ptr< CBaseTarcogLayer >& t_Layer );

    double getConductivity() const;

    void setLayerState( double const t_Tf, double const t_Tb, double const t_Jf, double const t_Jb );
    void setSolarRadiation( double const t_SolarRadiation );
    void setSolarAbsorptance( double const t_SolarAbsorptance );

    virtual std::shared_ptr< CBaseTarcogLayer > clone() const;

  protected:
    virtual void calculateConvectionOrConductionFlow();

  private:
    void setSurfaceState( double const t_Temperature, double const t_J, 
      FenestrationCommon::Side const t_Position );

    double m_Conductivity;
    double m_SolarAbsorptance;

  };

}

#endif
