#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>

#include "BSDFResults.hpp"
#include "BSDFDirections.hpp"
#include "BSDFPatch.hpp"
#include "SquareMatrix.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace SingleLayerOptics {

  CBSDFResults::CBSDFResults( const shared_ptr< const CBSDFDirections >& t_Directions ) :
    m_HemisphericalCalculated( false ) {
    m_Directions = t_Directions;
    m_DimMatrices = m_Directions->size();

    for( Side t_Side : EnumSide() ) {
      m_Tau[ t_Side ] = make_shared< CSquareMatrix >( m_DimMatrices );
      m_Rho[ t_Side ] = make_shared< CSquareMatrix >( m_DimMatrices );
      m_VTauHem[ t_Side ] = make_shared< vector< double > >( m_DimMatrices );
      m_VRhoHem[ t_Side ] = make_shared< vector< double > >( m_DimMatrices );
    }
  }

  shared_ptr< const CBSDFDirections > CBSDFResults::getDirections() const {
    return m_Directions; 
  }

  double CBSDFResults::TauDiffDiff( const Side t_Side ) const {
    return integrate( *Tau( t_Side ) );
  }

  double CBSDFResults::RhoDiffDiff( const Side t_Side ) const {
    return integrate( *Rho( t_Side ) );
  }

  shared_ptr< CSquareMatrix > CBSDFResults::Tau( const Side t_Side ) const {
    return m_Tau.at( t_Side ); 
  }

  shared_ptr< CSquareMatrix > CBSDFResults::Rho( const Side t_Side ) const {
    return m_Rho.at( t_Side );  
  }

  void CBSDFResults::setResultMatrices( const shared_ptr< CSquareMatrix >& t_Tau, 
    const shared_ptr< CSquareMatrix >& t_Rho, Side t_Side ) {
    m_Tau[ t_Side ] = t_Tau;
    m_Rho[ t_Side ] = t_Rho;
  }

  double CBSDFResults::TauDirDir( const FenestrationCommon::Side t_Side, 
    const double t_Theta, const double t_Phi ) {
    size_t index = m_Directions->getNearestBeamIndex( t_Theta, t_Phi );
    double lambda = ( *m_Directions->lambdaVector() )[ index ];
    double tau = ( *Tau( t_Side ) )[ index ][ index ];
    return tau * lambda;
  }

  double CBSDFResults::RhoDirDir( const FenestrationCommon::Side t_Side,
    const double t_Theta, const double t_Phi ) {
    size_t index = m_Directions->getNearestBeamIndex( t_Theta, t_Phi );
    double lambda = ( *m_Directions->lambdaVector() )[ index ];
    double rho = ( *Rho( t_Side ) )[ index ][ index ];
    return rho * lambda;
  }

  shared_ptr< vector< double > > CBSDFResults::TauDirHem( Side t_Side ) {
    calcHemispherical();
    return m_VTauHem.at( t_Side );
  }

  shared_ptr< vector< double > > CBSDFResults::RhoDirHem( Side t_Side ) {
    calcHemispherical();
    return m_VRhoHem.at( t_Side );
  }

  shared_ptr< vector< double > > CBSDFResults::Abs( Side t_Side ) {
    calcHemispherical();
    return m_Abs.at( t_Side );  
  }

  double CBSDFResults::TauDirHem( const Side t_Side, const double t_Theta, const double t_Phi ) {
    size_t index = m_Directions->getNearestBeamIndex( t_Theta, t_Phi );
    return ( *TauDirHem( t_Side ) )[ index ];
  }

  double CBSDFResults::RhoDirHem( const Side t_Side, const double t_Theta, const double t_Phi ) {
    size_t index = m_Directions->getNearestBeamIndex( t_Theta, t_Phi );
    return ( *RhoDirHem( t_Side ) )[ index ];
  }

  double CBSDFResults::Abs( const Side t_Side, const double t_Theta, const double t_Phi ) {
    size_t index = m_Directions->getNearestBeamIndex( t_Theta, t_Phi );
    return ( *Abs( t_Side ) )[ index ];
  }

  shared_ptr< const vector< double > > CBSDFResults::lambdaVector() const {
    return m_Directions->lambdaVector();
  }

  shared_ptr< const CSquareMatrix > CBSDFResults::lambdaMatrix() const {
    return m_Directions->lambdaMatrix();
  }

  double CBSDFResults::integrate( CSquareMatrix& t_Matrix ) const {
    double sum = 0;
    for( size_t i = 0; i < m_DimMatrices; ++i ) {
      for( size_t j = 0; j < m_DimMatrices; ++j ) {
        sum += t_Matrix[ i ][ j ] * ( *m_Directions )[ i ]->lambda() * ( *m_Directions )[ j ]->lambda();
      }
    }
    return sum / M_PI;
  }

  void CBSDFResults::calcHemispherical() {
    if( !m_HemisphericalCalculated ) {
      for( Side t_Side : EnumSide() ) {
        m_VTauHem[ t_Side ] = m_Tau.at( t_Side )->multVxM( *m_Directions->lambdaVector() );
        m_VRhoHem[ t_Side ] = m_Rho.at( t_Side )->multVxM( *m_Directions->lambdaVector() );
        m_Abs[ t_Side ] = make_shared< vector< double > >();
      }

      size_t size = m_VTauHem[ Side::Front ]->size();
      for( size_t i = 0; i < size; ++i ) {
        for( Side t_Side : EnumSide() ) {
          m_Abs.at( t_Side )->push_back( 1 - ( *m_VTauHem.at( t_Side ) )[ i ] - ( *m_VRhoHem.at( t_Side ) )[ i ] );
        }
      }
      m_HemisphericalCalculated = true;
    }
  }

}