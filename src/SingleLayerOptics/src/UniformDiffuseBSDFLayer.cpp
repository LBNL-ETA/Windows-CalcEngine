#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>

#include "UniformDiffuseBSDFLayer.hpp"
#include "UniformDiffuseCell.hpp"
#include "BSDFIntegrator.hpp"
#include "SquareMatrix.hpp"
#include "BSDFDirections.hpp"
#include "BSDFPatch.hpp"
#include "FenestrationCommon.hpp"
#include "BeamDirection.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace SingleLayerOptics {

  CUniformDiffuseBSDFLayer::CUniformDiffuseBSDFLayer( const shared_ptr< CUniformDiffuseCell >& t_Cell, 
    const shared_ptr< const CBSDFHemisphere >& t_Hemisphere ) :
    CBSDFLayer( t_Cell, t_Hemisphere ) {
    
  }

  shared_ptr< CUniformDiffuseCell > CUniformDiffuseBSDFLayer::cellAsUniformDiffuse() const {
    shared_ptr< CUniformDiffuseCell > aCell = dynamic_pointer_cast< CUniformDiffuseCell >( m_Cell );
    assert( aCell != nullptr );
    return aCell;
  }

  void CUniformDiffuseBSDFLayer::calcDiffuseDistribution( const Side aSide, 
    const CBeamDirection& t_Direction,
    const size_t t_DirectionIndex ) {

    shared_ptr< CUniformDiffuseCell > aCell = cellAsUniformDiffuse();
    
    shared_ptr< CSquareMatrix > Tau = m_Results->getMatrix( aSide, PropertySimple::T );
    shared_ptr< CSquareMatrix > Rho = m_Results->getMatrix( aSide, PropertySimple::R );

    double aTau = aCell->T_dir_dif( aSide, t_Direction );
    double Ref = aCell->R_dir_dif( aSide, t_Direction );

    shared_ptr< const CBSDFDirections > aDirections = 
      m_BSDFHemisphere->getDirections( BSDFHemisphere::Incoming );
    size_t size = aDirections->size();

    for( size_t j = 0; j < size; ++j ) {
      ( *Tau )[ j ][ t_DirectionIndex ] += aTau / M_PI;
      ( *Rho )[ j ][ t_DirectionIndex ] += Ref / M_PI;
    }

  }

  void CUniformDiffuseBSDFLayer::calcDiffuseDistribution_wv( const Side aSide, 
    const CBeamDirection& t_Direction,
    const size_t t_DirectionIndex ) {

    shared_ptr< CUniformDiffuseCell > aCell = cellAsUniformDiffuse();

    shared_ptr< vector< double > > aTau = aCell->T_dir_dif_band( aSide, t_Direction );
    shared_ptr< vector< double > > Ref = aCell->R_dir_dif_band( aSide, t_Direction );

    shared_ptr< const CBSDFDirections > aDirections = 
      m_BSDFHemisphere->getDirections( BSDFHemisphere::Incoming );
    size_t size = aDirections->size();

    for( size_t i = 0; i < size; ++i ) {
      size_t numWV = aTau->size();
      for( size_t j = 0; j < numWV; ++j ) {
        shared_ptr< CBSDFIntegrator > aResults = nullptr;
        aResults = ( *m_WVResults )[ j ];
        assert( aResults != nullptr );
        shared_ptr< CSquareMatrix > Tau = aResults->getMatrix( aSide, PropertySimple::T );
        shared_ptr< CSquareMatrix > Rho = aResults->getMatrix( aSide, PropertySimple::R );
        ( *Tau )[ i ][ t_DirectionIndex ] += ( *aTau )[ j ] / M_PI;
        ( *Rho )[ i ][ t_DirectionIndex ] += ( *Ref )[ j ] / M_PI;
      }
    }

  }

}