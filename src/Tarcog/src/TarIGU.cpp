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
#include "TarIGUSolidDeflection.hpp"
#include "TarIGUGapDeflection.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace Tarcog {
  CTarIGU::CTarIGU( double t_Width, double t_Height, double t_Tilt ) : 
    m_Width( t_Width ), m_Height( t_Height ), m_Tilt( t_Tilt ) {
  }

  CTarIGU::~CTarIGU() {
    for( shared_ptr< CBaseIGUTarcogLayer > layer : getSolidLayers() ) {
      layer->tearDownConnections();
    }
  }

  void CTarIGU::addLayer( shared_ptr< CBaseIGUTarcogLayer > t_Layer ) {

    // pushes only solid layers to array. Gap layers are connected via linked list
    // In case this is first layer then it must be a solid layer in order to create IGU
    if ( getNumOfLayers() == 0 ) {
      if ( dynamic_pointer_cast< CTarIGUSolidLayer > ( t_Layer ) != NULL ) {
        m_Layers.push_back( t_Layer );
      } else {
         throw runtime_error( "First inserted layer must be a solid layer." );
      }
    } else {
      shared_ptr< CBaseIGUTarcogLayer > lastLayer = m_Layers.back();
      if ( dynamic_pointer_cast< CTarIGUSolidLayer > ( t_Layer ) != 
        dynamic_pointer_cast< CTarIGUSolidLayer > ( lastLayer ) ) {
        m_Layers.push_back(t_Layer);
        lastLayer->connectToBackSide(t_Layer);
      } else {
        throw runtime_error( "Two adjecent layers in IGU cannot be of same type. "
          "IGU must be constructed of array of solid and gap layers." );
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
    for( shared_ptr< CTarIGUSolidLayer >& layer : getSolidLayers() ) {
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

    for( shared_ptr< CTarIGUSolidLayer >& layer : getSolidLayers() ) {
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
    size_t i = 0;
    for( shared_ptr< CTarIGUSolidLayer >& aLayer : getSolidLayers() ) {
      double Tf = (*t_State)[ 4*i ];
      double Jf = (*t_State)[ 4*i + 1 ];
      double Jb = (*t_State)[ 4*i + 2 ];
      double Tb = (*t_State)[ 4*i + 3 ];
      aLayer->setLayerState( Tf, Tb, Jf, Jb );
      ++i;
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

  size_t CTarIGU::getNumOfLayers() const {
    return ( m_Layers.size() + 1 ) / 2;
  }

  double CTarIGU::getInteriorVentilationFlow() const {
    size_t size = m_Layers.size();
    // needs to get gas layer on indoor side
    return m_Layers[ size - 2 ]->getGainFlow();
  }

  void CTarIGU::setInitialGuess( const shared_ptr< vector< double > >& t_Guess ) {
    if( 2 * getNumOfLayers() != t_Guess->size() ) {
      cout << "Number of temperatures in initial guess cannot fit number of layers."
        "Program will use initial guess instead" << endl;
    } else {
      size_t Index = 0;
      for( shared_ptr< CTarIGUSolidLayer >& aLayer : getSolidLayers() ) {
        for( Side aSide : EnumSide() ) {
          shared_ptr< CTarSurface > aSurface = aLayer->getSurface( aSide );
          aSurface->initializeStart( ( *t_Guess )[ Index ] );
          ++Index;
        }
      }
    }
  }

  void CTarIGU::setDeflectionProperties( const double t_Tini, const double t_Pini ) {
    // Simply decorating layers in a list with new behavior
    vector< shared_ptr< CTarIGUSolidLayer > > aVector = getSolidLayers();
    for( shared_ptr< CTarIGUSolidLayer >& aLayer : getSolidLayers() ) {
      // Deflection could aslo be decorated outside in which case program already have a layer as
      // deflection layer. If that is not done then layer must be decorated with defalut deflection
      // properties
      if( dynamic_pointer_cast< CTarIGUSolidLayerDeflection >( aLayer ) == NULL ) {
        replaceLayer( aLayer, make_shared< CTarIGUSolidLayerDeflection >( aLayer ) );
      }
    }
    for( shared_ptr< CTarIGUGapLayer >& aLayer : getGapLayers() ) {
      replaceLayer( aLayer, make_shared< CTarIGUGapLayerDeflection >( aLayer, t_Tini, t_Pini ) );
    }
  }

  void CTarIGU::replaceLayer( shared_ptr< CBaseIGUTarcogLayer > t_Original, 
    shared_ptr< CBaseIGUTarcogLayer > t_Replacement ) {
    size_t index = find( m_Layers.begin(), m_Layers.end(), t_Original ) - m_Layers.begin();
    m_Layers[ index ] = t_Replacement;
    if (index > 0) {
      m_Layers[ index - 1 ]->connectToBackSide( t_Replacement );
    }
    if ( index < m_Layers.size() - 1 ) {
      t_Replacement->connectToBackSide( m_Layers[ index + 1 ] );
    }
  }

  vector< shared_ptr< CTarIGUSolidLayer > > CTarIGU::getSolidLayers() {
    vector< shared_ptr< CTarIGUSolidLayer > > aVect;
    for( shared_ptr< CBaseIGUTarcogLayer >& aLayer : m_Layers ) {
      if( dynamic_pointer_cast< CTarIGUSolidLayer >( aLayer ) != NULL ) {
        aVect.push_back( dynamic_pointer_cast< CTarIGUSolidLayer >( aLayer ) );
      }
    }
    return aVect;
  }

  vector< shared_ptr< CTarIGUGapLayer > > CTarIGU::getGapLayers() {
    vector< shared_ptr< CTarIGUGapLayer > > aVect;
    for( shared_ptr< CBaseIGUTarcogLayer >& aLayer : m_Layers ) {
      if( dynamic_pointer_cast< CTarIGUGapLayer >( aLayer ) != NULL ) {
        aVect.push_back( dynamic_pointer_cast< CTarIGUGapLayer >( aLayer ) );
      }
    }
    return aVect;
  }

  vector< shared_ptr< CBaseIGUTarcogLayer > > CTarIGU::getLayers() const {
    return m_Layers;
  }

}