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

namespace LayerOptics {

  CBSDFResults::CBSDFResults( shared_ptr< const CBSDFDirections > t_Directions ) :
    m_HemisphericalCalculated( false ) {
    m_Directions = t_Directions;
    m_DimMatrices = m_Directions->size();
    
    m_TauF = make_shared< CSquareMatrix >( m_DimMatrices );
    m_TauB = make_shared< CSquareMatrix >( m_DimMatrices );
    m_RhoF = make_shared< CSquareMatrix >( m_DimMatrices );
    m_RhoB = make_shared< CSquareMatrix >( m_DimMatrices );

    m_VTauHemF = make_shared< vector< double > >( m_DimMatrices );
    m_VTauHemB = make_shared< vector< double > >( m_DimMatrices );
    m_VRhoHemF = make_shared< vector< double > >( m_DimMatrices );
    m_VRhoHemB = make_shared< vector< double > >( m_DimMatrices );
  };

  shared_ptr< const CBSDFDirections > CBSDFResults::getDirections() const {
    return m_Directions; 
  };

  double CBSDFResults::TauDiff( Side t_Side ) const {
    double Tdiff = 0;
    switch( t_Side ) {
    case Side::Front:
      Tdiff = integrate( m_TauF );
      break;
    case Side::Back:
      Tdiff = integrate( m_TauB );
      break;
    default:
      assert("Incorrect selection of side.");
      break;
    }
    return Tdiff;
  };

  double CBSDFResults::RhoDiff( Side t_Side ) const {
    double Rdiff = 0;
    switch( t_Side ) {
    case Side::Front:
      Rdiff = integrate( m_RhoF );
      break;
    case Side::Back:
      Rdiff = integrate( m_RhoB );
      break;
    default:
      assert("Incorrect selection of side.");
      break;
    }
    return Rdiff;
  };

  shared_ptr< CSquareMatrix > CBSDFResults::Tau( Side t_Side ) const {
    shared_ptr< CSquareMatrix > Tau = nullptr;
    switch( t_Side ) {
    case Side::Front:
      Tau = m_TauF;
      break;
    case Side::Back:
      Tau = m_TauB;
      break;
    default:
      assert("Incorrect selection of side.");
      break;
    }
    return Tau;  
  };

  shared_ptr< CSquareMatrix > CBSDFResults::Rho( Side t_Side ) const {
    shared_ptr< CSquareMatrix > Rho = nullptr;
    switch( t_Side ) {
    case Side::Front:
      Rho = m_RhoF;
      break;
    case Side::Back:
      Rho = m_RhoB;
      break;
    default:
      assert("Incorrect selection of side.");
      break;
    }
    return Rho;  
  };

  void CBSDFResults::setResultMatrices( shared_ptr< CSquareMatrix > t_Tau, shared_ptr< CSquareMatrix > t_Rho, Side t_Side ) {
    //m_HemisphericalCalculated = false;
    switch( t_Side ) {
    case Side::Front:
      m_TauF = t_Tau;
      m_RhoF = t_Rho;
      break;
    case Side::Back:
      m_TauB = t_Tau;
      m_RhoB = t_Rho;
      break;
    default:
      assert("Incorrect selection of side.");
      break;
    }
  };

  shared_ptr< vector< double > > CBSDFResults::TauHem( Side t_Side ) {
    calcHemispherical();
    shared_ptr< vector< double > > TauHem = nullptr;
    switch( t_Side ) {
    case Side::Front:
      TauHem = m_VTauHemF;
      break;
    case Side::Back:
      TauHem = m_VTauHemB;
      break;
    default:
      assert("Incorrect selection of side.");
      break;
    }
    return TauHem;
  };

  shared_ptr< vector< double > > CBSDFResults::RhoHem( Side t_Side ) {
    calcHemispherical();
    shared_ptr< vector< double > > RhoHem = nullptr;
    switch( t_Side ) {
    case Side::Front:
      RhoHem = m_VRhoHemF;
      break;
    case Side::Back:
      RhoHem = m_VRhoHemB;
      break;
    default:
      assert("Incorrect selection of side.");
      break;
    }
    return RhoHem;
  };

  shared_ptr< vector< double > > CBSDFResults::Abs( Side t_Side ) {
    calcHemispherical();
    shared_ptr< vector< double > > Abs = nullptr;
    switch( t_Side ) {
    case Side::Front:
      Abs = m_AbsF;
      break;
    case Side::Back:
      Abs = m_AbsB;
      break;
    default:
      assert("Incorrect selection of side.");
      break;
    }
    return Abs;  
  };

  shared_ptr< const vector< double > > CBSDFResults::lambdaVector() const {
    return m_Directions->lambdaVector();
  };

  shared_ptr< const CSquareMatrix > CBSDFResults::lambdaMatrix() const {
    return m_Directions->lambdaMatrix(); 
  };

  double CBSDFResults::integrate( shared_ptr< CSquareMatrix > t_Matrix ) const {
    double sum = 0;
    for( size_t i = 0; i < m_DimMatrices; ++i ) {
      for( size_t j = 0; j < m_DimMatrices; ++j ) {
        sum += (*t_Matrix)[i][j] * (*m_Directions)[i]->lambda() * (*m_Directions)[j]->lambda();
      }
    }
    return sum / M_PI;
  };

  void CBSDFResults::calcHemispherical() {
    if( !m_HemisphericalCalculated ) {
      m_VTauHemF = m_TauF->multVxM( *m_Directions->lambdaVector() );
      m_VTauHemB = m_TauB->multVxM( *m_Directions->lambdaVector() );
      m_VRhoHemF = m_RhoF->multVxM( *m_Directions->lambdaVector() );
      m_VRhoHemB = m_RhoB->multVxM( *m_Directions->lambdaVector() );
      // Layer absorptances
      m_AbsF = make_shared< vector< double > >();
      m_AbsB = make_shared< vector< double > >();
      size_t size = m_VTauHemF->size();
      for( size_t i = 0; i < size; ++i ) {
        m_AbsF->push_back( 1 - ( *m_VTauHemF )[ i ] - ( *m_VRhoHemF )[ i ] );
        m_AbsB->push_back( 1 - ( *m_VTauHemB )[ i ] - ( *m_VRhoHemB )[ i ] );
      }
      m_HemisphericalCalculated = true;
    }
  }

}