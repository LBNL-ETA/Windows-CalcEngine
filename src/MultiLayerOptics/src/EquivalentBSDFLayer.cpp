#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <iterator>
#include <functional>
#include <numeric>
#include <assert.h>
#include <stdexcept>
//#include <thread>

#include "EquivalentBSDFLayer.hpp"
#include "EquivalentBSDFLayerSingleBand.hpp"
#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"

using namespace std;
using namespace FenestrationCommon;
using namespace SingleLayerOptics;

namespace MultiLayerOptics {

  CEquivalentBSDFLayer::CEquivalentBSDFLayer( const shared_ptr< vector< double > >& t_CommonWavelengths,
    const shared_ptr< CBSDFLayer >& t_Layer ) : 
    m_CombinedLayerWavelengths( t_CommonWavelengths ), 
    m_Calculated( false ) {
    if( t_Layer == nullptr ) {
      throw runtime_error("Equivalent BSDF Layer must contain valid layer.");
    }

    // Create layers for each wavelength
    m_LayersWL = make_shared< vector< shared_ptr< CEquivalentBSDFLayerSingleBand > > >();

    // Lambda matrix from spectral results. Same lambda is valid for any wavelength
    m_Lambda = t_Layer->getResults()->lambdaMatrix();

    addLayer( t_Layer );

  }

  void CEquivalentBSDFLayer::addLayer( const shared_ptr< CBSDFLayer >& t_Layer ) {

    // t_Layer->setSourceData( m_SolarRadiation );

    m_Layer.push_back( t_Layer );

    updateWavelengthLayers( t_Layer );

  }

  shared_ptr< const CBSDFDirections > CEquivalentBSDFLayer::getDirections( const BSDFHemisphere t_Side ) const {
    return m_Layer[ 0 ]->getDirections( t_Side );
  }

  shared_ptr< vector< double > > CEquivalentBSDFLayer::getCommonWavelengths() const {
    return m_CombinedLayerWavelengths;
  }

  shared_ptr< CMatrixSeries > CEquivalentBSDFLayer::getTotalA( const Side t_Side ) {
    if( !m_Calculated ) {
      calculate();
    }
    return m_TotA.at( t_Side );
  }

  shared_ptr< CMatrixSeries > CEquivalentBSDFLayer::getTotal( 
    const Side t_Side, const PropertySimple t_Property ) {
    if( !m_Calculated ) {
      calculate();
    }
    return m_Tot.at( make_pair( t_Side, t_Property ) );
  }

  void CEquivalentBSDFLayer::setSolarRadiation( const shared_ptr< CSeries >& t_SolarRadiation ) {
    // Need to recreate wavelenght by wavelength layers
    m_LayersWL->clear();
    for( shared_ptr< CBSDFLayer > aLayer : m_Layer ) {
      aLayer->setSourceData( t_SolarRadiation );
      updateWavelengthLayers( aLayer );
    }
    m_Calculated = false;
  }

  void CEquivalentBSDFLayer::calculate() {
    size_t matrixSize = m_Lambda->getSize();
    size_t numberOfLayers = ( *m_LayersWL )[ 0 ]->getNumberOfLayers();

    for( Side aSide : EnumSide() ) {
      m_TotA[ aSide ] = make_shared< CMatrixSeries >( numberOfLayers, matrixSize );
      for( PropertySimple aProperty : EnumPropertySimple() ) {
        m_Tot[ make_pair( aSide, aProperty ) ] = make_shared< CMatrixSeries >( matrixSize, matrixSize );
      }
    }

    // Calculate total transmitted solar per matrix and perform integration over each wavelength
    size_t WLsize = m_CombinedLayerWavelengths->size();

    // // This is for multithread calculations. Results were correct and it was some decent improvement.
    // // However, this would require more testing on linux machine (it does not work on Travis)
    // size_t numOfThreads = size_t( thread::hardware_concurrency() - 2 );
    // size_t step = WLsize / numOfThreads;
    // vector< shared_ptr< thread > > aThreads = vector< shared_ptr< thread > >( numOfThreads );
    // 
    // size_t startNum = 0;
    // size_t endNum = step;
    // 
    // for( size_t i = 0; i < numOfThreads; ++i ) {
    //   if( i == numOfThreads - 1 ) {
    //     endNum = WLsize;
    //   }
    //   aThreads[ i ] = make_shared< thread >( &CEquivalentBSDFLayer::triggerLayerAbsCalculations, *this,
    //     numberOfLayers, startNum, endNum );
    //   startNum += step;
    //   endNum += step;
    // }
    // 
    // for( size_t i = 0; i < numOfThreads; ++i ) {
    //   aThreads[ i ]->join();
    // }


    calculateWavelengthProperties( m_TotA, m_Tot, numberOfLayers, 0, WLsize );

    m_Calculated = true;

  }

  void CEquivalentBSDFLayer::triggerLayerAbsCalculations( const size_t t_NumOfLayers, 
    const size_t t_Start, const size_t t_End ) {
    for( size_t i = t_Start; i < t_End; ++i ) {
      CEquivalentBSDFLayerSingleBand& curLayer = *( *m_LayersWL )[ i ];
      for( size_t k = 0; k < t_NumOfLayers; ++k ) {
        curLayer.getLayerAbsorptances( k + 1, Side::Front );
      }
    }
  }

void CEquivalentBSDFLayer::calculateWavelengthProperties(
    map< Side, shared_ptr< CMatrixSeries > >& t_TotA, 
    map< pair< Side, PropertySimple >, shared_ptr< CMatrixSeries > >& t_Tot, 
    const size_t t_NumOfLayers, const size_t t_Start, const size_t t_End ) {
    for( size_t i = t_Start; i < t_End; ++i ) {
      
      double curWL = ( *m_CombinedLayerWavelengths )[ i ];
      CEquivalentBSDFLayerSingleBand& curLayer = *( *m_LayersWL )[ i ];

      for( Side aSide : EnumSide() ) {
        for( size_t k = 0; k < t_NumOfLayers; ++k ) {
          t_TotA.at( aSide )->addProperties( k, curWL, *curLayer.getLayerAbsorptances( k + 1, aSide ) );
        }
        for( PropertySimple aProperty : EnumPropertySimple() ) {
          shared_ptr< CSquareMatrix > curPropertyMatrix = curLayer.getProperty( aSide, aProperty );
          t_Tot.at( make_pair( aSide, aProperty ) )->addProperties( curWL, *curPropertyMatrix );
        }
      }
    }
  }

void CEquivalentBSDFLayer::updateWavelengthLayers( 
  const shared_ptr< CBSDFLayer >& t_Layer ) {
  shared_ptr< vector< shared_ptr < CBSDFIntegrator > > > aResults = nullptr;

  aResults = t_Layer->getWavelengthResults();
  size_t size = m_CombinedLayerWavelengths->size();
  for( size_t i = 0; i < size; ++i ) {
    double curWL = ( *m_CombinedLayerWavelengths )[ i ];
    int index = t_Layer->getBandIndex( curWL );
    assert( index > -1 );

    shared_ptr< CBSDFIntegrator > currentLayer = ( *aResults )[ size_t( index ) ];

    if( m_LayersWL->size() <= i ) {
      shared_ptr< CEquivalentBSDFLayerSingleBand > aEquivalentLayer =
        make_shared< CEquivalentBSDFLayerSingleBand >( currentLayer );

      m_LayersWL->push_back( aEquivalentLayer );
    } else {
      shared_ptr< CEquivalentBSDFLayerSingleBand > currentEqLayer = ( *m_LayersWL )[ i ];
      currentEqLayer->addLayer( currentLayer );
    }

  }
}

}
