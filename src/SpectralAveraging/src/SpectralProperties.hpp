#ifndef SPECTRALPROPERTIES_H
#define SPECTRALPROPERTIES_H

#include <vector>
#include <memory>

namespace SpectralAveraging {

  // Interface definition for any spectral property. This can be any property that is depending on wavelength 
  // (solar intensity, reflectance, transmittance, etc)
  class ISpectralProperty {
  public:
    virtual double wavelength() const = 0;
    virtual double value() const = 0;
    virtual void value( double const t_Value ) = 0;
  };

  // Implementation of spectral property interface
  class CSpectralProperty : public ISpectralProperty {
  public:
    CSpectralProperty();
    CSpectralProperty( double t_Wavelength, double t_Value );
    double wavelength() const;
    double value() const;
    virtual void value( double const t_Value );
    CSpectralProperty& operator=( const CSpectralProperty& t_Property );

  private:
    double m_Wavelength;
    double m_Value;
  };

  enum class IntegrationType;

  // Spectral properties for certain range of data. It holds common behavior like integration and interpolation
  // over certain range of data.
  class CSpectralProperties : public std::enable_shared_from_this< CSpectralProperties > {
  public:
    CSpectralProperties();
    void addProperty( double t_Wavelength, double t_Value );
    void insertToBeginning( double t_Wavelength, double t_Value );

    // Create wavelength array with identical values over entire wavelength spectrum
    void setConstantValues( std::shared_ptr< std::vector< double > > t_Wavelengths, double const t_Value );

    std::shared_ptr< CSpectralProperties > integrate( IntegrationType t_IntegrationType ) const;
    std::shared_ptr< CSpectralProperties > 
      interpolate( std::shared_ptr< std::vector< double > > t_Wavelengths ) const;

    // Multiplication of values in spectral properties that have same wavelength. Function will work only
    // if two spectral properties have identical wavelengths. Otherwise runtime error will be thrown.
    // If two spectral properites do not have same wavelength range, then interpolation function should be called.
    std::shared_ptr< CSpectralProperties > 
      mMult( std::shared_ptr< const CSpectralProperties > t_SpectralProperties ) const;
    
    // Substraction of values in spectral properties that have same wavelength. Function will work only
    // if two spectral properties have identical wavelengths. Otherwise runtime error will be thrown.
    // If two spectral properites do not have same wavelength range, then interpolation function should be called.
    std::shared_ptr< CSpectralProperties > 
      mSub( std::shared_ptr< const CSpectralProperties > t_SpectralProperties ) const;
    
    // Addition of values in spectral properties that have same wavelength. Function will work only
    // if two spectral properties have identical wavelengths. Otherwise runtime error will be thrown.
    // If two spectral properites do not have same wavelength range, then interpolation function should be called.
    std::shared_ptr< CSpectralProperties > 
      mAdd( std::shared_ptr< const CSpectralProperties > t_SpectralProperties ) const;
    
    // Return wavelenght values for spectral properties.
    std::shared_ptr< std::vector< double > > getWavelengths() const;

    // Sum of all properties between two wavelenghts. If default wavelength arguments are used then sum
    // will be performed over every spectral data
    double sum( double const minLambda = 0, double const maxLambda = 0 ) const;

    std::vector< std::shared_ptr< CSpectralProperty > >::const_iterator begin() const;
    std::vector< std::shared_ptr< CSpectralProperty > >::const_iterator end() const;
    size_t size() const;
    std::shared_ptr< const CSpectralProperty > operator[]( size_t Index ) const;
    std::shared_ptr< CSpectralProperties > operator=( CSpectralProperties& t_Properties );

    void clear();

  private:
    std::shared_ptr< ISpectralProperty > findLower( double const t_Wavelength ) const;
    std::shared_ptr< ISpectralProperty > findUpper( double const t_Wavelength ) const;
    double interpolate( std::shared_ptr< const ISpectralProperty > t_Lower, 
      std::shared_ptr< const ISpectralProperty > t_Upper, double const t_Wavelength ) const;

    std::vector< std::shared_ptr< CSpectralProperty > > m_SpectralProperties;

  };

}

#endif