#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <memory>
#include <algorithm>
#include <assert.h>
#include <stdexcept>
#include <iostream>

#include "IGU.hpp"
#include "BaseIGULayer.hpp"
#include "IGUSolidLayer.hpp"
#include "IGUGapLayer.hpp"
#include "Surface.hpp"
#include "IGUSolidDeflection.hpp"
#include "IGUGapDeflection.hpp"
#include "IGUVentilatedGapLayer.hpp"
#include "BaseShade.hpp"
#include "Environment.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace Tarcog {
  CIGU::CIGU( const double t_Width, const double t_Height, const double t_Tilt ) : 
    m_Width( t_Width ), m_Height( t_Height ), m_Tilt( t_Tilt ) {
  }

  CIGU::CIGU( const CIGU& t_IGU ) {
    m_Width = t_IGU.m_Width;
    m_Height = t_IGU.m_Height;
    m_Tilt = t_IGU.m_Tilt;
    for( size_t i = 0; i < t_IGU.m_Layers.size(); ++i ) {
      shared_ptr< CBaseIGULayer > aLayer = 
        dynamic_pointer_cast< CBaseIGULayer >( t_IGU.m_Layers[ i ]->clone() );
      addLayer( aLayer );
    }
  }

  CIGU::~CIGU() {
    for( shared_ptr< CBaseIGULayer > layer : getSolidLayers() ) {
      layer->tearDownConnections();
    }
  }

  void CIGU::addLayer( const shared_ptr< CBaseIGULayer >& t_Layer ) {

    // pushes only solid layers to array. Gap layers are connected via linked list
    // In case this is first layer then it must be a solid layer in order to create IGU
    if ( getNumOfLayers() == 0 ) {
      if ( dynamic_pointer_cast< CIGUSolidLayer > ( t_Layer ) != NULL ) {
        m_Layers.push_back( t_Layer );
      } else {
         throw runtime_error( "First inserted layer must be a solid layer." );
      }
    } else {
      shared_ptr< CBaseIGULayer > lastLayer = m_Layers.back();
      if ( dynamic_pointer_cast< CIGUSolidLayer > ( t_Layer ) != 
        dynamic_pointer_cast< CIGUSolidLayer > ( lastLayer ) ) {
        m_Layers.push_back( t_Layer );
        lastLayer->connectToBackSide( t_Layer );
      } else {
        throw runtime_error( "Two adjecent layers in IGU cannot be of same type. "
          "IGU must be constructed of array of solid and gap layers." );
      }
    }

    checkForLayerUpgrades( t_Layer );

    t_Layer->setTilt( m_Tilt );
    t_Layer->setWidth( m_Width );
    t_Layer->setHeight( m_Height );
  }

  void CIGU::setTilt( const double t_Tilt ) {
    for( shared_ptr< CBaseIGULayer > layer : m_Layers ) {
      layer->setTilt( t_Tilt );
    }
  }

  void CIGU::setWidth( const double t_Width ) {
    for( shared_ptr< CBaseIGULayer > layer : m_Layers ) {
      layer->setWidth( t_Width );
    }
  }

  void CIGU::setHeight( const double t_Height ) {
    for( shared_ptr< CBaseIGULayer > layer : m_Layers ) {
      layer->setHeight( t_Height );
    }
  }

  void CIGU::setSolarRadiation( const double t_SolarRadiation ) {
    for( shared_ptr< CIGUSolidLayer >& layer : getSolidLayers() ) {
      layer->setSolarRadiation( t_SolarRadiation );
    }
  }

  shared_ptr<CBaseLayer> CIGU::getLayer( const Environment t_Environment ) const {
    shared_ptr< CBaseLayer > aLayer = nullptr;
    switch( t_Environment ) {
    case Environment::Indoor:
      aLayer = m_Layers.back();
      break;
    case Environment::Outdoor:
      aLayer = m_Layers.front();
      break;
    default:
      assert("Incorrect environment selection.");
      break;
    }
    return aLayer;
  }

  shared_ptr< vector< double > > CIGU::getState() {

    shared_ptr< vector< double > > aState = make_shared< vector< double > >();

    for( shared_ptr< CIGUSolidLayer >& layer : getSolidLayers() ) {
      shared_ptr< ISurface > aSurface = layer->getSurface( Side::Front );
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

  void CIGU::setState( shared_ptr< vector< double > > t_State ) {
    size_t i = 0;
    for( shared_ptr< CIGUSolidLayer >& aLayer : getSolidLayers() ) {
      double Tf = (*t_State)[ 4*i ];
      double Jf = (*t_State)[ 4*i + 1 ];
      double Jb = (*t_State)[ 4*i + 2 ];
      double Tb = (*t_State)[ 4*i + 3 ];
      aLayer->setLayerState( Tf, Tb, Jf, Jb );
      ++i;
    }
  }

  shared_ptr< vector< double > > CIGU::getTemperatures() const {
    shared_ptr< vector< double > > aTemperatures = make_shared< vector< double > >();

    for( const shared_ptr< CIGUSolidLayer >& layer : getSolidLayers() ) {
      for( Side aSide : EnumSide() ) {
        shared_ptr< ISurface > aSurface = layer->getSurface( aSide );
        assert( aSurface != nullptr );
        aTemperatures->push_back( aSurface->getTemperature() );
      }
    }

    return aTemperatures;
  }

  shared_ptr< vector< double > > CIGU::getRadiosities() const {
    shared_ptr< vector< double > > aRadiosities = make_shared< vector< double > >();

    for( const shared_ptr< CIGUSolidLayer >& layer : getSolidLayers() ) {
      for( Side aSide : EnumSide() ) {
        shared_ptr< ISurface > aSurface = layer->getSurface( aSide );
        assert( aSurface != nullptr );
        aRadiosities->push_back( aSurface->J() );
      }
    }

    return aRadiosities;
  }

  shared_ptr< vector< double > > CIGU::getMaxDeflections() const {
    shared_ptr< vector< double > > aMaxDeflections = make_shared< vector< double > >();

    for( const shared_ptr< CIGUSolidLayer >& layer : getSolidLayers() ) {
      aMaxDeflections->push_back( layer->getMaxDeflection() );
    }

    return aMaxDeflections;
  }

  shared_ptr< vector< double > > CIGU::getMeanDeflections() const {
    shared_ptr< vector< double > > aMeanDeflections = make_shared< vector< double > >();

    for( const shared_ptr< CIGUSolidLayer >& layer : getSolidLayers() ) {
      aMeanDeflections->push_back( layer->getMeanDeflection() );
    }

    return aMeanDeflections;
  }
  
  double CIGU::getThickness() const {
    double totalWidth = 0;

    for( shared_ptr< CBaseIGULayer > layer : m_Layers ) {
      totalWidth += layer->getThickness();
    }

    return totalWidth;
  }

  double CIGU::getTilt() const {
    return m_Tilt;
  }

  double CIGU::getWidth( ) const {
    return m_Width;
  }

  double CIGU::getHeight( ) const {
    return m_Height;
  }

  size_t CIGU::getNumOfLayers() const {
    return ( m_Layers.size() + 1 ) / 2;
  }

  double CIGU::getVentilationFlow( const Environment t_Environment ) const {
    size_t size = m_Layers.size();
    map< Environment, size_t > envLayer = { { Environment::Indoor, size - 2 }, { Environment::Outdoor, 1 } };
    return m_Layers[ envLayer.at( t_Environment ) ]->getGainFlow();
  }

  void CIGU::setInitialGuess( const shared_ptr< vector< double > >& t_Guess ) {
    if( 2 * getNumOfLayers() != t_Guess->size() ) {
      cout << "Number of temperatures in initial guess cannot fit number of layers."
        "Program will use initial guess instead" << endl;
    } else {
      size_t Index = 0;
      for( shared_ptr< CIGUSolidLayer >& aLayer : getSolidLayers() ) {
        for( Side aSide : EnumSide() ) {
          shared_ptr< ISurface > aSurface = aLayer->getSurface( aSide );
          aSurface->initializeStart( ( *t_Guess )[ Index ] );
          ++Index;
        }
      }
    }
  }

  void CIGU::setDeflectionProperties( const double t_Tini, const double t_Pini ) {
    // Simply decorating layers in a list with new behavior
    vector< shared_ptr< CIGUSolidLayer > > aVector = getSolidLayers();
    // deflection properties of the IGU
    double Lmean = Ldmean();
    double Lmax = Ldmax();

    for( shared_ptr< CIGUSolidLayer >& aLayer : getSolidLayers() ) {
      // Deflection could aslo be decorated (created) outside in which case program already have a layer as
      // deflection layer. If that is not done then layer must be decorated with defalut deflection
      // properties
      shared_ptr< CIGUSolidLayerDeflection > aDeflectionLayer = nullptr;
      if( dynamic_pointer_cast< CIGUSolidLayerDeflection >( aLayer ) == NULL ) {
        aDeflectionLayer = make_shared< CIGUSolidLayerDeflection >( *aLayer );        
      } else {
        aDeflectionLayer = dynamic_pointer_cast< CIGUSolidLayerDeflection >( aLayer );
      }
      replaceLayer( aLayer, make_shared< CIGUDeflectionTempAndPressure >( aDeflectionLayer, Lmax, Lmean ) );
    }
    for( shared_ptr< CIGUGapLayer >& aLayer : getGapLayers() ) {
      replaceLayer( aLayer, make_shared< CIGUGapLayerDeflection >( aLayer, t_Tini, t_Pini ) );
    }
  }

  void CIGU::setDeflectionProperties( const vector< double >& t_MeasuredDeflections ) {
    if( t_MeasuredDeflections.size() != getNumOfLayers() - 1 ) {
      throw runtime_error("Number of measured deflection values must be equal to number of gaps.");
    }

    double nominator = 0;
    for( size_t i = 0; i < t_MeasuredDeflections.size(); ++i ) {
      double SumL = 0;
      for( size_t j = i; j < t_MeasuredDeflections.size(); ++j ) {
        SumL += getGapLayers()[ j ]->getThickness() - t_MeasuredDeflections[ j ];
      }
      CIGUSolidLayerDeflection aDefLayer = CIGUSolidLayerDeflection( *getSolidLayers()[ i ] );
      nominator += SumL * aDefLayer.flexuralRigidity();
    }

    double denominator = 0;
    for( size_t i = 0; i < getSolidLayers().size(); ++i ) {
      CIGUSolidLayerDeflection aDefLayer = CIGUSolidLayerDeflection( *getSolidLayers()[ i ] );
      denominator += aDefLayer.flexuralRigidity();
    }

    // First need to calculate new deflections before applying them. Applying them right away will
    // cause that next gap width calculation will already have included one surface deflected
    double LDefNMax = nominator / denominator;
    double deflectionRatio = Ldmean() / Ldmax();

    vector< double > LDefMax; 
    LDefMax.push_back( LDefNMax );
    for( size_t i = getNumOfLayers() - 1; i > 0; --i ) {
      LDefNMax = t_MeasuredDeflections[ i - 1 ] - getGapLayers()[ i - 1 ]->getThickness() + LDefNMax;
      LDefMax.insert( LDefMax.begin(), LDefNMax );
    }

    for( size_t i = 0; i < getNumOfLayers(); ++i ) {
      LDefNMax = LDefMax[ i ];
      double LDefNMean = deflectionRatio * LDefNMax;
      shared_ptr< CIGUSolidLayer > aLayer = getSolidLayers()[ i ];
      shared_ptr< CIGUSolidLayerDeflection > aDefLayer = 
        make_shared< CIGUSolidLayerDeflection >( *aLayer );      
      aDefLayer = make_shared< CIGUDeflectionMeasuread >( aDefLayer, LDefNMean, LDefNMax );
      replaceLayer( aLayer, aDefLayer );
    }

  }

  void CIGU::replaceLayer( const shared_ptr< CBaseIGULayer >& t_Original, 
    const shared_ptr< CBaseIGULayer >& t_Replacement ) {
    size_t index = find( m_Layers.begin(), m_Layers.end(), t_Original ) - m_Layers.begin();
    m_Layers[ index ] = t_Replacement;
    if (index > 0) {
      m_Layers[ index - 1 ]->connectToBackSide( t_Replacement );
    }
    if ( index < m_Layers.size() - 1 ) {
      t_Replacement->connectToBackSide( m_Layers[ index + 1 ] );
    }
  }

  void CIGU::checkForLayerUpgrades( const shared_ptr< CBaseIGULayer > t_Layer ) {
    if( dynamic_pointer_cast< CIGUShadeLayer >( t_Layer ) != NULL ) {
      if( dynamic_pointer_cast< CIGUGapLayer >( t_Layer->getPreviousLayer() ) != NULL ) {
        shared_ptr< CIGUVentilatedGapLayer > newLayer =
          make_shared< CIGUVentilatedGapLayer >( dynamic_pointer_cast< CIGUGapLayer > ( t_Layer->getPreviousLayer() ) );
        replaceLayer( dynamic_pointer_cast< CIGUGapLayer > ( t_Layer->getPreviousLayer() ), newLayer );
      }
    }
    if( dynamic_pointer_cast< CIGUGapLayer >( t_Layer ) != NULL ) {
      if( dynamic_pointer_cast< CIGUShadeLayer >( t_Layer->getPreviousLayer() ) != NULL ) {
        shared_ptr< CIGUVentilatedGapLayer > newLayer =
          make_shared< CIGUVentilatedGapLayer >( dynamic_pointer_cast< CIGUGapLayer > ( t_Layer ) );
        replaceLayer( dynamic_pointer_cast< CIGUGapLayer > ( t_Layer ), newLayer );
      }
    }
  }

  double CIGU::Ldmean() const {
    double coeff = 16 / ( pow( M_PI, 6 ) );
    double totalSum = 0;
    for( size_t m = 1; m <= 5; m += 2 ) {
      for( size_t n = 1; n <= 5; n += 2 ) {
        double nomin = 4;
        double denom = m * m * n * n * M_PI * M_PI * pow( pow( m / m_Width, 2 ) + pow( n / m_Height, 2 ), 2 );
        totalSum += nomin / denom;
      }
    }
    return coeff * totalSum;
  }

  double CIGU::Ldmax() const {
    double coeff = 16 / ( pow( M_PI, 6 ) );
    double totalSum = 0;
    for( size_t m = 1; m <= 5; m += 2 ) {
      for( size_t n = 1; n <= 5; n += 2 ) {
        double nomin = sin( m * M_PI / 2 ) * sin( n * M_PI / 2 );
        double denom = m * n * pow( pow( m / m_Width, 2 ) + pow( n / m_Height, 2 ), 2 );
        totalSum += nomin / denom;
      }
    }
    return coeff * totalSum;
  }

  vector< shared_ptr< CIGUSolidLayer > > CIGU::getSolidLayers() const {
    vector< shared_ptr< CIGUSolidLayer > > aVect;
    for( const shared_ptr< CBaseIGULayer >& aLayer : m_Layers ) {
      if( dynamic_pointer_cast< CIGUSolidLayer >( aLayer ) != NULL ) {
        aVect.push_back( dynamic_pointer_cast< CIGUSolidLayer >( aLayer ) );
      }
    }
    return aVect;
  }

  vector< shared_ptr< CIGUGapLayer > > CIGU::getGapLayers() const {
    vector< shared_ptr< CIGUGapLayer > > aVect;
    for( const shared_ptr< CBaseIGULayer >& aLayer : m_Layers ) {
      if( dynamic_pointer_cast< CIGUGapLayer >( aLayer ) != NULL ) {
        aVect.push_back( dynamic_pointer_cast< CIGUGapLayer >( aLayer ) );
      }
    }
    return aVect;
  }

  vector< shared_ptr< CBaseIGULayer > > CIGU::getLayers() const {
    return m_Layers;
  }

}
