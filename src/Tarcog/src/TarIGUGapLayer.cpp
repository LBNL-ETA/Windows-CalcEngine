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
    CBaseIGUTarcogLayer( t_Thickness ), CGasLayer( t_Pressure ) {    
    
  }

  CTarIGUGapLayer::CTarIGUGapLayer( double t_Thickness, double t_Pressure, const shared_ptr< CGas >& t_Gas ) : 
    CBaseIGUTarcogLayer( t_Thickness ), CGasLayer( t_Pressure, t_Gas ) {
    assert( m_Gas != nullptr );
  }

  CTarIGUGapLayer::CTarIGUGapLayer( const CTarIGUGapLayer& t_Layer ) : 
    CLayerState( t_Layer ), CBaseIGUTarcogLayer( t_Layer ), CGasLayer( t_Layer ) {
    
  }

  void CTarIGUGapLayer::connectToBackSide( const shared_ptr< CBaseTarcogLayer >& t_Layer ) {
    CBaseTarcogLayer::connectToBackSide( t_Layer );
    m_Surface.at( Side::Back ) = t_Layer->getSurface( Side::Front );
  }

  void CTarIGUGapLayer::initializeStateVariables() {
    CGasLayer::initializeStateVariables();
  }

  void CTarIGUGapLayer::calculateConvectionConductionState() {
    checkNextLayer();
    if( !isCalculated() ) {
      if ( getThickness() == 0 ) {
        throw runtime_error( "Layer thickness is set to zero." );
      }

      convectiveH();
    }
  }

  void CTarIGUGapLayer::checkNextLayer() {
    if( m_NextLayer != nullptr ) {
      m_NextLayer->getGainFlow();
    }
  }

  double CTarIGUGapLayer::layerTemperature() {
    return averageTemperature();
  }

  double CTarIGUGapLayer::calculateRayleighNumber() {
    using ConstantsData::GRAVITYCONSTANT;

    double tGapTemperature = layerTemperature();
    double deltaTemp = fabs( getSurface( Side::Back )->getTemperature() - 
      getSurface( Side::Front )->getTemperature() );

    shared_ptr< GasProperties > aProperties = m_Gas->getGasProperties();

    double Ra = 0;
    if( aProperties->m_Viscosity != 0 ) { // if viscosity is zero then it is vacuum
      Ra = GRAVITYCONSTANT * pow( getThickness(), 3 ) * deltaTemp *
        aProperties->m_SpecificHeat * pow( aProperties->m_Density, 2 ) /
        ( tGapTemperature * aProperties->m_Viscosity * aProperties->m_ThermalConductivity );
    }

    return Ra;
  }

  double CTarIGUGapLayer::aspectRatio() {
    if( getThickness() == 0 ) {
      throw runtime_error( "Gap thickness is set to zero." );
    }
    return m_Height / getThickness();
  }

  double CTarIGUGapLayer::convectiveH() {
    double tGapTemperature = layerTemperature();
    m_Gas->setTemperatureAndPressure( tGapTemperature, getPressure() );
    double Ra = calculateRayleighNumber();
    double Asp = aspectRatio();
    shared_ptr< CNusseltNumber > nusseltNumber = make_shared< CNusseltNumber > ();
    shared_ptr< GasProperties > aProperties = m_Gas->getGasProperties();
    if( aProperties->m_Viscosity != 0 ) {
      m_ConductiveConvectiveCoeff = nusseltNumber->calculate( m_Tilt, Ra, Asp ) *
        aProperties->m_ThermalConductivity / getThickness();
    } else { // vacuum state
      m_ConductiveConvectiveCoeff = aProperties->m_ThermalConductivity;
    }
    if( m_AirSpeed != 0 ) {
      m_ConductiveConvectiveCoeff = m_ConductiveConvectiveCoeff + 2 * m_AirSpeed;
    }

    return m_ConductiveConvectiveCoeff;
  }

  double CTarIGUGapLayer::getGasTemperature() {
    return layerTemperature();
  }

  double CTarIGUGapLayer::averageTemperature() {
    double aveTemp = 0;
    if( areSurfacesInitalized() ) {
      aveTemp = ( getSurface( Side::Front )->getTemperature() +
        getSurface( Side::Back )->getTemperature() ) / 2;
    }

    return aveTemp;
  }

  double CTarIGUGapLayer::getPressure() {
    return m_Pressure;
  }

}
