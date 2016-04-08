#include <assert.h>

#include "BSDFResults.hpp"
#include "SquareMatrix.hpp"
#include "EquivalentBSDFLayerSingleBand.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;
using namespace LayerOptics;

namespace MultiPane {
 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CInterReflectance
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  CInterReflectance::CInterReflectance( shared_ptr< const CSquareMatrix > t_Lambda, 
    shared_ptr< const CSquareMatrix > t_Rb,
    shared_ptr< const CSquareMatrix > t_Rf ) {
    size_t size = t_Lambda->getSize();
    shared_ptr< CSquareMatrix > lRb = t_Lambda->mult( *t_Rb );
    shared_ptr< CSquareMatrix > lRf = t_Lambda->mult( *t_Rf );
    m_InterRefl = lRb->mult( *lRf );
    shared_ptr< CSquareMatrix > I = make_shared< CSquareMatrix >( size );
    I->setIdentity();
    m_InterRefl = I->sub( *m_InterRefl );
    m_InterRefl = m_InterRefl->inverse();
  };

  shared_ptr< CSquareMatrix > CInterReflectance::value() const {
    return m_InterRefl;
  };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CBSDFDoubleLayer
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  CBSDFDoubleLayer::CBSDFDoubleLayer( shared_ptr< CBSDFResults > t_FrontLayer, shared_ptr< CBSDFResults > t_BackLayer ) {
    shared_ptr< const CSquareMatrix > aLambda = t_FrontLayer->lambdaMatrix();
    CInterReflectance InterRefl1 = CInterReflectance( aLambda, t_FrontLayer->Rho( Side::Back ), 
      t_BackLayer->Rho( Side::Front ) );

    CInterReflectance InterRefl2 = CInterReflectance( aLambda, t_BackLayer->Rho( Side::Front ), 
      t_FrontLayer->Rho( Side::Back ) );

    m_Tf = equivalentT( t_BackLayer->Tau( Side::Front ), InterRefl1.value(), aLambda, t_FrontLayer->Tau( Side::Front ) );
    m_Tb = equivalentT( t_FrontLayer->Tau( Side::Back ), InterRefl2.value(), aLambda, t_BackLayer->Tau( Side::Back ) );
    m_Rf = equivalentR( t_FrontLayer->Rho( Side::Front ), t_FrontLayer->Tau( Side::Front ), t_FrontLayer->Tau( Side::Back ),
      t_BackLayer->Rho( Side::Front ), InterRefl2.value(), aLambda );
    m_Rb = equivalentR( t_BackLayer->Rho( Side::Back ), t_BackLayer->Tau( Side::Back ), t_BackLayer->Tau( Side::Front ),
      t_FrontLayer->Rho( Side::Back ), InterRefl1.value(), aLambda );

    m_Results = make_shared< CBSDFResults >( t_FrontLayer->getDirections() );
    m_Results->setResultMatrices( m_Tf, m_Rf, Side::Front );
    m_Results->setResultMatrices( m_Tb, m_Rb, Side::Back );

  };

  shared_ptr< CBSDFResults > CBSDFDoubleLayer::value() {
    return m_Results; 
  };

  shared_ptr< CSquareMatrix > CBSDFDoubleLayer::equivalentT( shared_ptr< const CSquareMatrix > t_Tf2, 
    shared_ptr< const CSquareMatrix > t_InterRefl, shared_ptr< const CSquareMatrix > t_Lambda,
    shared_ptr< const CSquareMatrix > t_Tf1 ) {
    shared_ptr< CSquareMatrix > TinterRefl = t_Tf2->mult( *t_InterRefl );
    shared_ptr< CSquareMatrix > lambdaTf1 = t_Lambda->mult( *t_Tf1 );
    shared_ptr< CSquareMatrix > aResult = TinterRefl->mult( *lambdaTf1 );
    return aResult;
  };

  shared_ptr< CSquareMatrix > CBSDFDoubleLayer::equivalentR( shared_ptr< const CSquareMatrix > t_Rf1,
    shared_ptr< const CSquareMatrix > t_Tf1, shared_ptr< const CSquareMatrix > t_Tb1, 
    shared_ptr< const CSquareMatrix > t_Rf2, shared_ptr< const CSquareMatrix > t_InterRefl, 
    shared_ptr< const CSquareMatrix > t_Lambda ) {
    shared_ptr< CSquareMatrix > TinterRefl = t_Tb1->mult( *t_InterRefl );
    shared_ptr< CSquareMatrix > lambdaRf2 = t_Lambda->mult( *t_Rf2 );
    shared_ptr< CSquareMatrix > lambdaTf1 = t_Lambda->mult( *t_Tf1 );
    TinterRefl = TinterRefl->mult( *lambdaRf2 );
    TinterRefl = TinterRefl->mult( *lambdaTf1 );
    shared_ptr< CSquareMatrix > aResult = t_Rf1->add( *TinterRefl );
    return aResult;
  };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CEquivalentBSDFLayerSingleBand
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  CEquivalentBSDFLayerSingleBand::CEquivalentBSDFLayerSingleBand( shared_ptr< CBSDFResults > t_Layer ) : m_PropertiesCalculated( false ) {
    m_EquivalentLayer = make_shared< CBSDFResults >( t_Layer->getDirections() );
    m_Af = make_shared< vector< shared_ptr< vector< double > > > >();
    m_Ab = make_shared< vector< shared_ptr< vector< double > > > >();
    m_Layers.push_back( t_Layer );
    m_Lambda = t_Layer->lambdaMatrix();
  }

  shared_ptr< CSquareMatrix > CEquivalentBSDFLayerSingleBand::Tau( Side t_Side ) {
    calcEquivalentProperties();
    return m_EquivalentLayer->Tau( t_Side );
  };

  shared_ptr< CSquareMatrix > CEquivalentBSDFLayerSingleBand::Rho( Side t_Side ) {
    calcEquivalentProperties();
    return m_EquivalentLayer->Rho( t_Side );
  };

  shared_ptr< vector< double > > CEquivalentBSDFLayerSingleBand::getLayerAbsorptances( const size_t Index, 
    Side t_Side ) {
    calcEquivalentProperties();
    shared_ptr< vector< double > > Abs = nullptr;
    switch( t_Side ) {
    case Side::Front:
      Abs = ( *m_Af )[ Index - 1 ];
      break;
    case Side::Back:
      Abs = ( *m_Ab )[ Index - 1 ];
      break;
    default:
      assert("Incorrect side selection.");
      break;
    }

    return Abs;
  };

  size_t CEquivalentBSDFLayerSingleBand::getNumberOfLayers() const {
    return m_Layers.size();
  };

  void CEquivalentBSDFLayerSingleBand::addLayer( std::shared_ptr< LayerOptics::CBSDFResults > t_Layer ) {
    m_Layers.push_back( t_Layer );
    m_PropertiesCalculated = false;
    m_Af->clear();
    m_Ab->clear();
  };

  void CEquivalentBSDFLayerSingleBand::calcEquivalentProperties() {
    if( m_PropertiesCalculated ) {
      return;
    }
    size_t size = m_Layers.size();
    m_EquivalentLayer = m_Layers[ 0 ];
    m_Forward.push_back( m_EquivalentLayer );
    for( size_t i = 1; i < size; ++i ) {
      m_EquivalentLayer = CBSDFDoubleLayer( m_EquivalentLayer, m_Layers[ i ] ).value();
      m_Forward.push_back( m_EquivalentLayer );
    }
    m_Backward.push_back( m_EquivalentLayer );

    shared_ptr< CBSDFResults > bLayer = m_Layers[ size - 1 ];
    for( size_t i = size - 1; i > 1; --i ) {
      bLayer = CBSDFDoubleLayer( m_Layers[ i - 1 ], bLayer ).value();
      m_Backward.push_back( bLayer );
    }
    m_Backward.push_back( m_Layers[ size - 1 ] );

    size_t matrixSize = m_Lambda->getSize();
    shared_ptr< vector< double > > zeros = make_shared< vector< double > >( matrixSize );
    // Check if need to fill with zeros

    shared_ptr< vector< double > > Ap1f = nullptr;
    shared_ptr< vector< double > > Ap2f = nullptr;
    shared_ptr< vector< double > > Ap1b = nullptr;
    shared_ptr< vector< double > > Ap2b = nullptr;

    for( size_t i = 0; i < size; i++ ) {
      if( i == size - 1 ) {
        Ap2f = zeros;
        Ap1b = m_Layers[ i ]->Abs( Side::Back );
      } else {
        shared_ptr< CBSDFResults > Layer1 = m_Backward[ i + 1 ];
        shared_ptr< CBSDFResults > Layer2 = m_Forward[ i ];
        shared_ptr< CInterReflectance > InterRefl2 = 
          make_shared< CInterReflectance >( m_Lambda, Layer1->Rho( Side::Front ), Layer2->Rho( Side::Back ) );
        shared_ptr< vector< double > > Ab = m_Layers[ i ]->Abs( Side::Back );
        Ap2f = absTerm2( Ab, InterRefl2->value(), m_Backward[ i + 1]->Rho( Side::Front ), 
          m_Forward[ i ]->Tau( Side::Front) );
        Ap1b = absTerm1( Ab, InterRefl2->value(), m_Backward[ i + 1 ]->Tau( Side::Back ) );
      }

      if( i == 0 ) {
        Ap1f = m_Layers[ i ]->Abs( Side::Front );
        Ap2b = zeros;
      } else {
        shared_ptr< CBSDFResults > Layer1 = m_Forward[ i - 1 ];
        shared_ptr< CBSDFResults > Layer2 = m_Backward[ i ];
        shared_ptr< CInterReflectance > InterRefl1 = 
          make_shared< CInterReflectance >( m_Lambda, Layer1->Rho( Side::Back ), Layer2->Rho( Side::Front ) );
        shared_ptr< vector< double > > Af = m_Layers[ i ]->Abs( Side::Front );
        Ap1f = absTerm1( Af, InterRefl1->value(), m_Forward[ i - 1]->Tau( Side::Front ) );
        Ap2b = absTerm2( Af, InterRefl1->value(), m_Forward[ i - 1 ]->Rho( Side::Back) , 
          m_Backward[ i ]->Tau( Side::Back ) );
      }

      shared_ptr< vector< double > > AfTotal = make_shared< vector< double > >();
      shared_ptr< vector< double > > AbTotal = make_shared< vector< double > >();
      for( size_t i = 0; i < matrixSize; ++i ) {
        AfTotal->push_back( ( *Ap1f )[ i ] + ( *Ap2f )[ i ] );
        AbTotal->push_back( ( *Ap1b )[ i ] + ( *Ap2b )[ i ] );
      }

      m_Af->push_back( AfTotal );
      m_Ab->push_back( AbTotal );

    }
    m_PropertiesCalculated = true;
  };

  shared_ptr< vector< double > > CEquivalentBSDFLayerSingleBand::absTerm1( shared_ptr< vector< double > > t_Alpha,
    shared_ptr< CSquareMatrix > t_InterRefl, shared_ptr< CSquareMatrix > t_T ) {
    shared_ptr< vector< double > > part1 = t_InterRefl->multVxM( *t_Alpha );
    shared_ptr< CSquareMatrix > part2 = m_Lambda->mult( *t_T );
    part1 = part2->multVxM( *part1 );
    return part1;
  };

  shared_ptr< vector< double > > CEquivalentBSDFLayerSingleBand::absTerm2( shared_ptr< vector< double > > t_Alpha,
    shared_ptr< CSquareMatrix > t_InterRefl, shared_ptr< CSquareMatrix > t_R,
    shared_ptr< CSquareMatrix > t_T ) {
    shared_ptr< vector< double > > part1 = t_InterRefl->multVxM( *t_Alpha );
    shared_ptr< CSquareMatrix > part2 = m_Lambda->mult( *t_R );
    shared_ptr< CSquareMatrix > part3 = m_Lambda->mult( *t_T );
    part1 = part2->multVxM( *part1 );
    part1 = part3->multVxM( *part1 );
    return part1;
  };

}