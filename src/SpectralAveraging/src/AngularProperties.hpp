#ifndef ANGULARPROPERTIES_H
#define ANGULARPROPERTIES_H

#include <memory>

namespace FenestrationCommon {

  enum class SurfaceType;

}

namespace SpectralAveraging {

  // Calculates angular propertes at given angle
  class CAngularProperties {
  public:
    CAngularProperties( const double t_TransmittanceZero, const double t_ReflectanceZero );

    virtual double transmittance( const double t_Angle, const double t_Wavelength = 0 ) = 0;
    virtual double reflectance( const double t_Angle, const double t_Wavelength = 0 ) = 0;

  protected:
    double cosAngle( const double t_Angle );
    virtual void checkStateProperties( const double t_Angle, const double t_Wavelength );

    double m_Transmittance0;
    double m_Reflectance0;

    double m_Transmittance;
    double m_Reflectance;
    double m_StateAngle;
    double m_StateWavelength;
  };

  // Angular properties for uncoated type of glass layer
  class CAngularPropertiesUncoated : public CAngularProperties {
  public:
    CAngularPropertiesUncoated( const double t_Thicknes, 
      const double t_TransmittanceZero, const double t_ReflectanceZero );

    double transmittance( const double t_Angle, const double t_Wavelength );
    double reflectance( const double t_Angle, const double t_Wavelength );

  protected:
    void checkStateProperties( const double t_Angle, const double t_Wavelength );

  private:
    double m_Thickness;
    double m_Beta;
    double m_Rho0;

  };

  class CAngularPropertiesCoated : public CAngularProperties {
  public:
    CAngularPropertiesCoated( const double t_Transmittance, const double t_Reflectance, 
      const double t_SolTransmittance0 );

    double transmittance( const double t_Angle, const double t_Wavelength = 0 );
    double reflectance( const double t_Angle, const double t_Wavelength = 0 );

  protected:
    void checkStateProperties( const double t_Angle, const double t_Wavelength );

    double m_SolTransmittance0;
  };

  enum class CoatingProperty { T, R };
  enum class CoatingType { Clear, Bronze };

  class Coefficients {
  public:
    Coefficients( const double t_C0, const double t_C1, const double t_C2, const double t_C3,
      const double t_C4 );
    double inerpolation( const double t_Value );

  private:
    double C0;
    double C1;
    double C2;
    double C3;
    double C4;
  };

  // creates coating coefficients according to property and type.
  class CCoatingCoefficients {
  public:
    CCoatingCoefficients();
    std::shared_ptr< Coefficients > getCoefficients( const CoatingProperty t_Property, const CoatingType t_Type );
  };

  class CAngularPropertiesFactory {
  public:
    CAngularPropertiesFactory( const double t_Transmittance0, const double t_Reflectance0, 
      const double t_Thickness = 0, const double t_SolarTransmittance = 0 );

    std::shared_ptr< CAngularProperties > 
      getAngularProperties( const FenestrationCommon::SurfaceType t_SurfaceType );

  private:
    double m_Thickness;
    double m_Transmittance0;
    double m_Reflectance0;
    double m_SolarTransmittance0;
  };

}

#endif