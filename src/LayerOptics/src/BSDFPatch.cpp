#define _USE_MATH_DEFINES
#include <math.h>

#include "BSDFPatch.hpp"
#include "BeamDirection.hpp"

using namespace std;

namespace LayerOptics {

  /////////////////////////////////////////////////////////////////
  ///  CAngleLimits
  /////////////////////////////////////////////////////////////////

  CAngleLimits::CAngleLimits( double const t_Low, double const t_High ) :
    m_Low( t_Low ), m_High( t_High ) {
  
  }

  double CAngleLimits::low() const {
    return m_Low;
  }

  double CAngleLimits::high() const {
    return m_High;
  }

  double CAngleLimits::delta() const {
    return m_High - m_Low;
  }

  double CAngleLimits::average() const {
    return ( m_Low + m_High ) / 2;
  }

  /////////////////////////////////////////////////////////////////
  ///  CCentralAngleLimits
  /////////////////////////////////////////////////////////////////
  CCentralAngleLimits::CCentralAngleLimits( const double t_High ) : CAngleLimits( 0, t_High ) {
     
  }

  double CCentralAngleLimits::average() const {
    return m_Low;
  }

  /////////////////////////////////////////////////////////////////
  ///  CBSDFPatch
  /////////////////////////////////////////////////////////////////

  CBSDFPatch::CBSDFPatch( shared_ptr< CAngleLimits > t_Theta, shared_ptr< CAngleLimits > t_Phi ) {
    m_Phi = t_Phi;
    m_Theta = t_Theta;
    calculateLambda();
  }

  shared_ptr< CBeamDirection > CBSDFPatch::centerPoint() const {
    return make_shared< CBeamDirection >( m_Theta->average(), m_Phi->average() );
  }

  double CBSDFPatch::lambda() const {
    return m_Lambda;
  }

  void CBSDFPatch::calculateLambda() {
    double thetaLow = m_Theta->low() * M_PI / 180;
    double thetaHight = m_Theta->high() * M_PI / 180;
    double deltaPhi = m_Phi->delta() * M_PI / 180;
    m_Lambda = 0.5 * deltaPhi * ( sin( thetaHight ) * sin( thetaHight ) - sin( thetaLow ) * sin( thetaLow ) );
  }

}