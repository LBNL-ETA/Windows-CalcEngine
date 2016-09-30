#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <iterator>
#include <functional>
#include <numeric>
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
#include "MatrixSeries.hpp"
#include "BSDFDirections.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;
using namespace LayerOptics;

namespace MultiPane {

  CEquivalentBSDFLayer::CEquivalentBSDFLayer( const shared_ptr< vector< double > >& t_CommonWavelengths,
    const shared_ptr< CSeries >& t_SolarRadiation, const shared_ptr< CBSDFLayer >& t_Layer ) : 
    m_SolarRadiation( t_SolarRadiation ), m_CombinedLayerWavelengths( t_CommonWavelengths ), m_Calculated( false ) {
    if( t_Layer == nullptr ) {
      throw runtime_error("Equivalent BSDF Layer must contain valid layer.");
    }

    m_LayersWL = make_shared< vector< shared_ptr< CEquivalentBSDFLayerSingleBand > > >();

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
      shared_ptr< CEquivalentBSDFLayerSingleBand > aEquivalentLayer = 
        make_shared< CEquivalentBSDFLayerSingleBand >( currentLayer );

      m_LayersWL->push_back( aEquivalentLayer );

    }

    m_Results = make_shared< CBSDFResults >( t_Layer->m_BSDFHemisphere->getDirections( BSDFHemisphere::Incoming ) );

    for( Side aSide : EnumSide() ) {
      m_AbsHem[ aSide ] = make_shared< vector< double > >();
    }

  }

  void CEquivalentBSDFLayer::addLayer( const shared_ptr< CBSDFLayer >& t_Layer ) {

    shared_ptr< vector< shared_ptr < CBSDFResults > > > aResults = nullptr;

    aResults = t_Layer->getWavelengthResults();
    size_t size = m_CombinedLayerWavelengths->size();
    for( size_t i = 0; i < size; ++i ) {
      double curWL = ( *m_CombinedLayerWavelengths )[ i ];
      int index = t_Layer->getBandIndex( curWL );
      assert( index > -1 );
      shared_ptr< CBSDFResults > currentLayer = ( *aResults )[ size_t( index ) ];
      shared_ptr< CEquivalentBSDFLayerSingleBand > currentEqLayer = ( *m_LayersWL )[ i ];
      currentEqLayer->addLayer( currentLayer );
    }

  }

  shared_ptr< CSquareMatrix > CEquivalentBSDFLayer::Tau( const double minLambda, 
    const double maxLambda, Side t_Side ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }

    return m_Results->Tau( t_Side );
  }

  shared_ptr< CSquareMatrix > CEquivalentBSDFLayer::Rho( const double minLambda, 
    const double maxLambda, Side t_Side ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }

    return m_Results->Rho( t_Side );
  }

  double CEquivalentBSDFLayer::TauDirDir( const double minLambda, const double maxLambda, Side t_Side, 
    const double t_Theta, const double t_Phi ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }

    return m_Results->TauDirDir( t_Side, t_Theta, t_Phi );
  }

  double CEquivalentBSDFLayer::RhoDirDir( const double minLambda, const double maxLambda, 
    Side t_Side, const double t_Theta, const double t_Phi ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }

    return m_Results->RhoDirDir( t_Side, t_Theta, t_Phi );
  }

  shared_ptr< vector< double > > CEquivalentBSDFLayer::Abs( const double minLambda, const double maxLambda, 
    const Side t_Side, const size_t Index ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }
    return ( *m_Abs.at( t_Side ) )[ Index - 1 ];
  }

  shared_ptr< vector< double > > CEquivalentBSDFLayer::TauDirHem( const double minLambda, const double maxLambda, 
    const Side t_Side ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }
    return m_Results->TauDirHem( t_Side );
  }

  shared_ptr< vector< double > > CEquivalentBSDFLayer::RhoDirHem( const double minLambda, const double maxLambda, 
    const Side t_Side ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }
    return m_Results->RhoDirHem( t_Side );
  }

  double CEquivalentBSDFLayer::TauDirHem( const double minLambda, const double maxLambda, 
    const Side t_Side, const double t_Theta, const double t_Phi ) {
    auto aIndex = m_Results->getDirections()->getNearestBeamIndex( t_Theta, t_Phi );
    return ( *TauDirHem( minLambda, maxLambda, t_Side ) )[ aIndex ];
  }

  double CEquivalentBSDFLayer::RhoDirHem( const double minLambda, const double maxLambda, 
    const Side t_Side, const double t_Theta, const double t_Phi ) {
    auto aIndex = m_Results->getDirections()->getNearestBeamIndex( t_Theta, t_Phi );
    return ( *RhoDirHem( minLambda, maxLambda, t_Side ) )[ aIndex ];
  }

  double CEquivalentBSDFLayer::Abs( const double minLambda, const double maxLambda, 
    const Side t_Side, const size_t Index, const double t_Theta, const double t_Phi ) {
    auto aIndex = m_Results->getDirections()->getNearestBeamIndex( t_Theta, t_Phi );
    return ( *Abs( minLambda, maxLambda, t_Side, Index ) )[ aIndex ];
  }

  double CEquivalentBSDFLayer::TauDiffDiff( const double minLambda, const double maxLambda, 
    const Side t_Side ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }
    return m_Results->TauDiffDiff( t_Side );
  }

  double CEquivalentBSDFLayer::RhoDiffDiff( const double minLambda, const double maxLambda, 
    const Side t_Side ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }
    return m_Results->RhoDiffDiff( t_Side );
  }

  double CEquivalentBSDFLayer::AbsDiff( const double minLambda, const double maxLambda, 
    const Side t_Side, const size_t t_LayerIndex ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }
    return ( *m_AbsHem[ t_Side ] )[ t_LayerIndex - 1 ];
  }

  void CEquivalentBSDFLayer::calculate( const double minLambda, const double maxLambda ) {
    size_t matrixSize = m_Lambda->getSize();
    size_t numberOfLayers = ( *m_LayersWL )[ 0 ]->getNumberOfLayers();

    // Produce local results matrices for each side and property
    map < pair< Side, PropertySimple >, shared_ptr< CSquareMatrix > > aResults;

    // Getting total solar energy and solar series interpolated over common wavelengths
    shared_ptr< CSeries > iTotalSolar = m_SolarRadiation->integrate( IntegrationType::Trapezoidal );
    double incomingSolar = iTotalSolar->sum( minLambda, maxLambda );

    shared_ptr< CSeries > interpolatedSolar = m_SolarRadiation->interpolate( *m_CombinedLayerWavelengths );

    // Create matrix of series for multiwavelength calculations
    map< pair< Side, PropertySimple >, shared_ptr< CMatrixSeries > > aTot;
    map< Side, shared_ptr< CMatrixSeries > > aTotalA;

    for(Side t_Side : EnumSide()) {
      aTotalA[ t_Side ] = make_shared< CMatrixSeries >( numberOfLayers, matrixSize );
      for( PropertySimple t_Property : EnumPropertySimple() ) {
        aTot[ make_pair( t_Side, t_Property ) ] = make_shared< CMatrixSeries >( matrixSize, matrixSize );
      }
    }

    // Calculate total transmitted solar per matrix and perform integration over each wavelength
    size_t WLsize = m_CombinedLayerWavelengths->size();
    
    // This is for multithread calculations. Results were correct and it was some decent improvement.
    // However, this would require more testing on linux machine (it does not work on Travis)
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
    
    
    calculateWavelengthProperties( aTotalA, aTot, numberOfLayers, matrixSize, 0, WLsize );

    for( Side t_Side : EnumSide() ) {
      aTotalA.at( t_Side )->mMult( *interpolatedSolar );
      aTotalA.at( t_Side )->integrate( IntegrationType::Trapezoidal );
      m_Abs[ t_Side ] = aTotalA.at( t_Side )->getSums( minLambda, maxLambda, 1 / incomingSolar );
      for( PropertySimple t_Proprerty : EnumPropertySimple() ) {
        aTot.at( make_pair( t_Side, t_Proprerty ) )->mMult( *interpolatedSolar );
        aTot.at( make_pair( t_Side, t_Proprerty ) )->integrate( IntegrationType::Trapezoidal );
        aResults[ make_pair( t_Side, t_Proprerty ) ] =
          aTot.at( make_pair( t_Side, t_Proprerty ) )->getSquaredMatrixSums( minLambda, maxLambda, 1 / incomingSolar );
      }

      // Update result matrices
      m_Results->setResultMatrices( aResults.at( make_pair( t_Side, PropertySimple::T ) ),
        aResults.at( make_pair( t_Side, PropertySimple::R ) ), t_Side );
    }

    // calculate hemispherical absorptances
    for( Side aSide : EnumSide() ) {
      calcHemisphericalAbs( aSide );
    }

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
    map< Side, shared_ptr< CMatrixSeries > > t_TotA, 
    map< pair< Side, PropertySimple >, shared_ptr< CMatrixSeries > > t_Tot, 
    const size_t t_NumOfLayers, const size_t t_MatrixSize,
    const size_t t_Start, const size_t t_End ) {
    for( size_t i = t_Start; i < t_End; ++i ) {
      
      double curWL = ( *m_CombinedLayerWavelengths )[ i ];
      CEquivalentBSDFLayerSingleBand& curLayer = *( *m_LayersWL )[ i ];

      for( Side t_Side : EnumSide() ) {
        for( size_t k = 0; k < t_NumOfLayers; ++k ) {
          t_TotA.at( t_Side )->addProperties( k, curWL, *curLayer.getLayerAbsorptances( k + 1, t_Side ) );
        }
        for( PropertySimple t_Property : EnumPropertySimple() ) {
          shared_ptr< CSquareMatrix > curPropertyMatrix = curLayer.getProperty( t_Side, t_Property );
          t_Tot.at( make_pair( t_Side, t_Property ) )->addProperties( curWL, *curPropertyMatrix );
        }
      }
    }
  }

  void CEquivalentBSDFLayer::calcHemisphericalAbs( const Side t_Side ) {
    size_t numOfLayers = m_Abs[ t_Side ]->size();
    vector< double > aLambdas = *m_Results->getDirections()->lambdaVector();
    for( size_t layNum = 0; layNum < numOfLayers; ++layNum ) {
      vector< double > aAbs = *( *m_Abs[ t_Side ] )[ layNum ];
      assert( aAbs.size() == aLambdas.size() );
      vector< double > mult( aLambdas.size() );
      std::transform( aLambdas.begin(), aLambdas.end(), aAbs.begin(), mult.begin(), std::multiplies< double >() );
      double sum = accumulate( mult.begin(), mult.end(), 0.0 ) / M_PI;
      m_AbsHem[ t_Side ]->push_back( sum );
    }
  }

}