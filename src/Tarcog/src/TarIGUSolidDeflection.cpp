#define _USE_MATH_DEFINES
#include <math.h>

#include "TarIGUSolidDeflection.hpp"
#include "FenestrationCommon.hpp"
#include "TarSurface.hpp"
#include "TarcogConstants.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace Tarcog {
  CTarIGUSolidLayerDeflection::CTarIGUSolidLayerDeflection( shared_ptr< CTarIGUSolidLayer >& t_SolidLayer ) :
    CTarIGUSolidLayer( *t_SolidLayer ), m_SolidLayer( t_SolidLayer ), 
    m_YoungsModulus( DeflectionConstants::YOUNGSMODULUS ), m_PoisonRatio( DeflectionConstants::POISONRATIO ) {

  }

  CTarIGUSolidLayerDeflection::CTarIGUSolidLayerDeflection( shared_ptr< CTarIGUSolidLayer >& t_SolidLayer,
    const double t_YoungsModulus, const double t_PoisonRatio ) : 
    CTarIGUSolidLayer( *t_SolidLayer ), m_SolidLayer( t_SolidLayer ), m_YoungsModulus( t_YoungsModulus ),
    m_PoisonRatio( t_PoisonRatio ) {

  }

  void CTarIGUSolidLayerDeflection::calculateConvectionConductionState() {
    CTarIGUSolidLayer::calculateConvectionConductionState();
    // Relaxation parameter is low because that will make possible solution to converge.
    // Instability in rest of equation is great if using higher relaxation parameter and 
    // it probaby does not matter what solver is used.
    const double RelaxationParamter = 0.005;

    double Dp = pressureDifference();
    double D = flexuralRigidity();
    double Ld = m_Surface[ Side::Front ]->getMeanDeflection();
    Ld += LdMean( Dp, D ) * RelaxationParamter;
    double Ldmax = m_Surface[ Side::Front ]->getMaxDeflection();
    Ldmax += LdMax( Dp, D ) * RelaxationParamter;
    m_Surface[ Side::Front ]->applyDeflection( Ld, Ldmax );
    m_Surface[ Side::Back ]->applyDeflection( Ld, Ldmax );
  }

  double CTarIGUSolidLayerDeflection::LdMean( const double t_P, const double t_D ) const {
    double coeff = 16 * t_P / ( pow( M_PI, 6 ) * t_D );
    double totalSum = 0;
    for( size_t m = 1; m <= 5; m += 2 ) {
      for( size_t n = 1; n <= 5; n += 2 ) {
        double denom = m * m * n * n * M_PI * M_PI * pow( pow( m / m_Width, 2 ) + pow( n / m_Height, 2 ), 2 );
        totalSum += 4 / denom;
      }
    }
    totalSum = coeff * totalSum;
    return totalSum;
  }

  double CTarIGUSolidLayerDeflection::LdMax( const double t_P, const double t_D ) const {
    double coeff = 16 * t_P / ( pow( M_PI, 6 ) * t_D );
    double totalSum = 0;
    for( size_t m = 1; m <= 5; m += 2 ) {
      for( size_t n = 1; n <= 5; n += 2 ) {
        double nomin = sin( m * M_PI / 2 ) * sin( n * M_PI / 2 );
        double denom = m * n * pow( pow( m / m_Width, 2 ) + pow( n / m_Height, 2 ), 2 );
        totalSum += nomin / denom;
      }
    }
    totalSum = coeff * totalSum;
    return totalSum;
  }

  double CTarIGUSolidLayerDeflection::flexuralRigidity() const {
    return m_YoungsModulus * pow( m_Thickness, 3 ) / ( 12 * ( 1 - pow( m_PoisonRatio, 2 ) ) );
  }

  double CTarIGUSolidLayerDeflection::pressureDifference() const {
    double P1 = dynamic_pointer_cast< CGasLayer >( m_NextLayer )->getPressure();
    double P2 = dynamic_pointer_cast< CGasLayer >( m_PreviousLayer )->getPressure();
    return P1 - P2;
  }

}