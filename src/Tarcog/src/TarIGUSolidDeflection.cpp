#define _USE_MATH_DEFINES
#include <math.h>

#include "TarIGUSolidDeflection.hpp"
#include "FenestrationCommon.hpp"
#include "TarSurface.hpp"
#include "TarcogConstants.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace Tarcog {

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //    CTarIGUSolidLayerDeflection
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////

  CTarIGUSolidLayerDeflection::CTarIGUSolidLayerDeflection( shared_ptr< CTarIGUSolidLayer >& t_SolidLayer ) :
    CTarIGUSolidLayer( *t_SolidLayer ),
    m_YoungsModulus( DeflectionConstants::YOUNGSMODULUS ), m_PoisonRatio( DeflectionConstants::POISONRATIO ) {
  }

  CTarIGUSolidLayerDeflection::CTarIGUSolidLayerDeflection( shared_ptr< CTarIGUSolidLayer >& t_SolidLayer,
    const double t_YoungsModulus, const double t_PoisonRatio ) : 
    CTarIGUSolidLayer( *t_SolidLayer ),
    m_YoungsModulus( t_YoungsModulus ), m_PoisonRatio( t_PoisonRatio ) {
  }

  CTarIGUSolidLayerDeflection::CTarIGUSolidLayerDeflection( 
    const CTarIGUSolidLayerDeflection & t_Layer ) : CTarIGUSolidLayer( t_Layer ) {
    m_YoungsModulus = t_Layer.m_YoungsModulus;
    m_PoisonRatio = t_Layer.m_PoisonRatio;
  }

  void CTarIGUSolidLayerDeflection::calculateConvectionConductionState() {
    CTarIGUSolidLayer::calculateConvectionConductionState();
    
  }

  double CTarIGUSolidLayerDeflection::flexuralRigidity() const {
    return m_YoungsModulus * pow( m_Thickness, 3 ) / ( 12 * ( 1 - pow( m_PoisonRatio, 2 ) ) );
  }

  double CTarIGUSolidLayerDeflection::pressureDifference() const {
    double P1 = dynamic_pointer_cast< CGasLayer >( m_NextLayer )->getPressure();
    double P2 = dynamic_pointer_cast< CGasLayer >( m_PreviousLayer )->getPressure();
    return P1 - P2;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //    CTarIGUDeflectionTempAndPressure
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////

  CTarIGUDeflectionTempAndPressure::CTarIGUDeflectionTempAndPressure( shared_ptr< CTarIGUSolidLayerDeflection >& t_SolidLayer, 
    const double t_MaxDeflectionCoeff, const double t_MeanDeflectionCoeff ) : 
    CTarIGUSolidLayerDeflection( *t_SolidLayer ), 
    m_MaxCoeff( t_MaxDeflectionCoeff ), m_MeanCoeff( t_MeanDeflectionCoeff ) {
    
  }

  void CTarIGUDeflectionTempAndPressure::calculateConvectionConductionState() {
    CTarIGUSolidLayerDeflection::calculateConvectionConductionState();
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
    for( Side aSide : EnumSide() ) {
      m_Surface[ aSide ]->applyDeflection( Ld, Ldmax );
    }
  }

  double CTarIGUDeflectionTempAndPressure::LdMean( const double t_P, const double t_D ) const {
    return m_MeanCoeff * t_P / t_D;
  }

  double CTarIGUDeflectionTempAndPressure::LdMax( const double t_P, const double t_D ) const {
    return m_MaxCoeff * t_P / t_D;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //    CTarIGUDeflectionTempAndPressure
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  CTarIGUDeflectionMeasuread::CTarIGUDeflectionMeasuread( shared_ptr< CTarIGUSolidLayerDeflection >& t_Layer, 
    const double t_MeanDeflection, const double t_MaxDeflection ) : 
    CTarIGUSolidLayerDeflection( *t_Layer ) {
    for( Side aSide : EnumSide() ) {
      m_Surface[ aSide ]->applyDeflection( t_MeanDeflection, t_MaxDeflection );
    }
  }

}