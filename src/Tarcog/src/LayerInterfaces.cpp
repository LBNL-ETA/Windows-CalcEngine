#include <assert.h>
#include <stdexcept>

#include "LayerInterfaces.hpp"
#include "TarSurface.hpp"
#include "TarcogConstants.hpp"
#include "Gas.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;
using namespace Gases;

namespace Tarcog {

  using namespace TarcogConstants;

  //////////////////////////////////////////////////////////////////////////
  //      CLayerState
  //////////////////////////////////////////////////////////////////////////

  CLayerState::CLayerState() : m_StateCalculated( false ) {
  
  }

  void CLayerState::resetCalculated() {
    m_StateCalculated = false;
    initializeStateVariables();
  }

  void CLayerState::setCalculated() {
    m_StateCalculated = true;
  }

  bool CLayerState::isCalculated() {
    return m_StateCalculated;
  }

  //////////////////////////////////////////////////////////////////////////
  //      CLayerGeometry
  //////////////////////////////////////////////////////////////////////////

  CLayerGeometry::CLayerGeometry() : m_Width(DEFAULT_WINDOW_WIDTH), m_Height(DEFAULT_WINDOW_HEIGHT),
	  m_Tilt(DEFAULT_TILT) {

  }

  void CLayerGeometry::setWidth( double const t_Width ) {
    m_Width = t_Width;
    resetCalculated();
  }

  void CLayerGeometry::setHeight( double const t_Height ) {
    m_Height = t_Height;
    resetCalculated();
  }

  void CLayerGeometry::setTilt( double const t_Tilt ) {
    m_Tilt = t_Tilt;
    resetCalculated();
  }

  //////////////////////////////////////////////////////////////////////////
  //      CLayerHeatFlow
  //////////////////////////////////////////////////////////////////////////

  CLayerHeatFlow::CLayerHeatFlow() : m_ConductiveConvectiveCoeff(0), m_LayerGainFlow(0) {
    m_Surface[ Side::Front ] = nullptr;
    m_Surface[ Side::Back ] = nullptr;
  }

  CLayerHeatFlow::CLayerHeatFlow( const shared_ptr< CTarSurface >& t_FrontSurface, 
    const shared_ptr< CTarSurface >& t_BackSurface ) :
    m_ConductiveConvectiveCoeff( 0 ), m_LayerGainFlow( 0 ) {
    m_Surface[ Side::Front ] = t_FrontSurface;
    m_Surface[ Side::Back ] = t_BackSurface;
  }

  double CLayerHeatFlow::getHeatFlow() {
    return getRadiationFlow() + getConvectionConductionFlow();
  }

  double CLayerHeatFlow::getGainFlow() {
    calculateLayerState();
    return m_LayerGainFlow;
  }

  double CLayerHeatFlow::getConductionConvectionCoefficient() {
    calculateLayerState();
    return m_ConductiveConvectiveCoeff;
  }

  double CLayerHeatFlow::getRadiationFlow() {
    calculateRadiationState();
    assert( m_Surface.at( Side::Front ) != nullptr );
    assert( m_Surface.at( Side::Back ) != nullptr );
    return m_Surface.at( Side::Back )->J() - m_Surface.at( Side::Front )->J();
  }

  double CLayerHeatFlow::getConvectionConductionFlow() {
    calculateLayerState();
    assert( m_Surface.at( Side::Front ) != nullptr );
    assert( m_Surface.at( Side::Back ) != nullptr );
    return ( m_Surface.at( Side::Back )->getTemperature() - m_Surface.at( Side::Front )->getTemperature() ) * m_ConductiveConvectiveCoeff;
  }

  void CLayerHeatFlow::calculateLayerState() {
    if( !isCalculated() ) {
      calculateRadiationState();
      calculateConvectionConductionState();
    }
    setCalculated();
  }

  shared_ptr< CTarSurface > CLayerHeatFlow::getSurface( Side const t_Position ) const {
    return m_Surface.at( t_Position );
  }

  void CLayerHeatFlow::setSurface( shared_ptr< CTarSurface > t_Surface, 
    Side const t_Position ) {
    m_Surface[ t_Position ] = t_Surface;
    resetCalculated();
  }


  //////////////////////////////////////////////////////////////////////////
  //      CGasLayer
  //////////////////////////////////////////////////////////////////////////

  CGasLayer::CGasLayer() : m_Pressure(0), m_AirSpeed(0),
    m_AirVerticalDirection(AirVerticalDirection::None), m_AirHorizontalDirection(AirHorizontalDirection::None) {
	  onCreate();
  }

  CGasLayer::CGasLayer( double const t_Pressure ) : m_Pressure( t_Pressure ), m_AirSpeed( 0 ),
    m_AirVerticalDirection( AirVerticalDirection::None ),
    m_AirHorizontalDirection( AirHorizontalDirection::None ) {
    onCreate();
  }

  CGasLayer::CGasLayer(double const t_Pressure, double const t_AirSpeed, 
    AirVerticalDirection const t_AirVerticalDirection) :
    m_Pressure(t_Pressure), m_AirSpeed(t_AirSpeed),
	  m_AirVerticalDirection(t_AirVerticalDirection), m_AirHorizontalDirection(AirHorizontalDirection::None) {
	  onCreate();
  }

  CGasLayer::CGasLayer(double const t_Pressure, double const t_AirSpeed, 
    AirHorizontalDirection const t_AirHorizontalDirection) :
    m_Pressure(t_Pressure), m_AirSpeed(t_AirSpeed), m_AirVerticalDirection(AirVerticalDirection::None), 
    m_AirHorizontalDirection(t_AirHorizontalDirection) {
    onCreate();
  }

  CGasLayer::CGasLayer(double const t_Pressure, shared_ptr< CGas > t_Gas) : m_Pressure(t_Pressure), m_AirSpeed(0),
    m_AirVerticalDirection(AirVerticalDirection::None), m_AirHorizontalDirection(AirHorizontalDirection::None) {
    m_Gas = t_Gas;
    onCreate();
  }

  double CGasLayer::getPressure() {
    return m_Pressure;
  }

  void CGasLayer::initializeStateVariables() {
    assert( m_Gas != nullptr );
    m_Gas->setTemperatureAndPressure( getGasTemperature(), m_Pressure );
  }

  void CGasLayer::onCreate() {

    // new gas will be created only if not passed from constructor.
    if( m_Gas == nullptr ) {
      m_Gas = make_shared< Gases::CGas >();
    }
    m_ForcedVentilation = ForcedVentilation();
  }

}