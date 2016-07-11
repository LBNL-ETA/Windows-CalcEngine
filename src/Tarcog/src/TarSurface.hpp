#ifndef TARSURFACE_H
#define TARSURFACE_H

#include <memory>
#include <vector>

namespace Tarcog {

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
    void initializeStart( double const t_Temperature );
    void initializeStart( const double t_Temperature, const double t_Radiation );

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