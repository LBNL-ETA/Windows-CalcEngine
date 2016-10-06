#include <stdexcept>
#include <algorithm>
#include <math.h>

#include "Series.hpp"
#include "IntegratorStrategy.hpp"

using namespace std;

namespace FenestrationCommon {

  /////////////////////////////////////////////////////
  //  CSeriesPoint
  /////////////////////////////////////////////////////

  CSeriesPoint::CSeriesPoint() : m_x( 0 ), m_Value( 0 ) {
  
  }

  CSeriesPoint::CSeriesPoint( double t_Wavelength, double t_Value ) :
    m_x( t_Wavelength ), m_Value( t_Value ) {
  
  }

  double CSeriesPoint::x() const {
    return m_x;
  }

  double CSeriesPoint::value() const {
    return m_Value;
  }

  void CSeriesPoint::value( double const t_Value ) {
    m_Value = t_Value; 
  }

  CSeriesPoint& CSeriesPoint::operator=( const CSeriesPoint& t_Property ) {
    m_x = t_Property.m_x;
    m_Value = t_Property.m_Value;
    return *this;
  }

  bool CSeriesPoint::operator<( const CSeriesPoint& t_Point ) {
    return m_x < t_Point.m_x;
  }

  /////////////////////////////////////////////////////
  //  CSeries
  /////////////////////////////////////////////////////

  CSeries::CSeries() { }

  void CSeries::addProperty( const double t_x, const double t_Value ) {
    shared_ptr< CSeriesPoint > aProperty = make_shared< CSeriesPoint >( t_x, t_Value );
    m_Series.push_back( aProperty );
  }

  void CSeries::insertToBeginning( double t_x, double t_Value ) {
    shared_ptr< CSeriesPoint > aProperty = make_shared< CSeriesPoint >( t_x, t_Value );
    m_Series.insert( m_Series.begin(), aProperty );
  }

  void CSeries::setConstantValues( const vector< double >& t_Wavelengths, double const t_Value ) {
    m_Series.clear();
    for( auto it = t_Wavelengths.begin(); it < t_Wavelengths.end(); ++it ) {
      addProperty( ( *it ), t_Value );
    }
  }

  shared_ptr< CSeries > CSeries::integrate( IntegrationType t_IntegrationType ) const {
    
    shared_ptr< CSeries > newProperties = make_shared< CSeries >();
    CIntegratorFactory aFactory = CIntegratorFactory();
    shared_ptr< IIntegratorStrategy > aIntegrator = aFactory.getIntegrator( t_IntegrationType );
    shared_ptr< ISeriesPoint > previousProperty = nullptr;

    for( shared_ptr< ISeriesPoint > spectralProperty : m_Series ) {
      if( previousProperty != nullptr ) {
        double w1 = previousProperty->x();
        double w2 = spectralProperty->x();
        double y1 = previousProperty->value();
        double y2 = spectralProperty->value();
        double aValue = aIntegrator->integrate( w1, w2, y1, y2 );
        newProperties->addProperty( w1, aValue );
      }
      previousProperty = spectralProperty;
    }

    return newProperties;

  }

  shared_ptr< ISeriesPoint > CSeries::findLower( double const t_Wavelength ) const {
    shared_ptr< ISeriesPoint > currentProperty = nullptr;

    for( shared_ptr< ISeriesPoint > spectralProperty : m_Series ) {
      double aWavelength = spectralProperty->x();
      if( aWavelength > t_Wavelength ) {
        break;
      }
      currentProperty = spectralProperty;
    }

    return currentProperty;
  }
  
  shared_ptr< ISeriesPoint > CSeries::findUpper( double const t_Wavelength ) const {
    shared_ptr< ISeriesPoint > currentProperty = nullptr;

    for( shared_ptr< ISeriesPoint > spectralProperty : m_Series ) {
      double aWavelength = spectralProperty->x();
      if( aWavelength > t_Wavelength ) {
        currentProperty = spectralProperty;
        break;
      }
    }

    return currentProperty;
  }

  double CSeries::interpolate( shared_ptr< const ISeriesPoint > t_Lower, 
    shared_ptr< const ISeriesPoint > t_Upper, double const t_Wavelength ) const {
    
    double w1 = t_Lower->x();
    double w2 = t_Upper->x();
    double v1 = t_Lower->value();
    double v2 = t_Upper->value();
    double vx = 0;
    if( w2 != w1 ) {
      vx = v1 + ( t_Wavelength - w1 ) * ( v2 - v1 ) / ( w2 - w1 );
    } else {
      vx = v1; // extrapolating same value for all values out of range
    }

    return vx;
  }

  shared_ptr< CSeries > CSeries::interpolate( 
    const vector< double >& t_Wavelengths ) const {
    shared_ptr< CSeries > newProperties = make_shared< CSeries >();

    if( size() != 0 ) {

      shared_ptr< ISeriesPoint > lower = nullptr;
      shared_ptr< ISeriesPoint > upper = nullptr;

      for( double wavelength : t_Wavelengths ) {
        lower = findLower( wavelength );
        upper = findUpper( wavelength );

        if( lower == nullptr ) {
          lower = upper;
        }

        if( upper == nullptr ) {
          upper = lower;
        }

        newProperties->addProperty( wavelength, interpolate( lower, upper, wavelength ) );
      }
    }

    return newProperties;
  }

  shared_ptr< CSeries > CSeries::mMult( const CSeries& t_Series ) const {
    shared_ptr< CSeries > newProperties = make_shared< CSeries >();

    const double WAVELENGTHTOLERANCE = 1e-10;

    size_t minSize = min( m_Series.size(), t_Series.m_Series.size() );

    for( size_t i = 0; i < minSize; ++i ) {
      double value = m_Series[i]->value() * t_Series.m_Series[i]->value();
      double wv = m_Series[i]->x();
      double testWv = t_Series.m_Series[i]->x();

      if( fabs( wv - testWv ) > WAVELENGTHTOLERANCE ) {
        throw runtime_error("Wavelengths of two vectors are not the same. Cannot preform multiplication.");
      }

      newProperties->addProperty( wv, value );
    }

    return newProperties;
  }

  shared_ptr< CSeries > CSeries::mSub( const CSeries& t_Series ) const {
    const double WAVELENGTHTOLERANCE = 1e-10;

    shared_ptr< CSeries > newProperties = make_shared< CSeries >();
    size_t minSize = min( m_Series.size(), t_Series.m_Series.size() );

    for( size_t i = 0; i < minSize; ++i ) {
      double value = m_Series[i]->value() - t_Series.m_Series[i]->value();
      double wv = m_Series[i]->x();
      double testWv = t_Series.m_Series[i]->x();

      if( fabs( wv - testWv ) > WAVELENGTHTOLERANCE ) {
        throw runtime_error("Wavelengths of two vectors are not the same. Cannot preform multiplication.");
      }

      newProperties->addProperty( wv, value );
    }

    return newProperties;
  }

  shared_ptr< CSeries > CSeries::mAdd( const CSeries& t_Series ) const {
    const double WAVELENGTHTOLERANCE = 1e-10;

    shared_ptr< CSeries > newProperties = make_shared< CSeries >();
    size_t minSize = min( m_Series.size(), t_Series.m_Series.size() );

    for( size_t i = 0; i < minSize; ++i ) {
      double value = m_Series[i]->value() + t_Series.m_Series[i]->value();
      double wv = m_Series[i]->x();
      double testWv = t_Series.m_Series[i]->x();

      if( fabs( wv - testWv ) > WAVELENGTHTOLERANCE ) {
        throw runtime_error("Wavelengths of two vectors are not the same. Cannot preform multiplication.");
      }

      newProperties->addProperty( wv, value );
    }

    return newProperties;
  }

  shared_ptr< vector< double > > CSeries::getXArray() const {
    shared_ptr< vector< double > > aArray = make_shared< vector< double > >();
    for( shared_ptr< ISeriesPoint > spectralProperty : m_Series ) {
      aArray->push_back( spectralProperty->x() );
    }

    return aArray;
  }

  double CSeries::sum( double const minLambda, double const maxLambda ) const {
    double const TOLERANCE = 1e-6; // introduced because of rounding error
    double total = 0;
    for( shared_ptr< ISeriesPoint > aPoint : m_Series ) {
      double wavelength = aPoint->x();
      // Last point must be excluded because of ranges. Each wavelength represent range from wavelength one
      // to wavelength two. Summing value of the last wavelength in array would be wrong because it would
      // include one additional range after the end of spectrum. For example, summing all the data from 0.38 to
      // 0.78 would include visible range. However, including 0.78 in sum would add extra value from 0.78 to 0.79.
      if( ( ( wavelength >= ( minLambda - TOLERANCE ) && wavelength < ( maxLambda - TOLERANCE ) ) ||
        ( minLambda == 0 && maxLambda == 0 ) ) ) {
        total += aPoint->value();
      }
    }
    return total;
  }

  void CSeries::sort() {
    std::sort( m_Series.begin(), m_Series.end(), 
      []( const shared_ptr< const CSeriesPoint >& l, const shared_ptr< const CSeriesPoint >& r ) -> bool
      {
        return l->x() < r->x();
      });
  }

  vector< shared_ptr< CSeriesPoint > >::const_iterator CSeries::begin() const {
    return m_Series.cbegin();
  }

  vector< shared_ptr< CSeriesPoint > >::const_iterator CSeries::end() const {
    return m_Series.cend();
  }

  size_t CSeries::size() const {
    return m_Series.size();
  }

  shared_ptr< const CSeriesPoint > CSeries::operator[]( size_t Index ) const {
  //CSeriesPoint CSeries::operator[]( int Index ) const {
    if( Index >= m_Series.size() ) {
      throw runtime_error("Index out of range");
    }
    return m_Series[Index];
  }

  void CSeries::clear() {
    m_Series.clear();
  }

}
