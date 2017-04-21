#include <memory>
#include <vector>
#include <stdexcept>
#include <assert.h>

#include "SingleSystem.hpp"
#include "BaseLayer.hpp"
#include "BaseIGULayer.hpp"
#include "IGUSolidLayer.hpp"
#include "IGU.hpp"
#include "OutdoorEnvironment.hpp"
#include "IndoorEnvironment.hpp"
#include "Surface.hpp"
#include "NonLinearSolver.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace Tarcog {

  CSingleSystem::CSingleSystem( shared_ptr< CIGU > t_IGU,
    shared_ptr< CEnvironment > t_Indoor, shared_ptr< CEnvironment > t_Outdoor ) :
    m_IGU( t_IGU ) {

    m_Environment[ Environment::Indoor ] = t_Indoor;
    m_Environment[ Environment::Outdoor ] = t_Outdoor;

    if( t_IGU == nullptr ) {
      throw runtime_error( "IGU has not been assigned to the system. Null value passed." );
    }

    if( t_Indoor == nullptr ) {
      throw runtime_error( "Indoor environment has not been assigned to the system. Null value passed." );
    }

    if( t_Outdoor == nullptr ) {
      throw runtime_error( "Outdoor environment has not been assigned to the system. Null value passed." );
    }
    
    shared_ptr< CBaseLayer > aIndoorLayer = m_IGU->getLayer( Environment::Indoor );
    shared_ptr< CEnvironment > aIndoor = m_Environment.at( Environment::Indoor );
    aIndoor->connectToIGULayer( aIndoorLayer );
    aIndoor->setTilt( m_IGU->getTilt() );
    aIndoor->setWidth( m_IGU->getWidth() );
    aIndoor->setHeight( m_IGU->getHeight() );

    shared_ptr< CBaseLayer > aOutdoorLayer = m_IGU->getLayer( Environment::Outdoor );
    shared_ptr< CEnvironment > aOutdoor = m_Environment.at( Environment::Outdoor );
    aOutdoor->connectToIGULayer( aOutdoorLayer );
    aOutdoor->setTilt( m_IGU->getTilt() );
    aOutdoor->setWidth( m_IGU->getWidth() );
    aOutdoor->setHeight( m_IGU->getHeight() );

    double solarRadiation = t_Outdoor->getDirectSolarRadiation();
    m_IGU->setSolarRadiation( solarRadiation );

    initializeStartValues();

    m_NonLinearSolver = make_shared< CNonLinearSolver >( m_IGU );
  }

  CSingleSystem::CSingleSystem( const CSingleSystem & t_SingleSystem ) {
    m_IGU = make_shared< CIGU >( *t_SingleSystem.m_IGU );

    m_Environment[ Environment::Indoor ] = t_SingleSystem.m_Environment.at( Environment::Indoor )->cloneEnvironment();
    shared_ptr< CBaseLayer > aLastLayer = m_IGU->getLayer( Environment::Indoor );
    m_Environment.at( Environment::Indoor )->connectToIGULayer( aLastLayer );

    m_Environment[ Environment::Outdoor ] = t_SingleSystem.m_Environment.at( Environment::Outdoor )->cloneEnvironment();
    shared_ptr< CBaseLayer > aFirstLayer = m_IGU->getLayer( Environment::Outdoor );
    m_Environment.at( Environment::Outdoor )->connectToIGULayer( aFirstLayer );

    initializeStartValues();

    m_NonLinearSolver = make_shared< CNonLinearSolver >( m_IGU );
  }

  vector< shared_ptr< CIGUSolidLayer > > CSingleSystem::getSolidLayers() const {
    return m_IGU->getSolidLayers();
  }

  vector< shared_ptr< CIGUGapLayer > > CSingleSystem::getGapLayers() {
    return m_IGU->getGapLayers();
  }

  shared_ptr< vector< double > > CSingleSystem::getTemperatures() const {
    return m_IGU->getTemperatures();
  }

  shared_ptr< vector< double > > CSingleSystem::getRadiosities() const {
    return m_IGU->getRadiosities();
  }

  shared_ptr< vector< double > > CSingleSystem::getMaxDeflections() const {
    return m_IGU->getMaxDeflections();
  }

  shared_ptr< vector< double > > CSingleSystem::getMeanDeflections() const {
    return m_IGU->getMeanDeflections();
  }

  shared_ptr< CSingleSystem > CSingleSystem::clone() const {
    return make_shared< CSingleSystem >( *this );
  }

  double CSingleSystem::getHeatFlow( const Environment t_Environment ) const {
    return m_Environment.at( t_Environment )->getHeatFlow();
  }

  double CSingleSystem::getConvectiveHeatFlow( const Environment t_Environment ) const {
    return m_Environment.at( t_Environment )->getConvectionConductionFlow();
  }

  double CSingleSystem::getRadiationHeatFlow( const Environment t_Environment ) const {
    return m_Environment.at( t_Environment )->getRadiationFlow();
  }

  double CSingleSystem::getHc( const Environment t_Environment ) const {
    return m_Environment.at( t_Environment )->getHc();
  }

  double CSingleSystem::getAirTemperature( const Environment t_Environment ) const {
    return m_Environment.at( t_Environment )->getAirTemperature();
  }

  double CSingleSystem::getVentilationFlow( const Environment t_Environment ) const {
    return m_IGU->getVentilationFlow( t_Environment );
  }

  double CSingleSystem::getUValue() const {
    double interiorAir = m_Environment.at( Environment::Indoor )->getAmbientTemperature();
    double outdoorAir = m_Environment.at( Environment::Outdoor )->getAmbientTemperature();
    return getHeatFlow( Environment::Indoor ) / ( interiorAir - outdoorAir );
  }

  void CSingleSystem::setTolerance( const double t_Tolerance ) {
    assert( m_NonLinearSolver != nullptr );
    m_NonLinearSolver->setTolerance( t_Tolerance );
  }

  size_t CSingleSystem::getNumberOfIterations() const {
    assert( m_NonLinearSolver != nullptr );
    return m_NonLinearSolver->getNumOfIterations();
  }

  void CSingleSystem::solve() {
    assert( m_NonLinearSolver != nullptr );
    m_NonLinearSolver->solve();
  }

  void CSingleSystem::initializeStartValues() {
    const double startX = 0.001;
    double thickness = m_IGU->getThickness() + startX + 0.01;
    double tOut = m_Environment.at( Environment::Outdoor )->getGasTemperature();
    double tInd = m_Environment.at( Environment::Indoor )->getGasTemperature();
    
    double deltaTemp = ( tInd - tOut ) / thickness;

    vector< shared_ptr< CBaseIGULayer > > aLayers = m_IGU->getLayers();

    shared_ptr< CBaseIGULayer > aLayer = aLayers.front();
    double currentXPosition = startX;
    shared_ptr< ISurface > aSurface = aLayer->getSurface( Side::Front );
    double curTemp = tOut + currentXPosition * deltaTemp;

    aSurface->initializeStart( curTemp );

    for( shared_ptr< CBaseIGULayer > layer : aLayers ) {
      currentXPosition += layer->getThickness();
      curTemp = tOut + currentXPosition * deltaTemp;
      aSurface = layer->getSurface( Side::Back );
      aSurface->initializeStart( curTemp );
    }
  }

  void CSingleSystem::setInitialGuess( const shared_ptr< vector< double > >& t_Temperatures ) {
    m_IGU->setInitialGuess( t_Temperatures );
  }

  void CSingleSystem::setSolarRadiation( double const t_SolarRadiation ) {
    dynamic_pointer_cast< COutdoorEnvironment > ( m_Environment.at( Environment::Outdoor ) )->
      setSolarRadiation( t_SolarRadiation );
    m_IGU->setSolarRadiation( t_SolarRadiation );
  }

  double CSingleSystem::getSolarRadiation() const {
    return dynamic_pointer_cast< COutdoorEnvironment > ( m_Environment.at( Environment::Outdoor ) )->
      getSolarRadiation();
  }

}