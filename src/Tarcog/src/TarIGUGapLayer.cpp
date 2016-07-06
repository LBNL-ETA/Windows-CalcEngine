#define _USE_MATH_DEFINES
#include <math.h>
#include <stdexcept>
#include <assert.h>
#include <iostream>

#include "TarBaseShade.hpp"
#include "TarIGUGapLayer.hpp"
#include "GasProperties.hpp"
#include "TarSurface.hpp"
#include "Constants.hpp"
#include "NusseltNumber.hpp"
#include "Gas.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace Gases;
using namespace FenestrationCommon;

namespace Tarcog {

  class CBaseTarcogLayer;

  CTarIGUGapLayer::CTarIGUGapLayer( double t_Thickness, double t_Pressure ) : 
    CBaseIGUTarcogLayer( t_Thickness ), CGasLayer( t_Pressure ), m_inTemperature( 0 ), 
    m_outTemperature( 0 ), m_Zin( 0 ), m_Zout( 0 ), m_ReferenceGas( make_shared< Gases::CGas >() ) {    
    m_ReferenceGas->setTemperatureAndPressure( ReferenceTemperature, m_Pressure );
  }

  CTarIGUGapLayer::CTarIGUGapLayer( double t_Thickness, double t_Pressure, shared_ptr< CGas > t_Gas ) : 
    CBaseIGUTarcogLayer( t_Thickness ), CGasLayer( t_Pressure, t_Gas ), 
    m_inTemperature ( 0 ), m_outTemperature( 0 ), m_Zin( 0 ), m_Zout( 0 ) {
    assert( m_Gas != nullptr );
    m_ReferenceGas = make_shared< Gases::CGas >();
    (*m_ReferenceGas) = (*m_Gas);
    m_ReferenceGas->setTemperatureAndPressure( ReferenceTemperature, m_Pressure );
  }

  void CTarIGUGapLayer::connectToBackSide( shared_ptr< CBaseTarcogLayer > t_Layer ) {
    CBaseTarcogLayer::connectToBackSide( t_Layer );
    m_BackSurface = t_Layer->getSurface( Side::Front );
  }

  void CTarIGUGapLayer::initializeStateVariables() {
    CGasLayer::initializeStateVariables();
  }

  void CTarIGUGapLayer::calculateConvectionConductionState() {
    checkNextLayer();
    if( !isCalculated() ) {
      if ( m_Thickness == 0 ) {
        throw runtime_error( "Solid layer thickness is set to zero." );
      }

      assert( m_BackSurface != nullptr );
      assert( m_FrontSurface != nullptr );

      convectiveH();
      ventilatedFlow();
    }
  }

  void CTarIGUGapLayer::checkNextLayer() {
    if( m_NextLayer != nullptr ) {
      if( dynamic_pointer_cast< CTarIGUShadeLayer >( m_NextLayer ) != NULL ) {
        dynamic_pointer_cast< CTarIGUShadeLayer >( m_NextLayer )->getGainFlow();
      }
    }
  }

  double CTarIGUGapLayer::layerTemperature() {
    assert( m_Height != 0 );
    double cHeight = characteristicHeight();
    double avTemp = averageTemperature();
    return avTemp - ( cHeight / m_Height ) * ( m_outTemperature - m_inTemperature );
  }

  double CTarIGUGapLayer::calculateRayleighNumber() {
    using ConstantsData::GRAVITYCONSTANT;

    double tGapTemperature = layerTemperature();
    double deltaTemp = fabs( m_BackSurface->getTemperature() - m_FrontSurface->getTemperature() );

    shared_ptr< GasProperties > aProperties = m_Gas->getGasProperties();

    double Ra = GRAVITYCONSTANT * pow( m_Thickness, 3 ) * deltaTemp * 
      aProperties->m_SpecificHeat * pow( aProperties->m_Density, 2 ) / 
      ( tGapTemperature * aProperties->m_Viscosity * aProperties->m_ThermalConductivity );

    return Ra;
  }

  double CTarIGUGapLayer::aspectRatio() {
    if( m_Thickness == 0 ) {
      throw runtime_error( "Gap thickness is set to zero." );
    }
    return m_Height / m_Thickness;
  }

  double CTarIGUGapLayer::convectiveH() {
    double tGapTemperature = layerTemperature();
    m_Gas->setTemperatureAndPressure( tGapTemperature, m_Pressure );
    double Ra = calculateRayleighNumber();
    double Asp = aspectRatio();
    shared_ptr< CNusseltNumber > nusseltNumber = make_shared< CNusseltNumber > ();
    shared_ptr< GasProperties > aProperties = m_Gas->getGasProperties();
    m_ConductiveConvectiveCoeff = nusseltNumber->calculate( m_Tilt, Ra, Asp) * aProperties->m_ThermalConductivity / m_Thickness;
    if( m_AirSpeed != 0 ) {
      m_ConductiveConvectiveCoeff = m_ConductiveConvectiveCoeff + 2 * m_AirSpeed;
    }

    return m_ConductiveConvectiveCoeff;
  }

  void CTarIGUGapLayer::ventilatedFlow() {
    shared_ptr< GasProperties > aProperties = m_Gas->getGasProperties();
    m_LayerGainFlow = aProperties->m_Density * aProperties->m_SpecificHeat * m_AirSpeed * m_Thickness *
      m_Width * ( m_inTemperature - m_outTemperature );
  }

  double CTarIGUGapLayer::getAirTemperature() {
    return layerTemperature();
  }

  double CTarIGUGapLayer::averageTemperature() {
    double aveTemp = 0;
    if( m_FrontSurface != nullptr && m_BackSurface != nullptr ) {
      aveTemp = ( m_FrontSurface->getTemperature() + m_BackSurface->getTemperature() ) / 2;
    }
    return aveTemp;
  }

  double CTarIGUGapLayer::characteristicHeight() {
    shared_ptr< GasProperties > aProperties = m_Gas->getGasProperties();
    double cHeight = 0;
    // Characteristic height can only be calculated after initialization is performed
    if( m_ConductiveConvectiveCoeff != 0 ) {
      cHeight = aProperties->m_Density * aProperties->m_SpecificHeat * m_Thickness * m_AirSpeed / 
        ( 4 * m_ConductiveConvectiveCoeff );
    }
    return cHeight;
  }

  double CTarIGUGapLayer::bernoullyPressureTerm() {
    shared_ptr< GasProperties > aGasProperties = m_Gas->getGasProperties();
    return 0.5 * aGasProperties->m_Density;
  }

  double CTarIGUGapLayer::hagenPressureTerm() {
    shared_ptr< GasProperties > aGasProperties = m_Gas->getGasProperties();
    return 12 * aGasProperties->m_Viscosity * m_Height / pow( m_Thickness, 2 );
  }

  double CTarIGUGapLayer::pressureLossTerm() {
    shared_ptr< GasProperties > aGasProperties = m_Gas->getGasProperties();
    return 0.5 * aGasProperties->m_Density * ( m_Zin + m_Zout );
  }

  double CTarIGUGapLayer::betaCoeff() {
    calculateLayerState();
    return exp( -m_Height / characteristicHeight() );
  }

  void CTarIGUGapLayer::setFlowGeometry( double const t_Atop, double const t_Abot, AirVerticalDirection const &t_Direction ) {
    m_AirVerticalDirection = t_Direction;
    double Ain = 0;
    double Aout = 0;
    switch ( m_AirVerticalDirection )
    {
    case AirVerticalDirection::None:
      // Do nothing. Airflow does not exist
      break;
    case AirVerticalDirection::Up:
      Ain = t_Abot;
      Aout = t_Atop;
      break;
    case AirVerticalDirection::Down:
      Ain = t_Atop;
      Aout = t_Abot;
      break;
    default:
      throw runtime_error( "Incorrect assignment for airflow direction." );
      break;
    }

    m_Zin = calcImpedance( Ain );
    m_Zout = calcImpedance( Aout );

    resetCalculated();
  }

  void CTarIGUGapLayer::setFlowTemperatures( double const t_topTemp, double const t_botTemp, 
    AirVerticalDirection const &t_Direction ) {
    m_AirVerticalDirection = t_Direction;
    switch ( m_AirVerticalDirection )
    {
    case AirVerticalDirection::None:
      break;
    case AirVerticalDirection::Up:
      m_inTemperature = t_botTemp;
      m_outTemperature = t_topTemp;
      break;
    case AirVerticalDirection::Down:
      m_inTemperature = t_topTemp;
      m_outTemperature = t_botTemp;
      break;
    default:
      throw runtime_error("Incorrect argument for airflow direction.");
      break;
    }

    resetCalculated();
  }

  void CTarIGUGapLayer::setFlowSpeed( double const t_speed ) {
    m_AirSpeed = t_speed;
    resetCalculated();
  }

  double CTarIGUGapLayer::getAirflowReferencePoint( double const t_GapTemperature ) {
    using ConstantsData::GRAVITYCONSTANT;

    double tiltAngle = M_PI / 180 * ( m_Tilt - 90 );
    double gapTemperature = layerTemperature();
    shared_ptr< GasProperties > aProperties = m_ReferenceGas->getGasProperties();
    double temperatureMultiplier = fabs( gapTemperature - t_GapTemperature ) / ( gapTemperature * t_GapTemperature );
    return aProperties->m_Density * ReferenceTemperature * GRAVITYCONSTANT * m_Height * 
      fabs(cos(tiltAngle)) * temperatureMultiplier;
  }

  double CTarIGUGapLayer::calcImpedance( double const t_A ) {
   double impedance = 0;

   if( t_A != 0 ) {
    impedance = pow( m_Width * m_Thickness / ( 0.6 * t_A ) - 1, 2 );
   }

   return impedance;
  }

  void CTarIGUGapLayer::smoothEnergy( double const qv1, double const qv2 ) {
    double smooth = ( fabs( qv1 ) + fabs( qv2 ) ) / 2;
    m_LayerGainFlow = smooth;
    if ( m_inTemperature < m_outTemperature ) {
      m_LayerGainFlow = -m_LayerGainFlow;
    }
  }

}
