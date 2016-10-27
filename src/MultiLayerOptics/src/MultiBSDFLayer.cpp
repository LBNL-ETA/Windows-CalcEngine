#define _USE_MATH_DEFINES
#include <math.h>
#include <numeric>
#include <algorithm>
#include <assert.h>

#include "MultiBSDFLayer.hpp"
#include "BSDFIntegrator.hpp"
#include "EquivalentBSDFLayer.hpp"
#include "Series.hpp"
#include "IntegratorStrategy.hpp"
#include "FenestrationCommon.hpp"
#include "MatrixSeries.hpp"
#include "BSDFDirections.hpp"

using namespace std;
using namespace FenestrationCommon;
using namespace SingleLayerOptics;

namespace MultiLayerOptics {

  CMultiBSDFLayer::CMultiBSDFLayer( const shared_ptr< CEquivalentBSDFLayer >& t_Layer, 
    const shared_ptr< CSeries >& t_SolarRadiation ) : m_Layer( t_Layer ), m_SolarRadiation( t_SolarRadiation ),
    m_Results( make_shared< CBSDFIntegrator >( t_Layer->getDirections( BSDFHemisphere::Incoming ) ) ), 
    m_Calculated( false ) {

    for( Side aSide : EnumSide() ) {
      m_AbsHem[ aSide ] = make_shared< vector< double > >();
    }

    t_Layer->setSolarRadiation( m_SolarRadiation );
  }

  shared_ptr< CSquareMatrix > CMultiBSDFLayer::getMatrix( const double minLambda, const double maxLambda,
    const Side t_Side, const PropertySimple t_Property ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }

    return m_Results->getMatrix( t_Side, t_Property );
  }

  double CMultiBSDFLayer::DirDir( const double minLambda, const double maxLambda,
    const Side t_Side, const PropertySimple t_Property, const double t_Theta, const double t_Phi ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }

    return m_Results->DirDir( t_Side, t_Property, t_Theta, t_Phi );
  }

  void CMultiBSDFLayer::calculate( const double minLambda, const double maxLambda ) {
    // Getting total solar energy and solar series interpolated over common wavelengths
    shared_ptr< CSeries > iTotalSolar = m_SolarRadiation->integrate( IntegrationType::Trapezoidal );
    double incomingSolar = iTotalSolar->sum( minLambda, maxLambda );

    shared_ptr< CSeries > interpolatedSolar = m_SolarRadiation->interpolate( *m_Layer->getCommonWavelengths() );

    // Produce local results matrices for each side and property
    map < pair< Side, PropertySimple >, shared_ptr< CSquareMatrix > > aResults;

    for( Side aSide : EnumSide() ) {
      CMatrixSeries aTotalA = *m_Layer->getTotalA( aSide );
      aTotalA.mMult( *interpolatedSolar );
      aTotalA.integrate( IntegrationType::Trapezoidal );
      m_Abs[ aSide ] = aTotalA.getSums( minLambda, maxLambda, 1 / incomingSolar );
      for( PropertySimple aProprerty : EnumPropertySimple() ) {
        CMatrixSeries& aTot = *m_Layer->getTotal( aSide, aProprerty );
        aTot.mMult( *interpolatedSolar );
        aTot.integrate( IntegrationType::Trapezoidal );
        aResults[ make_pair( aSide, aProprerty ) ] = aTot.getSquaredMatrixSums( minLambda, maxLambda, 1 / incomingSolar );
      }

      // Update result matrices
      m_Results->setResultMatrices( aResults.at( make_pair( aSide, PropertySimple::T ) ),
        aResults.at( make_pair( aSide, PropertySimple::R ) ), aSide );
    }

    // calculate hemispherical absorptances
    for( Side aSide : EnumSide() ) {
      calcHemisphericalAbs( aSide );
    }

    m_Calculated = true;
  }

  void CMultiBSDFLayer::calcHemisphericalAbs( const Side t_Side ) {
    size_t numOfLayers = m_Abs[ t_Side ]->size();
    vector< double > aLambdas = *m_Results->getDirections()->lambdaVector();
    for( size_t layNum = 0; layNum < numOfLayers; ++layNum ) {
      vector< double > aAbs = *( *m_Abs[ t_Side ] )[ layNum ];
      assert( aAbs.size() == aLambdas.size() );
      vector< double > mult( aLambdas.size() );
      transform( aLambdas.begin(), aLambdas.end(), aAbs.begin(), mult.begin(), multiplies< double >() );
      double sum = accumulate( mult.begin(), mult.end(), 0.0 ) / M_PI;
      m_AbsHem[ t_Side ]->push_back( sum );
    }
  }

  shared_ptr< vector< double > > CMultiBSDFLayer::Abs( const double minLambda, const double maxLambda,
    const Side t_Side, const size_t Index ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }
    return ( *m_Abs.at( t_Side ) )[ Index - 1 ];
  }

  shared_ptr< vector< double > > CMultiBSDFLayer::DirHem( const double minLambda, const double maxLambda,
    const Side t_Side, const PropertySimple t_Property ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }
    return m_Results->DirHem( t_Side, t_Property );
  }

  double CMultiBSDFLayer::DirHem( const double minLambda, const double maxLambda,
    const Side t_Side, const PropertySimple t_Property,
    const double t_Theta, const double t_Phi ) {
    auto aIndex = m_Results->getDirections()->getNearestBeamIndex( t_Theta, t_Phi );
    return ( *DirHem( minLambda, maxLambda, t_Side, t_Property ) )[ aIndex ];
  }

  double CMultiBSDFLayer::Abs( const double minLambda, const double maxLambda,
    const Side t_Side, const size_t Index, const double t_Theta, const double t_Phi ) {
    auto aIndex = m_Results->getDirections()->getNearestBeamIndex( t_Theta, t_Phi );
    return ( *Abs( minLambda, maxLambda, t_Side, Index ) )[ aIndex ];
  }

  double CMultiBSDFLayer::DiffDiff( const double minLambda, const double maxLambda,
    const Side t_Side, const PropertySimple t_Property ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }
    return m_Results->DiffDiff( t_Side, t_Property );
  }

  double CMultiBSDFLayer::AbsDiff( const double minLambda, const double maxLambda,
    const Side t_Side, const size_t t_LayerIndex ) {
    if( !m_Calculated ) {
      calculate( minLambda, maxLambda );
    }
    return ( *m_AbsHem[ t_Side ] )[ t_LayerIndex - 1 ];
  }

}