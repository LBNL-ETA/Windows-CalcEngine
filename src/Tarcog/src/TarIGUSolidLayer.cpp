#define _USE_MATH_DEFINES
#include <math.h>
#include <stdexcept>
#include <assert.h>

#include "TarIGUSolidLayer.hpp"
#include "BaseTarcogLayer.hpp"
#include "TarSurface.hpp"
#include "FenestrationCommon.hpp"
#include "TarcogConstants.hpp"
#include "LayerInterfaces.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace Tarcog {

  CTarIGUSolidLayer::CTarIGUSolidLayer( double const t_Thickness, double const t_Conductivity, 
    shared_ptr< CTarSurface > t_FrontSurface, shared_ptr< CTarSurface > t_BackSurface )
    : CBaseIGUTarcogLayer( t_Thickness ), m_Conductivity( t_Conductivity ), m_SolarAbsorptance( 0 ),
    m_YoungsModulus( DeflectionConstants::YOUNGSMODULUS ), m_PoisonRatio( DeflectionConstants::POISONRATIO ) {
    if ( t_FrontSurface != nullptr && t_BackSurface != nullptr ) {
      m_Surface[ Side::Front ] = t_FrontSurface;
      m_Surface[ Side::Back ] = t_BackSurface;
    } else {
      m_Surface[ Side::Front ] = make_shared< CTarSurface >();
      m_Surface[ Side::Back ] = make_shared< CTarSurface >();
    }
  }

  CTarIGUSolidLayer::CTarIGUSolidLayer( double const t_Thickness, double const t_Conductivity,
    double const t_FrontEmissivity, double const t_FrontIRTransmittance,
    double const t_BackEmissivity, double const t_BackIRTransmittance ) : CBaseIGUTarcogLayer( t_Thickness ),
    m_Conductivity( t_Conductivity ), m_SolarAbsorptance( 0 ),
    m_YoungsModulus( DeflectionConstants::YOUNGSMODULUS ), m_PoisonRatio( DeflectionConstants::POISONRATIO ) {
    m_Surface[ Side::Front ] = make_shared< CTarSurface >( t_FrontEmissivity, t_FrontIRTransmittance );
    m_Surface[ Side::Back ] = make_shared< CTarSurface >( t_BackEmissivity, t_BackIRTransmittance );
  }

  void CTarIGUSolidLayer::connectToBackSide( const shared_ptr< CBaseTarcogLayer >& t_Layer ) {
    CBaseTarcogLayer::connectToBackSide( t_Layer );
    t_Layer->setSurface( m_Surface[ Side::Back ], Side::Front );
  }

  double CTarIGUSolidLayer::getConductivity() const {
    return m_Conductivity;
  }

  void CTarIGUSolidLayer::calculateConvectionConductionState() {
    if ( m_Thickness == 0 ) {
      throw runtime_error( "Solid layer thickness is set to zero." );
    }

    m_ConductiveConvectiveCoeff = m_Conductivity / m_Thickness;

    if( m_CalcDeflection ) {
      // Relaxation parameter is low because that will make possible solution to converge.
      // Instability in rest of equation is great if using higher relaxation parameter and 
      // it probaby does not matter what solver is used.
      const double RelaxationParamter = 0.005;

      double P1 = dynamic_pointer_cast< CGasLayer >( m_NextLayer )->getPressure();
      double P2 = dynamic_pointer_cast< CGasLayer >( m_PreviousLayer )->getPressure();
      double Dp = P1 - P2;
      double D = m_YoungsModulus * pow( m_Thickness, 3 ) / ( 12 * ( 1 - pow( m_PoisonRatio, 2 ) ) );
      double Ld = m_Surface[ Side::Front ]->getDeflection();
      Ld += LdMean( Dp, D ) * RelaxationParamter;
      m_Surface[ Side::Front ]->applyDeflection( Ld );
      m_Surface[ Side::Back ]->applyDeflection( Ld );
    }
  }

  void CTarIGUSolidLayer::setLayerState( double const t_Tf, double const t_Tb, 
    double const t_Jf, double const t_Jb ) {
    setSurfaceState( t_Tf, t_Jf, Side::Front );
    setSurfaceState( t_Tb, t_Jb, Side::Back );
    if ( m_NextLayer != nullptr ) {
      m_NextLayer->resetCalculated();
    }
    if( m_PreviousLayer != nullptr ) {
      m_PreviousLayer->resetCalculated();
    }
  }

  void CTarIGUSolidLayer::setSurfaceState( double const t_Temperature, double const t_J, Side const t_Position ) {
    shared_ptr< CTarSurface > aSurface = m_Surface.at( t_Position );
    aSurface->setTemperature( t_Temperature );
    aSurface->setJ( t_J );

    resetCalculated();
  }

  void CTarIGUSolidLayer::setSolarRadiation( double const t_SolarRadiation ) {
    m_LayerGainFlow = t_SolarRadiation * m_SolarAbsorptance;
    resetCalculated();
  }

  void CTarIGUSolidLayer::setSolarAbsorptance( double const t_SolarAbsorptance ) {
    m_SolarAbsorptance = t_SolarAbsorptance;
    resetCalculated();
  }

  void CTarIGUSolidLayer::setYoungsModulus( const double t_Modulus ) {
    m_YoungsModulus = t_Modulus;
  }

  void CTarIGUSolidLayer::setPoisonRatio( const double t_Ratio ) {
    m_PoisonRatio = t_Ratio;
  }

  void CTarIGUSolidLayer::initializeStateVariables() {
    // Initialization of intermediate calculation variables goes here
  }

  double CTarIGUSolidLayer::LdMean( const double t_P, const double t_D ) const {
    double coeff1 = 16 * t_P / ( pow( M_PI, 6 ) * t_D );
    double totalSum = 0;
    for( size_t m = 1; m <= 5; m += 2 ) {
      for( size_t n = 1; n <= 5; n += 2 ) {
        double coeff2 = m * m * n * n * M_PI * M_PI * pow( pow( m / m_Width, 2 ) + pow( n / m_Height, 2), 2 );
        totalSum += 4 / coeff2;
      }
    }
    totalSum = coeff1 * totalSum;
    return totalSum;
  }

}