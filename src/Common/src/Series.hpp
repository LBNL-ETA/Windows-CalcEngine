#ifndef SERIES_H
#define SERIES_H

#include <vector>
#include <memory>

namespace FenestrationCommon {

  // Interface definition for any spectral property. This can be any property that is depending on wavelength 
  // (solar intensity, reflectance, transmittance, etc)
  class ISeriesPoint {
  public:
    virtual double x() const = 0;
    virtual double value() const = 0;
    virtual void value( double const t_Value ) = 0;
  };

  // Implementation of spectral property interface
  class CSeriesPoint : public ISeriesPoint {
  public:
    CSeriesPoint();
    CSeriesPoint( double t_Wavelength, double t_Value );
    double x() const;
    double value() const;
    virtual void value( double const t_Value );
    CSeriesPoint& operator=( const CSeriesPoint& t_Point );
    bool operator<( const CSeriesPoint& t_Point );

  private:
    double m_x;
    double m_Value;
  };

  enum class IntegrationType;

  // Spectral properties for certain range of data. It holds common behavior like integration and interpolation
  // over certain range of data.
  class CSeries : public std::enable_shared_from_this< CSeries > {
  // class CSeries {
  public:
    CSeries();
    void addProperty( double t_x, double t_Value );
    void insertToBeginning( double t_x, double t_Value );

    // Create wavelength array with identical values over entire wavelength spectrum
    void setConstantValues( std::shared_ptr< std::vector< double > > t_x, double const t_Value );

    std::shared_ptr< CSeries > integrate( IntegrationType t_IntegrationType ) const;
    std::shared_ptr< CSeries > 
      interpolate( const std::shared_ptr< std::vector< double > >& t_x ) const;

    // Multiplication of values in spectral properties that have same wavelength. Function will work only
    // if two spectral properties have identical wavelengths. Otherwise runtime error will be thrown.
    // If two spectral properites do not have same wavelength range, then interpolation function should be called.
    std::shared_ptr< CSeries > mMult( const std::shared_ptr< const CSeries >& t_Series ) const;
    
    // Substraction of values in spectral properties that have same wavelength. Function will work only
    // if two spectral properties have identical wavelengths. Otherwise runtime error will be thrown.
    // If two spectral properites do not have same wavelength range, then interpolation function should be called.
    std::shared_ptr< CSeries > mSub( const std::shared_ptr< const CSeries >& t_Series ) const;
    
    // Addition of values in spectral properties that have same wavelength. Function will work only
    // if two spectral properties have identical wavelengths. Otherwise runtime error will be thrown.
    // If two spectral properites do not have same wavelength range, then interpolation function should be called.
    std::shared_ptr< CSeries > 
      mAdd( const std::shared_ptr< const CSeries >& t_Series ) const;
    
    // Return wavelenght values for spectral properties.
    std::shared_ptr< std::vector< double > > getXArray() const;

    // Sum of all properties between two x values. Default arguments mean all items are sum
    double sum( double const minX = 0, double const maxX = 0 ) const;

    // Sort series by x values in accending order
    void sort();

    std::vector< std::shared_ptr< CSeriesPoint > >::const_iterator begin() const;
    std::vector< std::shared_ptr< CSeriesPoint > >::const_iterator end() const;
    size_t size() const;
    std::shared_ptr< const CSeriesPoint > operator[]( size_t Index ) const;

    void clear();

  private:
    struct SeriesCompare {
      bool operator()( const std::shared_ptr< const CSeriesPoint >& l,
        const std::shared_ptr< CSeriesPoint >& r ) const;
    };
    std::shared_ptr< ISeriesPoint > findLower( double const t_x ) const;
    std::shared_ptr< ISeriesPoint > findUpper( double const t_x ) const;
    double interpolate( std::shared_ptr< const ISeriesPoint > t_Lower, 
      std::shared_ptr< const ISeriesPoint > t_Upper, double const t_x ) const;

    std::vector< std::shared_ptr< CSeriesPoint > > m_Series;

  };

}

#endif