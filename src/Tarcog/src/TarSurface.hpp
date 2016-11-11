#ifndef TARSURFACE_H
#define TARSURFACE_H

#include <memory>
#include <vector>

namespace Tarcog {

  class CTarSurface {
  public:
    CTarSurface();
    CTarSurface( double t_Emissivity, double t_Transmittance );
    CTarSurface( const CTarSurface& t_Surface );

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

    //  Applies deflection to current surface
    void applyDeflection( const double t_MeanDeflection, const double t_MaxDeflection );
    double getMeanDeflection() const;
    double getMaxDeflection() const;

  private:
    void initializeOptical();

    double m_Emissivity;
    double m_Reflectance;
    double m_Transmittance;
    double m_Temperature;
    double m_J;

    // Value for deflection. Positive deflection is surface curved towards left side and 
    // negative deflection vice-versa.
    double m_MeanDeflection;
    double m_MaxDeflection;
  };

}

#endif