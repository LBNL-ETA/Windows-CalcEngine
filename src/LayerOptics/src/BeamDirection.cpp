#include <math.h>
#include <stdexcept>

#include "MathFunctions.hpp"
#include "BeamDirection.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace LayerOptics {

  CBeamDirection::CBeamDirection( const double t_Theta, const double t_Phi ) : 
    m_Theta( t_Theta ), m_Phi( t_Phi ) {
    if( t_Theta < 0 ) {
      throw runtime_error("Theta angle cannot be less than zero degrees.");
    }
    if( t_Theta > 90 ) {
      throw runtime_error("Theta angle cannot be more than 90 degrees.");
    }

    m_ProfileAngle = -atan( sin( radians( m_Phi ) ) * tan( radians( m_Theta ) ) );
    m_ProfileAngle = degrees( m_ProfileAngle );
  }

  double CBeamDirection::theta() const {
    return m_Theta;
  }

  double CBeamDirection::phi() const {
    return m_Phi;
  }

  double CBeamDirection::profileAngle() const {
    return m_ProfileAngle;
  }

  CBeamDirection& CBeamDirection::operator=( const CBeamDirection& t_SphericalPoint ) {
    m_Theta = t_SphericalPoint.m_Theta;
    m_Phi = t_SphericalPoint.m_Phi;
    m_ProfileAngle = t_SphericalPoint.m_ProfileAngle;
    return *this;
  }

  double CBeamDirection::Altitude() const {
    double aTheta = radians( m_Theta );
    double aPhi = radians( m_Phi );
    return asin( sin( aTheta ) * -sin( aPhi ) );
  }

  double CBeamDirection::Azimuth() const {
    double aAltitude = Altitude();
    double aTheta = radians( m_Theta );
    double aPhi = radians( m_Phi );
    double aAzimuth = 0;
    if( fabs( aTheta ) - fabs( aAltitude ) > 1e-8 ) {
      aAzimuth = -acos( cos( aTheta ) / cos( aAltitude ) );
    }
    if( cos( aPhi ) < 0 ) {
      aAzimuth = -aAzimuth;
    }
    return aAzimuth;  
  }

}