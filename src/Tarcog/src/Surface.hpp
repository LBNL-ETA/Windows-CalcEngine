#ifndef SURFACE_H
#define SURFACE_H

#include <memory>
#include <vector>

namespace Tarcog {

  class ISurface {
  public:
    ISurface();
    ISurface( const double t_Emissivity, const double t_Transmittance );

    ISurface( const ISurface& t_Surface );

    virtual std::shared_ptr< ISurface > clone() const = 0;

    virtual void setTemperature( double const t_Temperature ) = 0;
    virtual void setJ( double const t_J ) = 0;
    virtual void applyDeflection( const double t_MeanDeflection, const double t_MaxDeflection ) = 0;

    double getTemperature() const;
    double getEmissivity() const;
    double getReflectance() const;
    double getTransmittance() const;
    double J() const;
    double emissivePowerTerm() const;
    double getMeanDeflection() const;
    double getMaxDeflection() const;

    void initializeStart( double const t_Temperature );
    void initializeStart( const double t_Temperature, const double t_Radiation );

  protected:
    double m_Temperature;
    double m_J;

    double m_Emissivity;
    double m_Reflectance;
    double m_Transmittance;

    // Value for deflection. Positive deflection is surface curved towards left side and 
    // negative deflection vice-versa.
    double m_MeanDeflection;
    double m_MaxDeflection;

  private:
    void calculateReflectance();

  };

  class CSurface : public ISurface {
  public:
    CSurface();
    CSurface( double t_Emissivity, double t_Transmittance );
    CSurface( const CSurface& t_Surface );

    virtual std::shared_ptr< ISurface > clone() const;

    void setTemperature( double const t_Temperature );
    void setJ( double const t_J );

    //  Applies deflection to current surface
    void applyDeflection( const double t_MeanDeflection, const double t_MaxDeflection );

  };

}

#endif