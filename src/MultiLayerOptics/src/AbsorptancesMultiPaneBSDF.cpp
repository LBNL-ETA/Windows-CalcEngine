#include <assert.h>

#include "AbsorptancesMultiPaneBSDF.hpp"
#include "BSDFLayer.hpp"
#include "SpecularBSDFLayer.hpp"
#include "BSDFResults.hpp"
#include "Series.hpp"
#include "SquareMatrix.hpp"
#include "FenestrationCommon.hpp"
#include "IntegratorStrategy.hpp"

using namespace std;
using namespace SingleLayerOptics;
using namespace FenestrationCommon;

namespace MultiLayerOptics {

  CAbsorptancesMultiPaneBSDF::CAbsorptancesMultiPaneBSDF( Side t_Side, 
    const shared_ptr< vector< double > >& t_CommonWavelengths,
    const shared_ptr< CSeries >& t_SolarRadiation, 
    const shared_ptr< CBSDFLayer >& t_Layer ) : 
    m_CommonWavelengths( t_CommonWavelengths ), m_StateCalculated( false ), m_Side( t_Side ),
    m_NumOfLayers( 0 ){

    m_SolarRadiation = t_SolarRadiation->interpolate( *m_CommonWavelengths );
    
    // Lambda matrix from spectral results. Same lambda is valid for any wavelength
    // shared_ptr< const vector< double > > aLambdas = t_Layer->getResults()->lambdas();
    m_LambdaVector = t_Layer->getResults()->lambdaVector();
    m_Lambda = t_Layer->getResults()->lambdaMatrix();

    addLayer( *t_Layer );
  }

  void CAbsorptancesMultiPaneBSDF::addLayer( CBSDFLayer& t_Layer ) {
    m_StateCalculated = false;
    m_NumOfLayers++;
    shared_ptr< vector< shared_ptr < CBSDFResults > > > aResults = nullptr;

    shared_ptr< vector< shared_ptr< CSquareMatrix > > > aTausF = 
      make_shared< vector< shared_ptr< CSquareMatrix > > >();
    shared_ptr< vector< shared_ptr< CSquareMatrix > > > aTausB = 
      make_shared< vector< shared_ptr< CSquareMatrix > > >();
    shared_ptr< vector< shared_ptr< CSquareMatrix > > > aRhosF = 
      make_shared< vector< shared_ptr< CSquareMatrix > > >();
    shared_ptr< vector< shared_ptr< CSquareMatrix > > > aRhosB = 
      make_shared< vector< shared_ptr< CSquareMatrix > > >();
    
    aResults = t_Layer.getWavelengthResults();
    size_t size = m_CommonWavelengths->size();
    // loop through each wavelenght
    for( size_t i = 0; i < size; ++i ) {
      double curWL = ( *m_CommonWavelengths )[ i ];
      int index = t_Layer.getBandIndex( curWL );
      assert( index > -1 );
      aTausF->push_back( ( *aResults )[ size_t( index ) ]->getMatrix( Side::Front, PropertySimple::T ) );
      aTausB->push_back( ( *aResults )[ size_t( index ) ]->getMatrix( Side::Back, PropertySimple::T ) );
      aRhosF->push_back( ( *aResults )[ size_t( index ) ]->getMatrix( Side::Front, PropertySimple::R ) );
      aRhosB->push_back( ( *aResults )[ size_t( index ) ]->getMatrix( Side::Back, PropertySimple::R ) );
    }

    m_TausF.push_back( aTausF );
    m_TausB.push_back( aTausB );
    m_RhosF.push_back( aRhosF );
    m_RhosB.push_back( aRhosB );
  }

  shared_ptr< vector< double > > CAbsorptancesMultiPaneBSDF::Abs( const double minLambda, const double maxLambda, 
    const size_t Index ) {
    if( Index > m_TausF.size() ) {
      throw runtime_error("Index for glazing layer absorptance is out of range.");
    }

    size_t aLayerIndex = layerIndex( Index - 1 );

    if( !m_StateCalculated ) {
      calculateState( minLambda, maxLambda );
      m_StateCalculated = true;
    }

    return m_Abs[ aLayerIndex ];

  }

  shared_ptr< vector< double > > CAbsorptancesMultiPaneBSDF::multVectors( 
    const vector< double >& t_vec1, 
    const vector< double >& t_vec2 ) {
    if( t_vec1.size() != t_vec2.size() ) {
      throw runtime_error("Vectors are not same size.");
    }
    shared_ptr< vector< double > > Result = make_shared< vector< double > >();
    for( size_t i = 0; i < t_vec1.size(); ++i ) {
      double value = t_vec1 [ i ] * t_vec2[ i ];
      Result->push_back( value );
    }
    return Result;
  }

  shared_ptr< vector< double > > CAbsorptancesMultiPaneBSDF::divVectors( 
    const vector< double >& t_vec1, 
    const vector< double >& t_vec2 ) {
    if( t_vec1.size() != t_vec2.size() ) {
      throw runtime_error("Vectors are not same size.");
    }
    shared_ptr< vector< double > > Result = make_shared< vector< double > >();
    for( size_t i = 0; i < t_vec1.size(); ++i ) {
      double value = t_vec1[ i ] / t_vec2[ i ];
      Result->push_back( value );
    }
    return Result;
  }

  shared_ptr< vector< double > > CAbsorptancesMultiPaneBSDF::addVectors( 
    const vector< double >& t_vec1, 
    const vector< double >& t_vec2 ) {
    if( t_vec1.size() != t_vec2.size() ) {
      throw runtime_error("Vectors are not same size.");
    }
    shared_ptr< vector< double > > Result = make_shared< vector< double > >();
    for( size_t i = 0; i < t_vec1.size(); ++i ) {
      double value = t_vec1[ i ] + t_vec2[ i ];
      Result->push_back( value );
    }
    return Result;
  }

  void CAbsorptancesMultiPaneBSDF::calculateState( const double minLambda, const double maxLambda ) {
    size_t numOfWavelengths = m_CommonWavelengths->size();
    size_t matrixSize = ( *( *m_TausF[ 0 ] )[ 0 ] ).getSize();

    // calculation of forward r and t coefficients
    for( size_t i = m_NumOfLayers; i-- > 0; ) {

      // r and t for current layer (number of wavelengths)
      shared_ptr< SquareMatrices > r = make_shared< vector< shared_ptr< CSquareMatrix > > >();
      shared_ptr< SquareMatrices > t = make_shared< vector< shared_ptr< CSquareMatrix > > >();

      shared_ptr< SquareMatrices > vTauF = nullptr;
      shared_ptr< SquareMatrices > vTauB = nullptr;
      shared_ptr< SquareMatrices > vRhoF = nullptr;
      shared_ptr< SquareMatrices > vRhoB = nullptr;

      size_t aLayerIndex = layerIndex( i );

      switch( m_Side ) {
        case Side::Front:
          vTauF = m_TausF[ aLayerIndex ];
          vTauB = m_TausB[ aLayerIndex ];
          vRhoF = m_RhosF[ aLayerIndex ];
          vRhoB = m_RhosB[ aLayerIndex ];
          break;
        case Side::Back:
          vTauF = m_TausB[ aLayerIndex ];
          vTauB = m_TausF[ aLayerIndex ];
          vRhoF = m_RhosB[ aLayerIndex ];
          vRhoB = m_RhosF[ aLayerIndex ];
          break;
        default:
          assert("Incorrect side selection.");
          break;
      }

      for( size_t j = 0; j < numOfWavelengths; ++j ) {

        shared_ptr< CSquareMatrix > aTauF = ( *( vTauF ) )[ j ];
        shared_ptr< CSquareMatrix > aTauB = ( *( vTauB ) )[ j ];
        shared_ptr< CSquareMatrix > aRhoF = ( *( vRhoF ) )[ j ];
        shared_ptr< CSquareMatrix > aRhoB = ( *( vRhoB ) )[ j ];
        
        shared_ptr< CSquareMatrix > aRi = m_Lambda->mult( *aRhoF );

        if( i != m_NumOfLayers - 1 ) {
          shared_ptr< CSquareMatrix > prevR = ( *m_rCoeffs[ i ] )[ j ];
          shared_ptr< CSquareMatrix > lambdaTauF = m_Lambda->mult( *aTauF );
          shared_ptr< CSquareMatrix > Denominator = getDenomForRTCoeff( *aRhoB, *prevR );
          shared_ptr< CSquareMatrix > rsecF = lambdaTauF->mult( *lambdaTauF );
          rsecF = rsecF->mult( *prevR );
          rsecF = rsecF->mult( *Denominator );
          aRi = aRi->add( *rsecF );

          shared_ptr< CSquareMatrix > tfwd = lambdaTauF->mult( *Denominator );
          t->push_back( tfwd );
        } else {
          t->push_back( m_Lambda->mult( *aTauF ) );
        }

        r->push_back( aRi );

      }
      m_rCoeffs.insert( m_rCoeffs.begin(), r );
      m_tCoeffs.insert( m_tCoeffs.begin(), t );
    }

    // For every layer-wavelength set there is a set of incoming/outgoing directions
    vector< SquareMatrices > IminusM( m_NumOfLayers );
    vector< SquareMatrices > IplusM( m_NumOfLayers );

    for( size_t i = 0; i < m_NumOfLayers; ++i ) {
      IminusM[ i ].resize( numOfWavelengths );
      IplusM[ i ].resize( numOfWavelengths );
    }

    // This is true for every incoming wavelength
    shared_ptr< CSquareMatrix > Iincoming = make_shared< CSquareMatrix >( matrixSize );
    Iincoming->setIdentity();

    // calculation irradiances (normalized to 1)
    for( size_t i = 0; i < m_NumOfLayers; ++i ) {
      for( size_t j = 0; j < numOfWavelengths; ++j ) {
        shared_ptr< CSquareMatrix > r = ( *m_rCoeffs[ i ] )[ j ];
        shared_ptr< CSquareMatrix > t = ( *m_tCoeffs[ i ] )[ j ];
        shared_ptr< CSquareMatrix > activeI = nullptr;
        if( i == 0 ) {
          activeI = Iincoming;
        } else {
          activeI = IminusM[ i - 1 ][ j ];
        }
        assert( activeI != nullptr );
        IplusM[ i ][ j ] = r->mult( *activeI );
        IminusM[ i ][ j ] = t->mult( *activeI );
      }
    }

    // Convert to incoming/outgoing energy for every direction
    vector< vector< shared_ptr< vector< double > > > > IminusV( m_NumOfLayers );
    vector< vector< shared_ptr< vector< double > > > > IplusV( m_NumOfLayers );

    for( size_t i = 0; i < m_NumOfLayers; ++i ) {
      IminusV[ i ].resize( numOfWavelengths );
      IplusV[ i ].resize( numOfWavelengths );
    }

    shared_ptr< vector< double > > One = make_shared< vector< double > >();
    for( size_t i = 0; i < matrixSize; ++i ) {
      One->push_back( 1 );
    }


    for( size_t j = 0; j < m_NumOfLayers; ++j ) {
      for( size_t k = 0; k < m_CommonWavelengths->size(); ++k ) {
        IminusV[ j ][ k ] = IminusM[ j ][ k ]->multVxM( *One );
        IplusV[ j ][ k ] = IplusM[ j ][ k ]->multVxM( *One );
      }
    }

    m_Abs.clear();
    m_Abs.resize( m_NumOfLayers );

    for( size_t i = 0; i < m_NumOfLayers; ++i ) {
      m_Abs[ i ] = make_shared< vector< double > >( matrixSize );
    }

    double totalSolar = m_SolarRadiation->integrate( IntegrationType::Trapezoidal )->sum( minLambda, maxLambda );

    // calculation of solar absorptances
    for( size_t i = 0; i < matrixSize; ++i ) {
      for( size_t j = 0; j < m_NumOfLayers; ++j ) {
        shared_ptr< CSeries > curSpectralProperties = make_shared< CSeries >();
        for( size_t k = 0; k < m_CommonWavelengths->size(); ++k ) {
          double IminusIncoming = 0;
          double IminusOutgoing = 0;
          double IplusIncoming = 0;
          double IplusOutgoing = 0;
          
          IminusOutgoing = ( *( IminusV[ j ][ k ] ) )[ i ];
          IplusOutgoing = ( *( IplusV[ j ][ k ] ) )[ i ];
          if( j == 0 ) {
            IminusIncoming = 1;
          } else {
            IminusIncoming = ( *( IminusV[ j - 1 ][ k ] ) )[ i ];
          }
    
          if( j == m_NumOfLayers - 1 ) {
            IplusIncoming = 0;
          } else {
            IplusIncoming = ( *( IplusV[ j + 1 ][ k ] ) )[ i ];
          }
    
          double absValue = IminusIncoming + IplusIncoming - IminusOutgoing - IplusOutgoing;
          curSpectralProperties->addProperty( ( *m_CommonWavelengths )[ k ], absValue );
          
        }
    
        shared_ptr< CSeries > absorbedIrradiance = curSpectralProperties->mMult( *m_SolarRadiation );
        shared_ptr< CSeries > integratedAbsorbed = absorbedIrradiance->integrate( IntegrationType::Trapezoidal );
        double value = integratedAbsorbed->sum( minLambda, maxLambda );
        value = value / totalSolar;
        ( *m_Abs[ j ] )[ i ] = value;
      }
    }
    
    m_StateCalculated = true;

  }

  shared_ptr< CSquareMatrix > CAbsorptancesMultiPaneBSDF::getDenomForRTCoeff( 
    const CSquareMatrix& t_Reflectance,
    const CSquareMatrix& t_PreviousR ) {
    size_t matrixSize = t_Reflectance.getSize();
    shared_ptr< CSquareMatrix > Denominator = make_shared< CSquareMatrix >( matrixSize );
    Denominator->setIdentity();
    shared_ptr< CSquareMatrix > lambdaRF = m_Lambda->mult( t_Reflectance );
    lambdaRF = lambdaRF->mult( t_PreviousR );
    Denominator = Denominator->sub( *lambdaRF );
    Denominator = Denominator->inverse();
    return Denominator;
  }

  size_t CAbsorptancesMultiPaneBSDF::layerIndex( const size_t Index ) const {
    size_t aLayerIndex = 0;
    switch( m_Side ) {
    case Side::Front:
      aLayerIndex = Index;
      break;
    case Side::Back:
      aLayerIndex = m_NumOfLayers - Index - 1;
      break;
    default:
      assert("Incorrect side selection.");
      break;
    }
    return aLayerIndex;
  }

}