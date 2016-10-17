#include <assert.h>

#include "BSDFResults.hpp"
#include "SquareMatrix.hpp"
#include "EquivalentBSDFLayerSingleBand.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;
using namespace SingleLayerOptics;

namespace MultiLayerOptics {
 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CInterReflectance
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  CInterReflectance::CInterReflectance( const CSquareMatrix& t_Lambda, 
    const CSquareMatrix& t_Rb,
    const CSquareMatrix& t_Rf ) {
    size_t size = t_Lambda.getSize();
    CSquareMatrix lRb = *t_Lambda.mult( t_Rb );
    CSquareMatrix lRf = *t_Lambda.mult( t_Rf );
    m_InterRefl = lRb.mult( lRf );
    CSquareMatrix I = CSquareMatrix( size );
    I.setIdentity();
    m_InterRefl = I.sub( *m_InterRefl );
    m_InterRefl = m_InterRefl->inverse();
  }

  shared_ptr< CSquareMatrix > CInterReflectance::value() const {
    return m_InterRefl;
  }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CBSDFDoubleLayer
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  CBSDFDoubleLayer::CBSDFDoubleLayer( const CBSDFResults& t_FrontLayer, const CBSDFResults& t_BackLayer ) {
    const CSquareMatrix aLambda = *t_FrontLayer.lambdaMatrix();
    CInterReflectance InterRefl1 = CInterReflectance( aLambda, *t_FrontLayer.Rho( Side::Back ), 
      *t_BackLayer.Rho( Side::Front ) );

    CInterReflectance InterRefl2 = CInterReflectance( aLambda, *t_BackLayer.Rho( Side::Front ), 
      *t_FrontLayer.Rho( Side::Back ) );

    m_Tf = equivalentT( *t_BackLayer.Tau( Side::Front ), *InterRefl1.value(), aLambda, *t_FrontLayer.Tau( Side::Front ) );
    m_Tb = equivalentT( *t_FrontLayer.Tau( Side::Back ), *InterRefl2.value(), aLambda, *t_BackLayer.Tau( Side::Back ) );
    m_Rf = equivalentR( *t_FrontLayer.Rho( Side::Front ), *t_FrontLayer.Tau( Side::Front ), *t_FrontLayer.Tau( Side::Back ),
      *t_BackLayer.Rho( Side::Front ), *InterRefl2.value(), aLambda );
    m_Rb = equivalentR( *t_BackLayer.Rho( Side::Back ), *t_BackLayer.Tau( Side::Back ), *t_BackLayer.Tau( Side::Front ),
      *t_FrontLayer.Rho( Side::Back ), *InterRefl1.value(), aLambda );

    m_Results = make_shared< CBSDFResults >( t_FrontLayer.getDirections() );
    m_Results->setResultMatrices( m_Tf, m_Rf, Side::Front );
    m_Results->setResultMatrices( m_Tb, m_Rb, Side::Back );

  }

  shared_ptr< CBSDFResults > CBSDFDoubleLayer::value() {
    return m_Results; 
  }

  shared_ptr< CSquareMatrix > CBSDFDoubleLayer::equivalentT( const CSquareMatrix& t_Tf2, 
    const CSquareMatrix& t_InterRefl, const CSquareMatrix& t_Lambda,
    const CSquareMatrix& t_Tf1 ) {
    CSquareMatrix TinterRefl = *t_Tf2.mult( t_InterRefl );
    CSquareMatrix lambdaTf1 = *t_Lambda.mult( t_Tf1 );
    shared_ptr< CSquareMatrix > aResult = TinterRefl.mult( lambdaTf1 );
    return aResult;
  }

  shared_ptr< CSquareMatrix > CBSDFDoubleLayer::equivalentR( const CSquareMatrix& t_Rf1,
    const CSquareMatrix& t_Tf1, const CSquareMatrix& t_Tb1, 
    const CSquareMatrix& t_Rf2, const CSquareMatrix& t_InterRefl, 
    const CSquareMatrix& t_Lambda ) {
    CSquareMatrix TinterRefl = *t_Tb1.mult( t_InterRefl );
    CSquareMatrix lambdaRf2 = *t_Lambda.mult( t_Rf2 );
    CSquareMatrix lambdaTf1 = *t_Lambda.mult( t_Tf1 );
    TinterRefl = *TinterRefl.mult( lambdaRf2 );
    TinterRefl = *TinterRefl.mult( lambdaTf1 );
    shared_ptr< CSquareMatrix > aResult = t_Rf1.add( TinterRefl );
    return aResult;
  }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CEquivalentBSDFLayerSingleBand
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  CEquivalentBSDFLayerSingleBand::CEquivalentBSDFLayerSingleBand( const shared_ptr< CBSDFResults >& t_Layer ) : 
    m_PropertiesCalculated( false ) {
    m_EquivalentLayer = make_shared< CBSDFResults >( t_Layer->getDirections() );
    for( Side aSide : EnumSide() ) {
      m_A[ aSide ] = make_shared< vector< shared_ptr< vector< double > > > >();
    }
    m_Layers.push_back( t_Layer );
    m_Lambda = t_Layer->lambdaMatrix();
  }

  shared_ptr< CSquareMatrix > CEquivalentBSDFLayerSingleBand::Tau( const Side t_Side ) {
    calcEquivalentProperties();
    return m_EquivalentLayer->Tau( t_Side );
  }

  shared_ptr< CSquareMatrix > CEquivalentBSDFLayerSingleBand::Rho( const Side t_Side ) {
    calcEquivalentProperties();
    return m_EquivalentLayer->Rho( t_Side );
  }

  shared_ptr< CSquareMatrix> CEquivalentBSDFLayerSingleBand::getProperty( const Side t_Side, const 
    PropertySimple t_Property ) {
    shared_ptr< CSquareMatrix > aMatrix = nullptr;
    switch( t_Property ) {
    case PropertySimple::T:
      aMatrix = Tau( t_Side );
      break;
    case PropertySimple::R:
      aMatrix = Rho( t_Side );
      break;
    default:
      assert("Incorrect selection of layer property.");
      break;
    }
    return aMatrix;
  }

  shared_ptr< vector< double > > CEquivalentBSDFLayerSingleBand::getLayerAbsorptances( const size_t Index, 
    Side t_Side ) {
    calcEquivalentProperties();
    return ( *m_A.at( t_Side ) )[ Index - 1 ];
  }

  size_t CEquivalentBSDFLayerSingleBand::getNumberOfLayers() const {
    return m_Layers.size();
  }

  void CEquivalentBSDFLayerSingleBand::addLayer( const std::shared_ptr< SingleLayerOptics::CBSDFResults >& t_Layer ) {
    m_Layers.push_back( t_Layer );
    m_PropertiesCalculated = false;
    for( Side aSide : EnumSide() ) {
      m_A.at( aSide )->clear();
    }
  }

  void CEquivalentBSDFLayerSingleBand::calcEquivalentProperties() {
    if( m_PropertiesCalculated ) {
      return;
    }
    // Absorptance calculations need to observe every layer in isolation. For that purpose
    // code bellow will create m_Forward and m_Backward layers
    size_t size = m_Layers.size();
    m_EquivalentLayer = m_Layers[ 0 ];
    m_Forward.push_back( m_EquivalentLayer );
    for( size_t i = 1; i < size; ++i ) {
      m_EquivalentLayer = CBSDFDoubleLayer( *m_EquivalentLayer, *m_Layers[ i ] ).value();
      m_Forward.push_back( m_EquivalentLayer );
    }
    m_Backward.push_back( m_EquivalentLayer );

    shared_ptr< CBSDFResults > bLayer = m_Layers[ size - 1 ];
    for( size_t i = size - 1; i > 1; --i ) {
      bLayer = CBSDFDoubleLayer( *m_Layers[ i - 1 ], *bLayer ).value();
      m_Backward.push_back( bLayer );
    }
    m_Backward.push_back( m_Layers[ size - 1 ] );

    size_t matrixSize = m_Lambda->getSize();
    shared_ptr< vector< double > > zeros = make_shared< vector< double > >( matrixSize );

    shared_ptr< vector< double > > Ap1f = nullptr;
    shared_ptr< vector< double > > Ap2f = nullptr;
    shared_ptr< vector< double > > Ap1b = nullptr;
    shared_ptr< vector< double > > Ap2b = nullptr;

    for( size_t i = 0; i < size; i++ ) {
      if( i == size - 1 ) {
        Ap2f = zeros;
        Ap1b = m_Layers[ i ]->Abs( Side::Back );
      } else {
        CBSDFResults& Layer1 = *m_Backward[ i + 1 ];
        CBSDFResults& Layer2 = *m_Forward[ i ];
        CInterReflectance InterRefl2 = 
          CInterReflectance( *m_Lambda, *Layer1.Rho( Side::Front ), *Layer2.Rho( Side::Back ) );
        vector< double >& Ab = *m_Layers[ i ]->Abs( Side::Back );
        Ap1b = absTerm1( Ab, *InterRefl2.value(), *Layer1.Tau( Side::Back ) );
        Ap2f = absTerm2( Ab, *InterRefl2.value(), *Layer1.Rho( Side::Front ), *Layer2.Tau( Side::Front) );        
      }

      if( i == 0 ) {
        Ap1f = m_Layers[ i ]->Abs( Side::Front );
        Ap2b = zeros;
      } else {
        CBSDFResults& Layer1 = *m_Forward[ i - 1 ];
        CBSDFResults& Layer2 = *m_Backward[ i ];
        CInterReflectance InterRefl1 = 
          CInterReflectance( *m_Lambda, *Layer1.Rho( Side::Back ), *Layer2.Rho( Side::Front ) );
        vector< double >& Af = *m_Layers[ i ]->Abs( Side::Front );
        Ap1f = absTerm1( Af, *InterRefl1.value(), *Layer1.Tau( Side::Front ) );
        Ap2b = absTerm2( Af, *InterRefl1.value(), *Layer1.Rho( Side::Back), *Layer2.Tau( Side::Back ) );
      }

      map< Side, shared_ptr< vector< double > > > aTotal;
      for( Side aSide : EnumSide() ) {
        aTotal[ aSide ] = make_shared< vector< double > >();
      }
      for( size_t j = 0; j < matrixSize; ++j ) {
        aTotal.at( Side::Front )->push_back( ( *Ap1f )[ j ] + ( *Ap2f )[ j ] );
        aTotal.at( Side::Back )->push_back( ( *Ap1b )[ j ] + ( *Ap2b )[ j ] );
      }

      for( Side aSide : EnumSide() ) {
        m_A.at( aSide )->push_back( aTotal.at( aSide ) );
      }

    }
    m_PropertiesCalculated = true;
  }

  shared_ptr< vector< double > > CEquivalentBSDFLayerSingleBand::absTerm1( const vector< double >& t_Alpha,
    const CSquareMatrix& t_InterRefl, const CSquareMatrix& t_T ) {
    shared_ptr< vector< double > > part1 = t_InterRefl.multVxM( t_Alpha );
    CSquareMatrix part2 = *m_Lambda->mult( t_T );
    part1 = part2.multVxM( *part1 );
    return part1;
  }

  shared_ptr< vector< double > > CEquivalentBSDFLayerSingleBand::absTerm2( const vector< double >& t_Alpha,
    const CSquareMatrix& t_InterRefl, const CSquareMatrix& t_R, const CSquareMatrix& t_T ) {
    shared_ptr< vector< double > > part1 = t_InterRefl.multVxM( t_Alpha );
    CSquareMatrix part2 = *m_Lambda->mult( t_R );
    CSquareMatrix part3 = *m_Lambda->mult( t_T );
    part1 = part2.multVxM( *part1 );
    part1 = part3.multVxM( *part1 );
    return part1;
  }

}