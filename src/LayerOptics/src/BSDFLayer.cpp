#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>

#include "BSDFLayer.hpp"
#include "BaseCell.hpp"
#include "BSDFDirections.hpp"
#include "BSDFResults.hpp"
#include "MaterialDescription.hpp"
#include "SquareMatrix.hpp"
#include "BSDFPatch.hpp"
#include "FenestrationCommon.hpp"
#include "BeamDirection.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace LayerOptics {

  CBSDFLayer::CBSDFLayer( shared_ptr< CBaseCell > t_Cell, 
    shared_ptr< const CBSDFHemisphere > t_Hemisphere ) : 
    m_Calculated( false ), m_CalculatedWV( false ) {
    m_Cell = t_Cell;
    m_BSDFHemisphere = t_Hemisphere;

    // TODO: Maybe to refactor results to incoming and outgoing if not affecting speed.
    // This is not necessary before axisymmetry is introduced
    m_Results = make_shared< CBSDFResults >( m_BSDFHemisphere->getDirections( BSDFHemisphere::Incoming ) );
    m_WVResults = make_shared< vector< shared_ptr< CBSDFResults > > >();
    size_t size = t_Cell->getBandSize();
    for( size_t i = 0; i < size; ++i ) {
      shared_ptr< CBSDFResults > aResults = 
        make_shared< CBSDFResults >( m_BSDFHemisphere->getDirections( BSDFHemisphere::Incoming ) );
      m_WVResults->push_back( aResults );
    }
  }

  shared_ptr< CBSDFResults > CBSDFLayer::getResults() {
    if( !m_Calculated ) {
      calculate();
      m_Calculated = true;
    }
    return m_Results;
  }

  shared_ptr< vector< shared_ptr< CBSDFResults > > > CBSDFLayer::getWavelengthResults() {
    if( !m_CalculatedWV ) {
      calculate_wv();
      m_CalculatedWV = true;
    }
    return m_WVResults;
  }

  int CBSDFLayer::getBandIndex( const double t_Wavelength ) {
    return m_Cell->getBandIndex( t_Wavelength ) ;
  }

  void CBSDFLayer::calc_dir_dir() {
    for( Side t_Side : Enum< Side >() ) {
      shared_ptr< CBSDFDirections > aDirections = m_BSDFHemisphere->getDirections( BSDFHemisphere::Incoming );
      size_t size = aDirections->size();
      shared_ptr< CSquareMatrix > Tau = make_shared< CSquareMatrix >( size );
      shared_ptr< CSquareMatrix > Rho = make_shared< CSquareMatrix >( size );
      for( size_t i = 0; i < size; ++i ) {
        shared_ptr< const CBeamDirection > aDirection = ( *aDirections )[ i ]->centerPoint();
        double Lambda = ( *aDirections )[ i ]->lambda();
        
        double aTau = m_Cell->T_dir_dir( t_Side, aDirection );
        double aRho = m_Cell->R_dir_dir( t_Side, aDirection );

        aTau = aTau / Lambda;
        ( *Tau )[ i ][ i ] += aTau;
        ( *Rho )[ i ][ i ] += aRho / Lambda;
      }
      m_Results->setResultMatrices( Tau, Rho, t_Side );
    }
  }

  void CBSDFLayer::calc_dir_dir_wv() {
    for( Side aSide : Enum< Side >() ) {
      shared_ptr< CBSDFDirections > aDirections = m_BSDFHemisphere->getDirections( BSDFHemisphere::Incoming );
      size_t size = aDirections->size();
      for( size_t i = 0; i < size; ++i ) {
        shared_ptr< const CBeamDirection > aDirection = ( *aDirections )[ i ]->centerPoint();
        shared_ptr< vector< double > > aTau = m_Cell->T_dir_dir_band( aSide, aDirection );
        shared_ptr< vector< double > > aRho = m_Cell->R_dir_dir_band( aSide, aDirection );
        double Lambda = ( *aDirections )[ i ]->lambda();
        shared_ptr< CSquareMatrix > Tau = nullptr;
        shared_ptr< CSquareMatrix > Rho = nullptr;
        size_t numWV = aTau->size();
        for( size_t j = 0; j < numWV; ++j ) {
          shared_ptr< CBSDFResults > aResults = ( *m_WVResults )[ j ];
          assert( aResults != nullptr );
          Tau = aResults->Tau( aSide );
          Rho = aResults->Rho( aSide );
          ( *Tau )[ i ][ i ] += ( *aTau )[ j ] / Lambda;
          ( *Rho )[ i ][ i ] += ( *aRho )[ j ] / Lambda;
        }
      }
    }
  }

  void CBSDFLayer::calc_dir_dif() {
    for( Side aSide : Enum< Side >() ) {

      shared_ptr< CBSDFDirections > aDirections = m_BSDFHemisphere->getDirections( BSDFHemisphere::Incoming );

      size_t size = aDirections->size();
      for( size_t i = 0; i < size; ++i ) {

        shared_ptr< const CBeamDirection > aDirection = ( *aDirections )[ i ]->centerPoint();
        
        calcDiffuseDistribution( aSide, aDirection, i );
      }
    }
  }

  void CBSDFLayer::calc_dir_dif_wv() {
    for( Side aSide : Enum< Side >() ) {

      shared_ptr< CBSDFDirections > aDirections = m_BSDFHemisphere->getDirections( BSDFHemisphere::Incoming );

      size_t size = aDirections->size();
      for( size_t i = 0; i < size; ++i ) {
        shared_ptr< const CBeamDirection > aDirection = ( *aDirections )[ i ]->centerPoint();
        calcDiffuseDistribution_wv( aSide, aDirection, i );
      }
    }
  }

  void CBSDFLayer::calculate() {
    calc_dir_dir();
    calc_dir_dif();
  }

  void CBSDFLayer::calculate_wv() {
    calc_dir_dir_wv();
    calc_dir_dif_wv();
  }

}