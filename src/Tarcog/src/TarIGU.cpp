#define _USE_MATH_DEFINES
#include <math.h>
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
#include "TarIGUVentilatedGapLayer.hpp"
#include "TarBaseShade.hpp"
#include "TarEnvironment.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace Tarcog {
  CTarIGU::CTarIGU( double t_Width, double t_Height, double t_Tilt, double t_TotalSolar ) : 
    m_Width( t_Width ), m_Height( t_Height ), m_Tilt( t_Tilt ), m_TotalSolar( t_TotalSolar ) {
  }

  CTarIGU::CTarIGU( const CTarIGU& t_IGU ) {
    m_Width = t_IGU.m_Width;
    m_Height = t_IGU.m_Height;
    m_Tilt = t_IGU.m_Tilt;
    m_TotalSolar = t_IGU.m_TotalSolar;
    for( size_t i = 0; i < t_IGU.m_Layers.size(); ++i ) {
      shared_ptr< CBaseIGUTarcogLayer > aLayer = 
        dynamic_pointer_cast< CBaseIGUTarcogLayer >( t_IGU.m_Layers[ i ]->clone() );
      addLayer( aLayer );
    }
  }

  CTarIGU::~CTarIGU() {
    for( shared_ptr< CBaseIGUTarcogLayer > layer : getSolidLayers() ) {
      layer->tearDownConnections();
    }
  }

  void CTarIGU::addLayer( const shared_ptr< CBaseIGUTarcogLayer >& t_Layer ) {

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

  void CTarIGU::setTilt( const double t_Tilt ) {
    for( shared_ptr< CBaseIGUTarcogLayer > layer : m_Layers ) {
      layer->setTilt( t_Tilt );
    }
  }

  void CTarIGU::setWidth( const double t_Width ) {
    for( shared_ptr< CBaseIGUTarcogLayer > layer : m_Layers ) {
      layer->setWidth( t_Width );
    }
  }

  void CTarIGU::setHeight( const double t_Height ) {
    for( shared_ptr< CBaseIGUTarcogLayer > layer : m_Layers ) {
      layer->setHeight( t_Height );
    }
  }

  void CTarIGU::setSolarRadiation( const double t_SolarRadiation ) {
    for( shared_ptr< CTarIGUSolidLayer >& layer : getSolidLayers() ) {
      layer->setSolarRadiation( t_SolarRadiation );
    }
  }

  void CTarIGU::setTotalSolar( const double t_TotSol ) {
    m_TotalSolar = t_TotSol;
  }

  shared_ptr<CBaseTarcogLayer> CTarIGU::getLayer( const Environment t_Environment ) const {
    shared_ptr< CBaseTarcogLayer > aLayer = nullptr;
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

  shared_ptr< vector< double > > CTarIGU::getTemperatures() const {
    shared_ptr< vector< double > > aTemperatures = make_shared< vector< double > >();

    for( const shared_ptr< CTarIGUSolidLayer >& layer : getSolidLayers() ) {
      for( Side aSide : EnumSide() ) {
        shared_ptr< CTarSurface > aSurface = layer->getSurface( aSide );
        assert( aSurface != nullptr );
        aTemperatures->push_back( aSurface->getTemperature() );
      }
    }

    return aTemperatures;
  }

  shared_ptr< vector< double > > CTarIGU::getRadiosities() const {
    shared_ptr< vector< double > > aRadiosities = make_shared< vector< double > >();

    for( const shared_ptr< CTarIGUSolidLayer >& layer : getSolidLayers() ) {
      for( Side aSide : EnumSide() ) {
        shared_ptr< CTarSurface > aSurface = layer->getSurface( aSide );
        assert( aSurface != nullptr );
        aRadiosities->push_back( aSurface->J() );
      }
    }

    return aRadiosities;
  }

  shared_ptr< vector< double > > CTarIGU::getMaxDeflections() const {
    shared_ptr< vector< double > > aMaxDeflections = make_shared< vector< double > >();

    for( const shared_ptr< CTarIGUSolidLayer >& layer : getSolidLayers() ) {
      aMaxDeflections->push_back( layer->getMaxDeflection() );
    }

    return aMaxDeflections;
  }

  shared_ptr< vector< double > > CTarIGU::getMeanDeflections() const {
    shared_ptr< vector< double > > aMeanDeflections = make_shared< vector< double > >();

    for( const shared_ptr< CTarIGUSolidLayer >& layer : getSolidLayers() ) {
      aMeanDeflections->push_back( layer->getMeanDeflection() );
    }

    return aMeanDeflections;
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

  double CTarIGU::getVentilationFlow( const Environment t_Environment ) const {
    size_t size = m_Layers.size();
    map< Environment, size_t > envLayer = { { Environment::Indoor, size - 2 }, { Environment::Outdoor, 1 } };
    return m_Layers[ envLayer.at( t_Environment ) ]->getGainFlow();
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
    // deflection properties of the IGU
    double Lmean = Ldmean();
    double Lmax = Ldmax();

    for( shared_ptr< CTarIGUSolidLayer >& aLayer : getSolidLayers() ) {
      // Deflection could aslo be decorated (created) outside in which case program already have a layer as
      // deflection layer. If that is not done then layer must be decorated with defalut deflection
      // properties
      shared_ptr< CTarIGUSolidLayerDeflection > aDeflectionLayer = nullptr;
      if( dynamic_pointer_cast< CTarIGUSolidLayerDeflection >( aLayer ) == NULL ) {
        aDeflectionLayer = make_shared< CTarIGUSolidLayerDeflection >( *aLayer );        
      } else {
        aDeflectionLayer = dynamic_pointer_cast< CTarIGUSolidLayerDeflection >( aLayer );
      }
      replaceLayer( aLayer, make_shared< CTarIGUDeflectionTempAndPressure >( aDeflectionLayer, Lmax, Lmean ) );
    }
    for( shared_ptr< CTarIGUGapLayer >& aLayer : getGapLayers() ) {
      replaceLayer( aLayer, make_shared< CTarIGUGapLayerDeflection >( aLayer, t_Tini, t_Pini ) );
    }
  }

  void CTarIGU::setDeflectionProperties( const vector< double >& t_MeasuredDeflections ) {
    if( t_MeasuredDeflections.size() != getNumOfLayers() - 1 ) {
      throw runtime_error("Number of measured deflection values must be equal to number of gaps.");
    }

    double nominator = 0;
    for( size_t i = 0; i < t_MeasuredDeflections.size(); ++i ) {
      double SumL = 0;
      for( size_t j = i; j < t_MeasuredDeflections.size(); ++j ) {
        SumL += getGapLayers()[ j ]->getThickness() - t_MeasuredDeflections[ j ];
      }
      CTarIGUSolidLayerDeflection aDefLayer = CTarIGUSolidLayerDeflection( *getSolidLayers()[ i ] );
      nominator += SumL * aDefLayer.flexuralRigidity();
    }

    double denominator = 0;
    for( size_t i = 0; i < getSolidLayers().size(); ++i ) {
      CTarIGUSolidLayerDeflection aDefLayer = CTarIGUSolidLayerDeflection( *getSolidLayers()[ i ] );
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
      shared_ptr< CTarIGUSolidLayer > aLayer = getSolidLayers()[ i ];
      shared_ptr< CTarIGUSolidLayerDeflection > aDefLayer = 
        make_shared< CTarIGUSolidLayerDeflection >( *aLayer );      
      aDefLayer = make_shared< CTarIGUDeflectionMeasuread >( aDefLayer, LDefNMean, LDefNMax );
      replaceLayer( aLayer, aDefLayer );
    }

  }

  void CTarIGU::replaceLayer( const shared_ptr< CBaseIGUTarcogLayer >& t_Original, 
    const shared_ptr< CBaseIGUTarcogLayer >& t_Replacement ) {
    size_t index = find( m_Layers.begin(), m_Layers.end(), t_Original ) - m_Layers.begin();
    m_Layers[ index ] = t_Replacement;
    if (index > 0) {
      m_Layers[ index - 1 ]->connectToBackSide( t_Replacement );
    }
    if ( index < m_Layers.size() - 1 ) {
      t_Replacement->connectToBackSide( m_Layers[ index + 1 ] );
    }
  }

  void CTarIGU::checkForLayerUpgrades( const shared_ptr< CBaseIGUTarcogLayer > t_Layer ) {
    if( dynamic_pointer_cast< CTarIGUShadeLayer >( t_Layer ) != NULL ) {
      if( dynamic_pointer_cast< CTarIGUGapLayer >( t_Layer->getPreviousLayer() ) != NULL ) {
        shared_ptr< CTarIGUVentilatedGapLayer > newLayer =
          make_shared< CTarIGUVentilatedGapLayer >( dynamic_pointer_cast< CTarIGUGapLayer > ( t_Layer->getPreviousLayer() ) );
        replaceLayer( dynamic_pointer_cast< CTarIGUGapLayer > ( t_Layer->getPreviousLayer() ), newLayer );
      }
    }
    if( dynamic_pointer_cast< CTarIGUGapLayer >( t_Layer ) != NULL ) {
      if( dynamic_pointer_cast< CTarIGUShadeLayer >( t_Layer->getPreviousLayer() ) != NULL ) {
        shared_ptr< CTarIGUVentilatedGapLayer > newLayer =
          make_shared< CTarIGUVentilatedGapLayer >( dynamic_pointer_cast< CTarIGUGapLayer > ( t_Layer ) );
        replaceLayer( dynamic_pointer_cast< CTarIGUGapLayer > ( t_Layer ), newLayer );
      }
    }
  }

  double CTarIGU::Ldmean() const {
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

  double CTarIGU::Ldmax() const {
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

  vector< shared_ptr< CTarIGUSolidLayer > > CTarIGU::getSolidLayers() const {
    vector< shared_ptr< CTarIGUSolidLayer > > aVect;
    for( const shared_ptr< CBaseIGUTarcogLayer >& aLayer : m_Layers ) {
      if( dynamic_pointer_cast< CTarIGUSolidLayer >( aLayer ) != NULL ) {
        aVect.push_back( dynamic_pointer_cast< CTarIGUSolidLayer >( aLayer ) );
      }
    }
    return aVect;
  }

  vector< shared_ptr< CTarIGUGapLayer > > CTarIGU::getGapLayers() const {
    vector< shared_ptr< CTarIGUGapLayer > > aVect;
    for( const shared_ptr< CBaseIGUTarcogLayer >& aLayer : m_Layers ) {
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
