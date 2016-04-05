#define _USE_MATH_DEFINES
#include <math.h>
#include <stdexcept>
#include <assert.h>

#include "AngularProperties.hpp"
#include "MathFunctions.hpp"
#include "SpecularLayer.hpp"

using namespace FenestrationCommon;
using namespace std;

namespace SpectralAveraging {

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  CAngularProperties
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  CAngularProperties::CAngularProperties( const double t_TransmittanceZero, const double t_ReflectanceZero ) : 
    m_Transmittance0( t_TransmittanceZero ), m_Reflectance0( t_ReflectanceZero ), 
    m_Transmittance( -1 ), m_Reflectance( -1 ), m_StateAngle( -1 ), m_StateWavelength( -1 ) {
  };

  double CAngularProperties::cosAngle( const double t_Angle ) {
    double aAngle = radians( t_Angle );
    return cos( aAngle );
  }

  void CAngularProperties::checkStateProperties( const double t_Angle, const double ) {
    if( t_Angle > 90 || t_Angle < 0 ) {
      throw runtime_error("Incoming angle is out of range. Incoming angle must be between 0 and 90 degrees.");
    }

  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  CAngularPropertiesUncoated
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  CAngularPropertiesUncoated::CAngularPropertiesUncoated( const double t_Thicknes, 
    const double t_TransmittanceZero, const double t_ReflectanceZero ) : 
    CAngularProperties( t_TransmittanceZero, t_ReflectanceZero ), m_Thickness( t_Thicknes ) {
    
    // calculation of intermediate uncoated properties
    m_Beta = m_Transmittance0 * m_Transmittance0 - m_Reflectance0 * m_Reflectance0 + 2 * m_Reflectance0 + 1;
    m_Rho0 = ( m_Beta - sqrt( m_Beta * m_Beta - 4 * ( 2 - m_Reflectance0) * m_Reflectance0 ) ) / ( 2 * ( 2 - m_Reflectance0 ) );
    
  };

  double CAngularPropertiesUncoated::transmittance( const double t_Angle, const double t_Wavelength ) {
    checkStateProperties( t_Angle, t_Wavelength );
    return m_Transmittance;
  };

  double CAngularPropertiesUncoated::reflectance( const double t_Angle, const double t_Wavelength ) {
    checkStateProperties( t_Angle, t_Wavelength );
    return m_Reflectance;
  };

  void CAngularPropertiesUncoated::checkStateProperties( const double t_Angle, const double t_Wavelength ) {
    CAngularProperties::checkStateProperties( t_Angle, t_Wavelength );

    if( m_StateAngle != t_Angle || m_StateWavelength != t_Wavelength ) {
      double aAngle = radians( t_Angle );
      double aCosPhi = cos( aAngle );
      double n = ( 1 + sqrt( m_Rho0 ) ) / ( 1 - sqrt( m_Rho0 ) );
      double aCosPhiPrim = cos( asin( sin( aAngle ) / n ) );
      double a = 0;
      
      if( m_Transmittance0 > 0 ) {
        double k = 0;
        double alpha = 0;
        k = - t_Wavelength / ( 4 * M_PI * m_Thickness ) * log( ( m_Reflectance0 - m_Rho0 ) / ( m_Transmittance0 * m_Rho0 ) );
        alpha = 2 * M_PI * k / t_Wavelength;
        a = exp( -2 * alpha * m_Thickness / aCosPhiPrim );
      }

      double rhoP = pow( ( ( n * aCosPhi - aCosPhiPrim )  / ( n * aCosPhi + aCosPhiPrim ) ), 2 );
      double rhoS = pow( ( ( aCosPhi - n * aCosPhiPrim )  / ( aCosPhi + n * aCosPhiPrim ) ), 2 );
      double tauP = 1 - rhoP;
      double tauS = 1 - rhoS;

      double tau_TotP = a * pow( tauP, 2 ) / ( 1 - pow( a, 2 ) * pow( rhoP, 2 ) );
      double tau_TotS = a * pow( tauS, 2 ) / ( 1 - pow( a, 2 ) * pow( rhoS, 2 ) );

      double rho_TotP = ( 1 + a * tau_TotP ) * rhoP;
      double rho_TotS = ( 1 + a * tau_TotS ) * rhoS;

      m_Transmittance = ( tau_TotS + tau_TotP ) / 2;
      m_Reflectance = ( rho_TotS + rho_TotP ) / 2;

      m_StateAngle = t_Angle;
      m_StateWavelength = t_Wavelength;
    }

  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  CAngularPropertiesCoated
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  CAngularPropertiesCoated::CAngularPropertiesCoated( const double t_Transmittance, const double t_Reflectance,
    const double t_SolarTransmittance ) : CAngularProperties( t_Transmittance, t_Reflectance ), 
    m_SolTransmittance0( t_SolarTransmittance ) {
    
  };
  
  double CAngularPropertiesCoated::transmittance( const double t_Angle, const double t_Wavelength ) {
    checkStateProperties( t_Angle, t_Wavelength );
    return m_Transmittance;
  };

  double CAngularPropertiesCoated::reflectance( const double t_Angle, const double t_Wavelength ) {
    checkStateProperties( t_Angle, t_Wavelength );
    return m_Reflectance;
  };

  void CAngularPropertiesCoated::checkStateProperties( const double t_Angle, const double ) {
    CAngularProperties::checkStateProperties( t_Angle, 0 ); // Wavelength is not provided for coated glass

    if( m_StateAngle != t_Angle ) {
      double aAngle = radians( t_Angle );
      double aCosPhi = cos( aAngle );

      shared_ptr< Coefficients > TCoeff = nullptr;
      shared_ptr< Coefficients > RCoeff = nullptr;

      CCoatingCoefficients aCoefficients = CCoatingCoefficients();

      if( m_SolTransmittance0 > 0.645 ) {
        TCoeff = aCoefficients.getCoefficients( CoatingProperty::T, CoatingType::Clear );
        RCoeff = aCoefficients.getCoefficients( CoatingProperty::R, CoatingType::Clear );
      } else {
        TCoeff = aCoefficients.getCoefficients( CoatingProperty::T, CoatingType::Bronze );
        RCoeff = aCoefficients.getCoefficients( CoatingProperty::R, CoatingType::Bronze );
      }

      assert( TCoeff != nullptr );
      assert( RCoeff != nullptr );
      
      double tau = TCoeff->inerpolation( aCosPhi );
      m_Transmittance = tau * m_Transmittance0;
      double rho = RCoeff->inerpolation( aCosPhi ) - tau;
      m_Reflectance = m_Reflectance0 * ( 1 - rho ) + rho;

      // Interpolation coefficients are not precise enought and this check is necessary
      if( m_Transmittance > 1 ) {
        m_Transmittance = 1;
      } else if ( m_Transmittance < 0 ) {
        m_Transmittance = 0;
      }

      if( m_Reflectance > 1 ) {
        m_Reflectance = 1;
      } else if ( m_Reflectance < 0 ) {
        m_Reflectance = 0;
      }

      m_StateAngle = t_Angle;
    }

  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  Coefficients
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  Coefficients::Coefficients( const double t_C0, const double t_C1, const double t_C2, const double t_C3,
      const double t_C4 ) : C0( t_C0 ), C1( t_C1 ), C2( t_C2 ), C3( t_C3 ), C4( t_C4 ) {
  
  };

  double Coefficients::inerpolation( const double t_Value ) {
    return C0 + C1 * t_Value + C2 * pow( t_Value, 2 ) + C3 * pow( t_Value, 3 ) + C4 * pow( t_Value, 4 );
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  CCoatingCoefficients
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  CCoatingCoefficients::CCoatingCoefficients() {
  
  };

  shared_ptr< Coefficients > CCoatingCoefficients::getCoefficients( const CoatingProperty t_Property, const CoatingType t_Type ) {
    shared_ptr< Coefficients > aCoefficients = nullptr;
    switch ( t_Property ) {
    case CoatingProperty::T:
      switch ( t_Type ) {
      case CoatingType::Clear:
        aCoefficients = make_shared< Coefficients >( -0.0015, 3.355, -3.84, 1.46, 0.0288 );
        break;
      case CoatingType::Bronze:
        aCoefficients = make_shared< Coefficients >( -0.002, 2.813, -2.341, -0.05725, 0.599 );
        break;
      default:
        assert("Incorrect selection of type.");
        break;
      }
      break;
    case CoatingProperty::R:
      switch ( t_Type ) {
      case CoatingType::Clear:
        aCoefficients = make_shared< Coefficients >( 0.999, -0.563, 2.043, -2.532, 1.054 );
        break;
      case CoatingType::Bronze:
        aCoefficients = make_shared< Coefficients >( 0.997, -1.868, 6.513, -7.862, 3.225 );
        break;
      default:
        assert("Incorrect selection of type.");
        break;
      }
      break;
    default:
      assert("Incorrect selection of property.");
      break;
    }

    assert( aCoefficients != nullptr );

    return aCoefficients;

  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  CAngularPropertiesFactory
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  CAngularPropertiesFactory::CAngularPropertiesFactory( const double t_Transmittance0, const double t_Reflectance0, const double t_Thickness,
    const double t_SolarTransmittance ) : m_Thickness( t_Thickness ), m_Transmittance0( t_Transmittance0 ), m_Reflectance0( t_Reflectance0 ),
    m_SolarTransmittance0( t_SolarTransmittance ) {
    
  };

  shared_ptr< CAngularProperties > CAngularPropertiesFactory::getAngularProperties( const SpecularLayerSurfaceType t_SurfaceType ) {
    shared_ptr< CAngularProperties > aProperties = nullptr;
    switch( t_SurfaceType ) {
    case SpecularLayerSurfaceType::Coated:
      aProperties = make_shared< CAngularPropertiesCoated >( m_Transmittance0, m_Reflectance0, m_SolarTransmittance0 );
      break;
    case SpecularLayerSurfaceType::Uncoated:
      aProperties = make_shared< CAngularPropertiesUncoated >( m_Thickness, m_Transmittance0, m_Reflectance0 );
      break;
    default:
      throw runtime_error("Incorrect surface type. Cannot create correct angular properties.");
      break;
    }
    assert( aProperties != nullptr );
    return aProperties;
  };

}