#include <algorithm>
#include <iterator>
#include <assert.h>
#include <stdexcept>


#include "EquivalentBSDFLayer.hpp"
#include "EquivalentBSDFLayerSingleBand.hpp"
#include "BSDFLayer.hpp"
#include "SpecularBSDFLayer.hpp"
#include "Series.hpp"
#include "IntegratorStrategy.hpp"
#include "BSDFResults.hpp"
#include "SquareMatrix.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;
using namespace LayerOptics;

namespace MultiPane {

  CEquivalentBSDFLayer::CEquivalentBSDFLayer( shared_ptr< vector< double > > t_CommonWavelengths,
    shared_ptr< CSeries > t_SolarRadiation, shared_ptr< CBSDFLayer > t_Layer ) : 
    m_SolarRadiation( t_SolarRadiation ), m_CombinedLayerWavelengths( t_CommonWavelengths ), m_Calculated( false ) {
    if( t_Layer == nullptr ) {
      throw runtime_error("Equivalent BSDF Layer must contain valid layer.");
    }

    m_TauF = nullptr;
    m_TauB = nullptr;
    m_RhoF = nullptr;
    m_RhoB = nullptr;    
    m_AbsF = nullptr;
    m_AbsB = nullptr;

    m_LayersWL = make_shared< vector< shared_ptr< CEquivalentBSDFLayerSingleBand > > >();

    // m_TauF_WL = make_shared< vector< shared_ptr< CSquareMatrix > > >();
    // m_TauB_WL = make_shared< vector< shared_ptr< CSquareMatrix > > >();
    // m_RhoF_WL = make_shared< vector< shared_ptr< CSquareMatrix > > >();
    // m_RhoB_WL = make_shared< vector< shared_ptr< CSquareMatrix > > >();

    // Lambda matrix from spectral results. Same lambda is valid for any wavelength
    m_Lambda = t_Layer->getResults()->lambdaMatrix();

    shared_ptr< vector< shared_ptr < CBSDFResults > > > aResults = nullptr;

    aResults = t_Layer->getWavelengthResults();
    size_t size = m_CombinedLayerWavelengths->size();
    for( size_t i = 0; i < size; ++i ) {
      double curWL = ( *m_CombinedLayerWavelengths )[ i ];
      int index = t_Layer->getBandIndex( curWL );
      assert( index > -1 );

      shared_ptr< CBSDFResults > currentLayer = ( *aResults )[ size_t( index ) ];
      shared_ptr< CEquivalentBSDFLayerSingleBand > aEquivalentLayer = make_shared< CEquivalentBSDFLayerSingleBand >( currentLayer );

      m_LayersWL->push_back( aEquivalentLayer );
      
      // m_TauF_WL->push_back( ( *aResults )[ size_t( index ) ]->Tau( Side::Front ) );
      // m_TauB_WL->push_back( ( *aResults )[ size_t( index ) ]->Tau( Side::Back ) );
      // m_RhoF_WL->push_back( ( *aResults )[ size_t( index ) ]->Rho( Side::Front ) );
      // m_RhoB_WL->push_back( ( *aResults )[ size_t( index ) ]->Rho( Side::Back ) );
    }

  }

  void CEquivalentBSDFLayer::addLayer( shared_ptr< CBSDFLayer > t_Layer ) {

    shared_ptr< vector< shared_ptr < CBSDFResults > > > aResults = nullptr;

    // m_AbsF.addLayer( t_Layer );
    // m_AbsB.addLayer( t_Layer );

    aResults = t_Layer->getWavelengthResults();
    size_t size = m_CombinedLayerWavelengths->size();
    for( size_t i = 0; i < size; ++i ) {
      double curWL = ( *m_CombinedLayerWavelengths )[ i ];
      int index = t_Layer->getBandIndex( curWL );
      assert( index > -1 );
      shared_ptr< CBSDFResults > currentLayer = ( *aResults )[ size_t( index ) ];
      shared_ptr< CEquivalentBSDFLayerSingleBand > currentEqLayer = ( *m_LayersWL )[ i ];
      currentEqLayer->addLayer( currentLayer );
      // shared_ptr< CSquareMatrix > Tf2 = ( *aResults )[ size_t( index ) ]->Tau( Side::Front );
      // shared_ptr< CSquareMatrix > Tb2 = ( *aResults )[ size_t( index ) ]->Tau( Side::Back );
      // shared_ptr< CSquareMatrix > Rf2 = ( *aResults )[ size_t( index ) ]->Rho( Side::Front );
      // shared_ptr< CSquareMatrix > Rb2 = ( *aResults )[ size_t( index ) ]->Rho( Side::Back );
    
      // shared_ptr< CSquareMatrix > aTauF = totTransmittance( ( *m_TauF_WL )[ i ], Tf2, ( *m_RhoB_WL )[ i ], Rf2 );
      // shared_ptr< CSquareMatrix > aTauB = totTransmittance( Tf2, ( *m_TauB_WL )[ i ], Rf2, ( *m_RhoB_WL )[ i ] );
      // shared_ptr< CSquareMatrix > aRhoF = totReflectance( ( *m_TauF_WL )[ i ], ( *m_TauB_WL )[ i ], 
      //   ( *m_RhoF_WL )[ i ], ( *m_RhoB_WL )[ i ], Rf2 );
      // shared_ptr< CSquareMatrix > aRhoB = totReflectance( Tb2, Tf2, Rb2, Rf2, ( *m_RhoB_WL )[ i ] );
      // 
      // ( *m_TauF_WL )[ i ] = aTauF;
      // ( *m_TauB_WL )[ i ] = aTauB;
      // ( *m_RhoF_WL )[ i ] = aRhoF;
      // ( *m_RhoB_WL )[ i ] = aRhoB;
    }

  }

  shared_ptr< CSquareMatrix > CEquivalentBSDFLayer::Tau( const double minLambda, const double maxLambda, Side t_Side ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }

    shared_ptr< CSquareMatrix > aMatrix = nullptr;

    switch( t_Side ) {
      case Side::Front:
        aMatrix = m_TauF;
        break;
      case Side::Back:
        aMatrix = m_TauB;
        break;
      default:
        assert("Incorrect side selection.");
        break;
      }

    return aMatrix;
  }

  shared_ptr< CSquareMatrix > CEquivalentBSDFLayer::Rho( const double minLambda, 
    const double maxLambda, Side t_Side ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }

    shared_ptr< CSquareMatrix > aMatrix = nullptr;

    switch( t_Side ) {
      case Side::Front:
        aMatrix = m_RhoF;
        break;
      case Side::Back:
        aMatrix = m_RhoB;
        break;
      default:
        assert("Incorrect side selection.");
        break;
      }

    return aMatrix;
  }

  shared_ptr< vector< double > > CEquivalentBSDFLayer::Abs( const double minLambda, const double maxLambda, 
    const Side t_Side, const size_t Index ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }
    shared_ptr< vector< double > > aAbs = nullptr;
    switch( t_Side ) {
      case Side::Front:
        aAbs = ( *m_AbsF )[ Index - 1 ];
        break;
      case Side::Back:
        aAbs = ( *m_AbsB )[ Index - 1 ];
        break;
      default:
        assert("Incorrect side selection.");
        break;
      }
    return aAbs;
  }

  void CEquivalentBSDFLayer::calculate( const double minLambda, const double maxLambda ) {
    size_t matrixSize = m_Lambda->getSize();
    m_TauF = make_shared< CSquareMatrix >( matrixSize );
    m_TauB = make_shared< CSquareMatrix >( matrixSize );
    m_RhoF = make_shared< CSquareMatrix >( matrixSize );
    m_RhoB = make_shared< CSquareMatrix >( matrixSize );

    size_t numberOfLayers = ( *m_LayersWL )[ 0 ]->getNumberOfLayers();

    m_AbsF = make_shared< vector< shared_ptr< vector< double > > > >( numberOfLayers );
    m_AbsB = make_shared< vector< shared_ptr< vector< double > > > >( numberOfLayers );

    for( size_t i = 0; i < numberOfLayers; ++i ) {
      ( *m_AbsF )[ i ] = make_shared< vector< double > >( matrixSize );
      ( *m_AbsB )[ i ] = make_shared< vector< double > >( matrixSize );
    }

    shared_ptr< CSeries > iTotalSolar = m_SolarRadiation->integrate( IntegrationType::Trapezoidal );
    double incomingSolar = iTotalSolar->sum( minLambda, maxLambda );

    shared_ptr< CSeries > interpolatedSolar = m_SolarRadiation->interpolate( m_CombinedLayerWavelengths );

    size_t size = m_CombinedLayerWavelengths->size();

    // Total matrices for every property
    vector< vector< shared_ptr< CSeries > > > aTotalTFront;
    vector< vector< shared_ptr< CSeries > > > aTotalTBack;
    vector< vector< shared_ptr< CSeries > > > aTotalRFront;
    vector< vector< shared_ptr< CSeries > > > aTotalRBack;

    vector< vector< shared_ptr< CSeries > > > aTotalAf = vector< vector< shared_ptr< CSeries > > >( numberOfLayers );
    vector< vector< shared_ptr< CSeries > > > aTotalAb = vector< vector< shared_ptr< CSeries > > >( numberOfLayers );
    for( size_t i = 0; i < numberOfLayers; ++i ) {
      aTotalAf[ i ].resize( matrixSize );
      aTotalAb[ i ].resize( matrixSize );
    }

    aTotalTFront.resize( matrixSize );
    aTotalTBack.resize( matrixSize );
    aTotalRFront.resize( matrixSize );
    aTotalRBack.resize( matrixSize );

    for( size_t i = 0; i < matrixSize; ++i ) {
      aTotalTFront[ i ].resize( matrixSize );
      aTotalTBack[ i ].resize( matrixSize );
      aTotalRFront[ i ].resize( matrixSize );
      aTotalRBack[ i ].resize( matrixSize );
    }

    // Calculate total transmitted solar per matrix and perform integration
    for( size_t i = 0; i < size; ++i ) {
      // First need to select correct side
      double curWL = ( *m_CombinedLayerWavelengths )[ i ];
      shared_ptr< CEquivalentBSDFLayerSingleBand > curLayer = ( *m_LayersWL )[ i ];

      for( size_t j = 0; j < matrixSize; ++j ) {
        for( size_t k = 0; k < numberOfLayers; ++k ) {
          if( i == 0 ) {
            aTotalAf[ k ][ j ] = make_shared< CSeries >();
            aTotalAb[ k ][ j ] = make_shared< CSeries >();
          }
          aTotalAf[ k ][ j ]->addProperty( curWL, ( *curLayer->getLayerAbsorptances( k + 1, Side::Front ) )[ j ] );
          aTotalAb[ k ][ j ]->addProperty( curWL, ( *curLayer->getLayerAbsorptances( k + 1, Side::Back ) )[ j ] );
        }
        
        for( size_t k = 0; k < matrixSize; ++k ) {
          if( i == 0 ) {
            aTotalTFront[ j ][ k ] = make_shared< CSeries >();
            aTotalTBack[ j ][ k ] = make_shared< CSeries >();
            aTotalRFront[ j ][ k ] = make_shared< CSeries >();
            aTotalRBack[ j ][ k ] = make_shared< CSeries >();
          }
          
          aTotalTFront[ j ][ k ]->addProperty( curWL, ( *curLayer->Tau( Side::Front ) )[ j ][ k ] );
          aTotalTBack[ j ][ k ]->addProperty( curWL, ( *curLayer->Tau( Side::Back ) )[ j ][ k ] );
          aTotalRFront[ j ][ k ]->addProperty( curWL, ( *curLayer->Rho( Side::Front ) )[ j ][ k ] );
          aTotalRBack[ j ][ k ]->addProperty( curWL, ( *curLayer->Rho( Side::Back ) )[ j ][ k ] );
        }
      }
    }

    for( size_t j = 0; j < matrixSize; ++j ) {
      for( size_t k = 0; k < numberOfLayers; ++k ) {
          aTotalAf[ k ][ j ] = aTotalAf[ k ][ j ]->mMult( interpolatedSolar );
          aTotalAf[ k ][ j ] = aTotalAf[ k ][ j ]->integrate( IntegrationType::Trapezoidal );
          ( *( *m_AbsF )[ k ] )[ j ] = aTotalAf[ k ][ j ]->sum( minLambda, maxLambda );
          ( *( *m_AbsF )[ k ] )[ j ] = ( *( *m_AbsF )[ k ] )[ j ] / incomingSolar;

          aTotalAb[ k ][ j ] = aTotalAb[ k ][ j ]->mMult( interpolatedSolar );
          aTotalAb[ k ][ j ] = aTotalAb[ k ][ j ]->integrate( IntegrationType::Trapezoidal );
          ( *( *m_AbsB )[ k ] )[ j ] = aTotalAb[ k ][ j ]->sum( minLambda, maxLambda );
          ( *( *m_AbsB )[ k ] )[ j ] = ( *( *m_AbsB )[ k ] )[ j ] / incomingSolar;
        }
      for( size_t k = 0; k < matrixSize; ++k ) {
        // Front transmittance
        aTotalTFront[ j ][ k ] = aTotalTFront[ j ][ k ]->mMult( interpolatedSolar );
        aTotalTFront[ j ][ k ] = aTotalTFront[ j ][ k ]->integrate( IntegrationType::Trapezoidal );
        ( *m_TauF )[ j ][ k ] = aTotalTFront[ j ][ k ]->sum( minLambda, maxLambda );
        ( *m_TauF )[ j ][ k ] = ( *m_TauF )[ j ][ k ] / incomingSolar;

        // Back transmittance
        aTotalTBack[ j ][ k ] = aTotalTBack[ j ][ k ]->mMult( interpolatedSolar );
        aTotalTBack[ j ][ k ] = aTotalTBack[ j ][ k ]->integrate( IntegrationType::Trapezoidal );
        ( *m_TauB )[ j ][ k ] = aTotalTBack[ j ][ k ]->sum( minLambda, maxLambda );
        ( *m_TauB )[ j ][ k ] = ( *m_TauB )[ j ][ k ] / incomingSolar;

        // Front reflectance
        aTotalRFront[ j ][ k ] = aTotalRFront[ j ][ k ]->mMult( interpolatedSolar );
        aTotalRFront[ j ][ k ] = aTotalRFront[ j ][ k ]->integrate( IntegrationType::Trapezoidal );
        ( *m_RhoF )[ j ][ k ] = aTotalRFront[ j ][ k ]->sum( minLambda, maxLambda );
        ( *m_RhoF )[ j ][ k ] = ( *m_RhoF )[ j ][ k ] / incomingSolar;

        // Back reflectance
        aTotalRBack[ j ][ k ] = aTotalRBack[ j ][ k ]->mMult( interpolatedSolar );
        aTotalRBack[ j ][ k ] = aTotalRBack[ j ][ k ]->integrate( IntegrationType::Trapezoidal );
        ( *m_RhoB )[ j ][ k ] = aTotalRBack[ j ][ k ]->sum( minLambda, maxLambda );
        ( *m_RhoB )[ j ][ k ] = ( *m_RhoB )[ j ][ k ] / incomingSolar;
      }
    }

    m_Calculated = true;

  }

}