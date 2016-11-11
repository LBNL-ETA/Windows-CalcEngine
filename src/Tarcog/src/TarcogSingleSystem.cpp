#include <memory>
#include <vector>
#include <stdexcept>
#include <assert.h>

#include "TarcogSingleSystem.hpp"
#include "BaseTarcogLayer.hpp"
#include "BaseIGUTarcogLayer.hpp"
#include "TarIGUSolidLayer.hpp"
#include "TarIGU.hpp"
#include "TarOutdoorEnvironment.hpp"
#include "TarIndoorEnvironment.hpp"
#include "TarSurface.hpp"
#include "TarNonLinearSolver.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace Tarcog {

  CTarcogSingleSystem::CTarcogSingleSystem( shared_ptr< CTarIGU > t_IGU,
    shared_ptr< CTarEnvironment > t_Indoor, shared_ptr< CTarEnvironment > t_Outdoor ) :
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
    
    shared_ptr< CBaseTarcogLayer > aIndoorLayer = m_IGU->getLayer( Environment::Indoor );
    shared_ptr< CTarEnvironment > aIndoor = m_Environment.at( Environment::Indoor );
    aIndoor->connectToIGULayer( aIndoorLayer );
    aIndoor->setTilt( m_IGU->getTilt() );
    aIndoor->setWidth( m_IGU->getWidth() );
    aIndoor->setHeight( m_IGU->getHeight() );

    shared_ptr< CBaseTarcogLayer > aOutdoorLayer = m_IGU->getLayer( Environment::Outdoor );
    shared_ptr< CTarEnvironment > aOutdoor = m_Environment.at( Environment::Outdoor );
    aOutdoor->connectToIGULayer( aOutdoorLayer );
    aOutdoor->setTilt( m_IGU->getTilt() );
    aOutdoor->setWidth( m_IGU->getWidth() );
    aOutdoor->setHeight( m_IGU->getHeight() );

    double solarRadiation = t_Outdoor->getDirectSolarRadiation();
    m_IGU->setSolarRadiation( solarRadiation );

    initializeStartValues();

    m_NonLinearSolver = make_shared< CTarNonLinearSolver >( m_IGU );
  }

  CTarcogSingleSystem::CTarcogSingleSystem( const CTarcogSingleSystem & t_SingleSystem ) {
    m_IGU = make_shared< CTarIGU >( *t_SingleSystem.m_IGU );

    m_Environment[ Environment::Indoor ] = t_SingleSystem.m_Environment.at( Environment::Indoor )->cloneEnvironment();
    shared_ptr< CBaseTarcogLayer > aLastLayer = m_IGU->getLayer( Environment::Indoor );
    m_Environment.at( Environment::Indoor )->connectToIGULayer( aLastLayer );

    m_Environment[ Environment::Outdoor ] = t_SingleSystem.m_Environment.at( Environment::Outdoor )->cloneEnvironment();
    shared_ptr< CBaseTarcogLayer > aFirstLayer = m_IGU->getLayer( Environment::Outdoor );
    m_Environment.at( Environment::Outdoor )->connectToIGULayer( aFirstLayer );

    initializeStartValues();

    m_NonLinearSolver = make_shared< CTarNonLinearSolver >( m_IGU );
  }

  vector< shared_ptr< CTarIGUSolidLayer > > CTarcogSingleSystem::getSolidLayers() const {
    return m_IGU->getSolidLayers();
  }

  vector< shared_ptr< CTarIGUGapLayer > > CTarcogSingleSystem::getGapLayers() {
    return m_IGU->getGapLayers();
  }

  shared_ptr< vector< double > > CTarcogSingleSystem::getTemperatures() const {
    return m_IGU->getTemperatures();
  }

  shared_ptr< vector< double > > CTarcogSingleSystem::getRadiosities() const {
    return m_IGU->getRadiosities();
  }

  shared_ptr< vector< double > > CTarcogSingleSystem::getMaxDeflections() const {
    return m_IGU->getMaxDeflections();
  }

  shared_ptr< vector< double > > CTarcogSingleSystem::getMeanDeflections() const {
    return m_IGU->getMeanDeflections();
  }

  shared_ptr< CTarcogSingleSystem > CTarcogSingleSystem::clone() const {
    return make_shared< CTarcogSingleSystem >( *this );
  }

  double CTarcogSingleSystem::getHeatFlow( const Environment t_Environment ) const {
    return m_Environment.at( t_Environment )->getHeatFlow();
  }

  double CTarcogSingleSystem::getConvectiveHeatFlow( const Environment t_Environment ) const {
    return m_Environment.at( t_Environment )->getConvectionConductionFlow();
  }

  double CTarcogSingleSystem::getRadiationHeatFlow( const Environment t_Environment ) const {
    return m_Environment.at( t_Environment )->getRadiationFlow();
  }

  double CTarcogSingleSystem::getVentilationFlow( const Environment t_Environment ) const {
    return m_IGU->getVentilationFlow( t_Environment );
  }

  double CTarcogSingleSystem::getUValue() const {
    double interiorAir = m_Environment.at( Environment::Indoor )->getAmbientTemperature();
    double outdoorAir = m_Environment.at( Environment::Outdoor )->getAmbientTemperature();
    return getHeatFlow( Environment::Indoor ) / ( interiorAir - outdoorAir );
  }

  void CTarcogSingleSystem::setTolerance( const double t_Tolerance ) {
    assert( m_NonLinearSolver != nullptr );
    m_NonLinearSolver->setTolerance( t_Tolerance );
  }

  size_t CTarcogSingleSystem::getNumberOfIterations() const {
    assert( m_NonLinearSolver != nullptr );
    return m_NonLinearSolver->getNumOfIterations();
  }

  void CTarcogSingleSystem::solve() {
    assert( m_NonLinearSolver != nullptr );
    m_NonLinearSolver->solve();
  }

  void CTarcogSingleSystem::initializeStartValues() {
    const double startX = 0.001;
    double thickness = m_IGU->getThickness() + startX + 0.01;
    double tOut = m_Environment.at( Environment::Outdoor )->getGasTemperature();
    double tInd = m_Environment.at( Environment::Indoor )->getGasTemperature();
    
    double deltaTemp = ( tInd - tOut ) / thickness;

    vector< shared_ptr< CBaseIGUTarcogLayer > > aLayers = m_IGU->getLayers();

    shared_ptr< CBaseIGUTarcogLayer > aLayer = aLayers.front();
    double currentXPosition = startX;
    shared_ptr< CTarSurface > aSurface = aLayer->getSurface( Side::Front );
    double curTemp = tOut + currentXPosition * deltaTemp;

    aSurface->initializeStart( curTemp );

    for( shared_ptr< CBaseIGUTarcogLayer > layer : aLayers ) {
      currentXPosition += layer->getThickness();
      curTemp = tOut + currentXPosition * deltaTemp;
      aSurface = layer->getSurface( Side::Back );
      aSurface->initializeStart( curTemp );
    }
  }

  void CTarcogSingleSystem::setInitialGuess( const shared_ptr< vector< double > >& t_Temperatures ) {
    m_IGU->setInitialGuess( t_Temperatures );
  }

  void CTarcogSingleSystem::setSolarRadiation( double const t_SolarRadiation ) {
    dynamic_pointer_cast< CTarOutdoorEnvironment > ( m_Environment.at( Environment::Outdoor ) )->
      setSolarRadiation( t_SolarRadiation );
    m_IGU->setSolarRadiation( t_SolarRadiation );
  }

  double CTarcogSingleSystem::getSolarRadiation() const {
    return dynamic_pointer_cast< CTarOutdoorEnvironment > ( m_Environment.at( Environment::Outdoor ) )->
      getSolarRadiation();
  }

}