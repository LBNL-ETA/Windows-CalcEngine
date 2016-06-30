#include <memory>
#include <vector>
#include <stdexcept>
#include <assert.h>

#include "TarcogSystem.hpp"
#include "BaseTarcogLayer.hpp"
#include "BaseIGUTarcogLayer.hpp"
#include "TarIGU.hpp"
#include "TarOutdoorEnvironment.hpp"
#include "TarIndoorEnvironment.hpp"
#include "TarSurface.hpp"
#include "TarNonLinearSolver.hpp"

using namespace std;

namespace Tarcog {

  CTarcogSystem::CTarcogSystem( shared_ptr< CTarIGU > t_IGU,
                   shared_ptr< CTarEnvironment > t_Indoor,
                   shared_ptr< CTarEnvironment > t_Outdoor ) :
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

  void CTarcogSystem::solve() {
    assert( m_NonLinearSolver != nullptr );
    m_NonLinearSolver->solve();
  }

  void CTarcogSystem::initializeStartValues() {
    const double startX = 0.001;
    double thickness = m_IGU->getThickness() + startX + 0.01;
    double tOut = m_Outdoor->getAirTemperature();
    double tInd = m_Indoor->getAirTemperature();
    
    double deltaTemp = ( tInd - tOut ) / thickness;

    vector< shared_ptr< CBaseIGUTarcogLayer > > aLayers = m_IGU->getLayers();

    shared_ptr< CBaseIGUTarcogLayer > aLayer = aLayers.front();
    double currentXPosition = startX;
    shared_ptr< CTarSurface > aSurface = aLayer->getSurface( SurfacePosition::Front );
    double curTemp = tOut + currentXPosition * deltaTemp;

    aSurface->intializeStart( curTemp );

    for( shared_ptr< CBaseIGUTarcogLayer > layer : aLayers ) {
      currentXPosition += layer->getThickness();
      curTemp = tOut + currentXPosition * deltaTemp;
      aSurface = layer->getSurface( SurfacePosition::Back );
      aSurface->intializeStart( curTemp );
    }
  }

}