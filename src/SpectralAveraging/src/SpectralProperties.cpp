#include <stdexcept>
#include <algorithm>
#include <math.h>

#include "SpectralProperties.hpp"
#include "IntegratorStrategy.hpp"

using namespace std;

namespace SpectralAveraging {

  /////////////////////////////////////////////////////
  //  CSpectralProperty
  /////////////////////////////////////////////////////

  CSpectralProperty::CSpectralProperty() : m_Wavelength( 0 ), m_Value( 0 ) {
  
  }

  CSpectralProperty::CSpectralProperty( double t_Wavelength, double t_Value ) :
    m_Wavelength( t_Wavelength ), m_Value( t_Value ) {
  
  }

  double CSpectralProperty::wavelength() const {
    return m_Wavelength;
  }

  double CSpectralProperty::value() const {
    return m_Value;
  }

  void CSpectralProperty::value( double const t_Value ) {
    m_Value = t_Value; 
  };

  CSpectralProperty& CSpectralProperty::operator=( const CSpectralProperty& t_Property ) {
    m_Wavelength = t_Property.m_Wavelength;
    m_Value = t_Property.m_Value;
    return *this;
  };

  /////////////////////////////////////////////////////
  //  CSpectralProperties
  /////////////////////////////////////////////////////

  CSpectralProperties::CSpectralProperties() {};

  void CSpectralProperties::addProperty( double t_Wavelength, double t_Value ) {
    shared_ptr< CSpectralProperty > aProperty = make_shared< CSpectralProperty >( t_Wavelength, t_Value );
    m_SpectralProperties.push_back( aProperty );
  }

  void CSpectralProperties::insertToBeginning( double t_Wavelength, double t_Value ) {
    shared_ptr< CSpectralProperty > aProperty = make_shared< CSpectralProperty >( t_Wavelength, t_Value );
    m_SpectralProperties.insert( m_SpectralProperties.begin(), aProperty );
  };

  void CSpectralProperties::setConstantValues( shared_ptr< vector< double > > t_Wavelengths, double const t_Value ) {
    m_SpectralProperties.clear();
    vector< double >::iterator it;
    for( it = t_Wavelengths->begin(); it < t_Wavelengths->end(); ++it ) {
      addProperty( ( *it ), t_Value );
    }
  };

  shared_ptr< CSpectralProperties > CSpectralProperties::integrate( IntegrationType t_IntegrationType ) const {
    
    shared_ptr< CSpectralProperties > newProperties = make_shared< CSpectralProperties >();
    CIntegratorFactory aFactory = CIntegratorFactory();
    shared_ptr< IIntegratorStrategy > aIntegrator = aFactory.getIntegrator( t_IntegrationType );
    shared_ptr< ISpectralProperty > previousProperty = nullptr;

    for( shared_ptr< ISpectralProperty > spectralProperty : m_SpectralProperties ) {
      if( previousProperty != nullptr ) {
        double w1 = previousProperty->wavelength();
        double w2 = spectralProperty->wavelength();
        double y1 = previousProperty->value();
        double y2 = spectralProperty->value();
        double aValue = aIntegrator->integrate( w1, w2, y1, y2 );
        newProperties->addProperty( w1, aValue );
      }
      previousProperty = spectralProperty;
    }

    return newProperties;

  };

  shared_ptr< ISpectralProperty > CSpectralProperties::findLower( double const t_Wavelength ) const {
    shared_ptr< ISpectralProperty > currentProperty = nullptr;

    for( shared_ptr< ISpectralProperty > spectralProperty : m_SpectralProperties ) {
      double aWavelength = spectralProperty->wavelength();
      if( aWavelength > t_Wavelength ) {
        break;
      }
      currentProperty = spectralProperty;
    }

    return currentProperty;
  };
  
  shared_ptr< ISpectralProperty > CSpectralProperties::findUpper( double const t_Wavelength ) const {
    shared_ptr< ISpectralProperty > currentProperty = nullptr;

    for( shared_ptr< ISpectralProperty > spectralProperty : m_SpectralProperties ) {
      double aWavelength = spectralProperty->wavelength();
      if( aWavelength > t_Wavelength ) {
        currentProperty = spectralProperty;
        break;
      }
    }

    return currentProperty;
  };

  double CSpectralProperties::interpolate( shared_ptr< const ISpectralProperty > t_Lower, 
    shared_ptr< const ISpectralProperty > t_Upper, double const t_Wavelength ) const {
    
    double w1 = t_Lower->wavelength();
    double w2 = t_Upper->wavelength();
    double v1 = t_Lower->value();
    double v2 = t_Upper->value();
    double vx = 0;
    if( w2 != w1 ) {
      vx = v1 + ( t_Wavelength - w1 ) * ( v2 - v1 ) / ( w2 - w1 );
    } else {
      vx = v1; // extrapolating same value for all values out of range
    }

    return vx;
  };

  shared_ptr< CSpectralProperties > CSpectralProperties::interpolate( 
    shared_ptr< vector< double > > t_Wavelengths ) const {
    shared_ptr< CSpectralProperties > newProperties = make_shared< CSpectralProperties >();

    if( size() != 0 ) {

      shared_ptr< ISpectralProperty > lower = nullptr;
      shared_ptr< ISpectralProperty > upper = nullptr;

      for( double wavelength : *t_Wavelengths ) {
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
  };

  shared_ptr< CSpectralProperties > CSpectralProperties::mMult( 
    shared_ptr< const CSpectralProperties > t_SpectralProperties ) const {
    shared_ptr< CSpectralProperties > newProperties = make_shared< CSpectralProperties >();

    const double WAVELENGTHTOLERANCE = 1e-10;

    size_t minSize = min( m_SpectralProperties.size(), t_SpectralProperties->m_SpectralProperties.size() );

    for( size_t i = 0; i < minSize; ++i ) {
      double value = m_SpectralProperties[i]->value() * t_SpectralProperties->m_SpectralProperties[i]->value();
      double wv = m_SpectralProperties[i]->wavelength();
      double testWv = t_SpectralProperties->m_SpectralProperties[i]->wavelength();

      if( fabs( wv - testWv ) > WAVELENGTHTOLERANCE ) {
        throw runtime_error("Wavelengths of two vectors are not the same. Cannot preform multiplication.");
      }

      newProperties->addProperty( wv, value );
    }

    return newProperties;
  };

  shared_ptr< CSpectralProperties > CSpectralProperties::mSub( 
    shared_ptr< const CSpectralProperties > t_SpectralProperties ) const {
     const double WAVELENGTHTOLERANCE = 1e-10;

    shared_ptr< CSpectralProperties > newProperties = make_shared< CSpectralProperties >();
    size_t minSize = min( m_SpectralProperties.size(), t_SpectralProperties->m_SpectralProperties.size() );

    for( size_t i = 0; i < minSize; ++i ) {
      double value = m_SpectralProperties[i]->value() - t_SpectralProperties->m_SpectralProperties[i]->value();
      double wv = m_SpectralProperties[i]->wavelength();
      double testWv = t_SpectralProperties->m_SpectralProperties[i]->wavelength();

      if( fabs( wv - testWv ) > WAVELENGTHTOLERANCE ) {
        throw runtime_error("Wavelengths of two vectors are not the same. Cannot preform multiplication.");
      }

      newProperties->addProperty( wv, value );
    }

    return newProperties;
  };

  shared_ptr< CSpectralProperties > CSpectralProperties::mAdd( 
    shared_ptr< const CSpectralProperties > t_SpectralProperties ) const {
     const double WAVELENGTHTOLERANCE = 1e-10;

    shared_ptr< CSpectralProperties > newProperties = make_shared< CSpectralProperties >();
    size_t minSize = min( m_SpectralProperties.size(), t_SpectralProperties->m_SpectralProperties.size() );

    for( size_t i = 0; i < minSize; ++i ) {
      double value = m_SpectralProperties[i]->value() + t_SpectralProperties->m_SpectralProperties[i]->value();
      double wv = m_SpectralProperties[i]->wavelength();
      double testWv = t_SpectralProperties->m_SpectralProperties[i]->wavelength();

      if( fabs( wv - testWv ) > WAVELENGTHTOLERANCE ) {
        throw runtime_error("Wavelengths of two vectors are not the same. Cannot preform multiplication.");
      }

      newProperties->addProperty( wv, value );
    }

    return newProperties;
  };

  shared_ptr< vector< double > > CSpectralProperties::getWavelengths() const {
    shared_ptr< vector< double > > aWavelengths = make_shared< vector< double > >();
    for( shared_ptr< ISpectralProperty > spectralProperty : m_SpectralProperties ) {
      aWavelengths->push_back( spectralProperty->wavelength() );
    }

    return aWavelengths;
  };

  double CSpectralProperties::sum( double const minLambda, double const maxLambda ) const {
    double const TOLERANCE = 1e-6; // introduced because of rounding error
    double total = 0;
    for( shared_ptr< ISpectralProperty > spectralProperty : m_SpectralProperties ) {
      double wavelength = spectralProperty->wavelength();
      if( ( wavelength >= ( minLambda - TOLERANCE ) && wavelength < maxLambda ) || ( minLambda == 0 && maxLambda == 0 ) ) {
        total += spectralProperty->value();
      }
    }
    return total;
  };

  vector< shared_ptr< CSpectralProperty > >::const_iterator CSpectralProperties::begin() const {
    return m_SpectralProperties.cbegin();
  };

  vector< shared_ptr< CSpectralProperty > >::const_iterator CSpectralProperties::end() const {
    return m_SpectralProperties.cend();
  };

  size_t CSpectralProperties::size() const {
    return m_SpectralProperties.size();
  };

  shared_ptr< const CSpectralProperty > CSpectralProperties::operator[]( size_t Index ) const {
  //CSpectralProperty CSpectralProperties::operator[]( int Index ) const {
    if( Index >= m_SpectralProperties.size() ) {
      throw runtime_error("Index out of range");
    }
    return m_SpectralProperties[Index];
  };

  shared_ptr< CSpectralProperties > CSpectralProperties::operator=( CSpectralProperties& t_Properties ) {
    clear();
    for( shared_ptr< ISpectralProperty > spectralProperty : t_Properties ) {
      addProperty( spectralProperty->wavelength(), spectralProperty->value() );
    }
    return shared_from_this();
  };

  void CSpectralProperties::clear() {
    m_SpectralProperties.clear();
  };

}