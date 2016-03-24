#ifndef TARSURFACE_H
#define TARSURFACE_H

#include <memory>
#include <vector>

namespace Tarcog {

  enum class SurfacePosition {Front, Back};
  class CBaseTarcogLayer;

  class CTarSurface
  {
  public:
    CTarSurface();
    CTarSurface( double t_Emissivity, double t_Transmittance );

    void setTemperature( double const t_Temperature );
    double getTemperature() const;
    double getEmissivity() const;
    double getReflectance() const;
    double getTransmittance() const;
    double J() const;
    void setJ( double const t_J );
    double emissivePowerTerm() const;
    void intializeStart( double const t_Temperature );

  private:
    void initialize();

    double m_Emissivity;
    double m_Reflectance;
    double m_Transmittance;
    double m_Temperature;
    double m_J;
  };

}

#endif