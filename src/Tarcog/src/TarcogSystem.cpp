#include <memory>
#include <vector>
#include <stdexcept>
#include <assert.h>

#include "TarcogSystem.hpp"
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

  CTarcogSystem::CTarcogSystem( shared_ptr< CTarIGU > t_IGU,
    shared_ptr< CTarEnvironment > t_Indoor, shared_ptr< CTarEnvironment > t_Outdoor ) :
    m_IGU( t_IGU ), m_Indoor( t_Indoor ), m_Outdoor( t_Outdoor ) {

    if( t_IGU == nullptr ) {
      throw runtime_error( "IGU has not been assigned to the system. Null value passed." );
    }

    if( t_Indoor == nullptr ) {
      throw runtime_error( "Indoor environment has not been assigned to the system. Null value passed." );
    }

    if( t_Outdoor == nullptr ) {
      throw runtime_error( "Outdoor environment has not been assigned to the system. Null value passed." );
    }
    
    shared_ptr< CBaseTarcogLayer > aLastLayer = m_IGU->getLastLayer();
    m_Indoor->connectToIGULayer( aLastLayer );
    m_Indoor->setTilt( m_IGU->getTilt() );
    m_Indoor->setWidth( m_IGU->getWidth() );
    m_Indoor->setHeight( m_IGU->getHeight() );

    shared_ptr< CBaseTarcogLayer > aFirstLayer = m_IGU->getFirstLayer();
    m_Outdoor->connectToIGULayer( aFirstLayer );
    m_Outdoor->setTilt( m_IGU->getTilt() );
    m_Outdoor->setWidth( m_IGU->getWidth() );
    m_Outdoor->setHeight( m_IGU->getHeight() );

    double solarRadiation = t_Outdoor->getDirectSolarRadiation();
    m_IGU->setSolarRadiation( solarRadiation );

    initializeStartValues();

    m_NonLinearSolver = make_shared< CTarNonLinearSolver >( m_IGU );
  }

  shared_ptr< CTarEnvironment > CTarcogSystem::getIndoor() const {
    return m_Indoor;
  }

  shared_ptr< CTarEnvironment > CTarcogSystem::getOutdoor() const {
    return m_Outdoor;
  }

  vector< shared_ptr < CBaseIGUTarcogLayer > > CTarcogSystem::getLayers() const {
    return m_IGU->getLayers();
  }

  vector< shared_ptr< CTarIGUSolidLayer > > CTarcogSystem::getSolidLayers() const {
    return m_IGU->getSolidLayers();
  }

  vector<double> CTarcogSystem::getSurfaceTemperatures() const {
    vector< double > temperatures;
    vector< shared_ptr< CTarIGUSolidLayer > > aSolidLayers = getSolidLayers();
    for( shared_ptr< CTarIGUSolidLayer > aLayer : aSolidLayers ) {
      for( Side aSide : EnumSide() ) {
        temperatures.push_back( aLayer->getTemperature( aSide ) );
      }
    }
    return temperatures;
  }

  std::vector<double> CTarcogSystem::getSurfaceRadiosities() const {
    vector< double > radiosities;
    vector< shared_ptr< CTarIGUSolidLayer > > aSolidLayers = getSolidLayers();
    for( shared_ptr< CTarIGUSolidLayer > aLayer : aSolidLayers ) {
      for( Side aSide : EnumSide() ) {
        radiosities.push_back( aLayer->J( aSide ) );
      }
    }
    return radiosities;
  }

  double CTarcogSystem::getHeatFlow() const {
    assert( m_Indoor != nullptr );
    return m_Indoor->getHeatFlow();
  }

  double CTarcogSystem::getInteriorVentilationFlow() const {
    return m_IGU->getInteriorVentilationFlow();
  }

  void CTarcogSystem::setTolerance( const double t_Tolerance ) {
    assert( m_NonLinearSolver != nullptr );
    m_NonLinearSolver->setTolerance( t_Tolerance );
  }

  size_t CTarcogSystem::getNumberOfIterations() const {
    assert( m_NonLinearSolver != nullptr );
    return m_NonLinearSolver->getNumOfIterations();
  }

  void CTarcogSystem::solve() {
    assert( m_NonLinearSolver != nullptr );
    m_NonLinearSolver->solve();
  }

  void CTarcogSystem::initializeStartValues() {
    const double startX = 0.001;
    double thickness = m_IGU->getThickness() + startX + 0.01;
    double tOut = m_Outdoor->getGasTemperature();
    double tInd = m_Indoor->getGasTemperature();
    
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

  void CTarcogSystem::setInitialGuess( shared_ptr< vector< double > > t_Temperatures ) {
    m_IGU->setInitialGuess( t_Temperatures );
  }

}