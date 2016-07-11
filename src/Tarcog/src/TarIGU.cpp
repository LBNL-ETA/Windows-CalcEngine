#include <vector>
#include <memory>
#include <algorithm>
#include <assert.h>
#include <stdexcept>
#include <iostream>

#include "TarIGU.hpp"
#include "BaseIGUTarcogLayer.hpp"
#include "TarIGUSolidLayer.hpp"
#include "TarIGUGapLayer.hpp"
#include "TarSurface.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace Tarcog {
  CTarIGU::CTarIGU( double t_Width, double t_Height, double t_Tilt ) : 
    m_Width( t_Width ), m_Height( t_Height ), m_Tilt( t_Tilt ) {
  }

  CTarIGU::~CTarIGU() {
    for( shared_ptr< CBaseIGUTarcogLayer > layer : m_Layers )
    {
      layer->tearDownConnections();
    }
  }

  void CTarIGU::addLayer( shared_ptr< CBaseIGUTarcogLayer > t_Layer ) {

    // pushes only solid layers to array. Gap layers are connected via linked list
    // In case this is first layer then it must be a solid layer in order to create IGU
    if ( m_Layers.size() == 0 ) {
      if ( dynamic_pointer_cast< CTarIGUSolidLayer > ( t_Layer ) != NULL ) {
        m_SolidLayers.push_back( dynamic_pointer_cast< CTarIGUSolidLayer >( t_Layer ) );
        m_Layers.push_back( t_Layer );
      } else {
         throw runtime_error( "First inserted layer must be a solid layer." );
      }
    } else {
      shared_ptr< CBaseIGUTarcogLayer > lastLayer = m_Layers.back();
      if ( dynamic_pointer_cast< CTarIGUSolidLayer > ( t_Layer ) != dynamic_pointer_cast< CTarIGUSolidLayer > ( lastLayer ) ) {
        if( dynamic_pointer_cast< CTarIGUSolidLayer > ( t_Layer ) != NULL ) {
          m_SolidLayers.push_back( dynamic_pointer_cast< CTarIGUSolidLayer > ( t_Layer ) );
        }
        m_Layers.push_back(t_Layer);
        lastLayer->connectToBackSide(t_Layer);
      } else {
        throw runtime_error( "Two adjecent layers in IGU cannot be of same type. IGU must be constructed of array of solid and gap layers." );
      }
    }
    t_Layer->setTilt( m_Tilt );
    t_Layer->setWidth( m_Width );
    t_Layer->setHeight( m_Height );
  }

  void CTarIGU::setTilt( double const t_Tilt ) {
    for( shared_ptr< CBaseIGUTarcogLayer > layer : m_Layers ) {
      layer->setTilt( t_Tilt );
    }
  }

  void CTarIGU::setWidth( double const t_Width ) {
    for( shared_ptr< CBaseIGUTarcogLayer > layer : m_Layers ) {
      layer->setWidth( t_Width );
    }
  }

  void CTarIGU::setHeight( double const t_Height ) {
    for( shared_ptr< CBaseIGUTarcogLayer > layer : m_Layers ) {
      layer->setHeight( t_Height );
    }
  }

  void CTarIGU::setSolarRadiation( double const t_SolarRadiation ) {
    for( shared_ptr< CTarIGUSolidLayer > layer : m_SolidLayers ) {
      layer->setSolarRadiation( t_SolarRadiation );
    }
  }

  shared_ptr< CBaseTarcogLayer > CTarIGU::getFirstLayer() const {
    return m_Layers.front();
  }

  shared_ptr< CBaseTarcogLayer > CTarIGU::getLastLayer() const {
    return m_Layers.back();
  }

  shared_ptr< vector< double > > CTarIGU::getState() {
    shared_ptr< vector< double > > aState = make_shared< vector< double > >();
    shared_ptr< CTarSurface > aSurface = nullptr;

    for( shared_ptr< CTarIGUSolidLayer > layer : m_SolidLayers ) {
      aSurface = layer->getSurface( Side::Front );
      assert( aSurface != nullptr );
      aState->push_back( aSurface->getTemperature() );
      aState->push_back( aSurface->J() );
      aSurface = layer->getSurface( Side::Back );
      assert( aSurface != nullptr );
      aState->push_back( aSurface->J() );
      aState->push_back( aSurface->getTemperature() );
    }

    return aState;
  }

  void CTarIGU::setState( shared_ptr< vector< double > > t_State ) {
    assert( (t_State->size() / 4) == m_SolidLayers.size() );
    for( size_t i = 0; i < m_SolidLayers.size(); ++i ) {
      double Tf = (*t_State)[ 4*i ];
      double Jf = (*t_State)[ 4*i + 1 ];
      double Jb = (*t_State)[ 4*i + 2 ];
      double Tb = (*t_State)[ 4*i + 3 ];
      m_SolidLayers[ i ]->setLayerState( Tf, Tb, Jf, Jb );
    }
  }
  
  double CTarIGU::getThickness() const {
    double totalWidth = 0;

    for( shared_ptr< CBaseIGUTarcogLayer > layer : m_Layers ) {
      totalWidth += layer->getThickness();
    }

    return totalWidth;
  }

  double CTarIGU::getTilt() const {
    return m_Tilt;
  }

  double CTarIGU::getWidth( ) const {
    return m_Width;
  }

  double CTarIGU::getHeight( ) const {
    return m_Height;
  }

  int CTarIGU::getNumOfLayers() const {
    return int( m_SolidLayers.size() );
  }

  void CTarIGU::setInitialGuess( shared_ptr< vector< double > > t_Guess ) {
    if( 2 * m_SolidLayers.size() != t_Guess->size() ) {
      cout << "Number of temperatures in initial guess cannot fit number of layers."
        "Program will use initial guess instead" << endl;
    } else {
      size_t Index = 0;
      for( shared_ptr< CTarIGUSolidLayer > aLayer : m_SolidLayers ) {
        for( Side aSide : EnumSide() ) {
          shared_ptr< CTarSurface > aSurface = aLayer->getSurface( aSide );
          aSurface->initializeStart( ( *t_Guess )[ Index ] );
          ++Index;
        }
      }
    }
  }

  vector< shared_ptr< CTarIGUSolidLayer > > CTarIGU::getSolidLayers() {
    return m_SolidLayers;
  }

  vector< shared_ptr< CBaseIGUTarcogLayer > > CTarIGU::getLayers() {
    return m_Layers;
  }

}