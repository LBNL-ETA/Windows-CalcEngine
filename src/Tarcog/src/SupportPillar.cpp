#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>

#include "WCETarcog.hpp"
#include "BaseIGULayer.hpp"

using namespace std;

namespace Tarcog {


  ////////////////////////////////////////////////////////////////////////////
  ////  CSupportPillar
  ////////////////////////////////////////////////////////////////////////////
  CSupportPillar::CSupportPillar( const CIGUGapLayer& t_Layer, const double t_Conductivity ) : 
    CIGUGapLayer( t_Layer ), m_Conductivity( t_Conductivity ) {

  }

  CSupportPillar::CSupportPillar( const CSupportPillar& t_Pillar ) : 
    CState( t_Pillar ), CIGUGapLayer( t_Pillar ) {

  }

  void CSupportPillar::calculateConvectionOrConductionFlow() {
    CIGUGapLayer::calculateConvectionOrConductionFlow();
    if( !isCalculated() ) {
      m_ConductiveConvectiveCoeff += conductivityOfPillarArray();
    }
  }

  ////////////////////////////////////////////////////////////////////////////
  ////  CCircularPillar
  ////////////////////////////////////////////////////////////////////////////
  CCircularPillar::CCircularPillar( const CIGUGapLayer& t_Gap,
    const double t_Conductivity, const double t_Spacing, const double t_Radius ) :
    CSupportPillar( t_Gap, t_Conductivity ), m_Spacing( t_Spacing ), m_Radius( t_Radius ) {

  }

  CCircularPillar::CCircularPillar( const CCircularPillar& t_Pillar ) : 
    CState( t_Pillar ), CSupportPillar( t_Pillar ) {

  }

  double CCircularPillar::conductivityOfPillarArray() {
    double cond1 = dynamic_pointer_cast< CBaseIGULayer >( m_PreviousLayer )->getConductivity();
    double cond2 = dynamic_pointer_cast< CBaseIGULayer >( m_NextLayer )->getConductivity();
    double aveCond = ( cond1 + cond2 ) / 2;

    double cond = 2 * aveCond * m_Radius / ( pow( m_Spacing, 2 ) );
    cond *= 1 / ( 1 + 2 * m_Thickness * aveCond / ( m_Conductivity * M_PI * m_Radius ) );

    return cond;
  }

}
