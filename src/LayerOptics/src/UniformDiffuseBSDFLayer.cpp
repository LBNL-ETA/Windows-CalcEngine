#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>

#include "UniformDiffuseBSDFLayer.hpp"
#include "UniformDiffuseCell.hpp"
#include "BSDFResults.hpp"
#include "SquareMatrix.hpp"
#include "BSDFDirections.hpp"
#include "BSDFPatch.hpp"
#include "FenestrationCommon.hpp"
#include "BeamDirection.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace LayerOptics {

  CUniformDiffuseBSDFLayer::CUniformDiffuseBSDFLayer( shared_ptr< CUniformDiffuseCell > t_Cell, 
    shared_ptr< const CBSDFHemisphere > t_Hemisphere ) :
    CBSDFLayer( t_Cell, t_Hemisphere ) {
    
  };

  shared_ptr< CUniformDiffuseCell > CUniformDiffuseBSDFLayer::cellAsUniformDiffuse() const {
    shared_ptr< CUniformDiffuseCell > aCell = dynamic_pointer_cast< CUniformDiffuseCell >( m_Cell );
    assert( aCell != nullptr );
    return aCell;
  };

  void CUniformDiffuseBSDFLayer::calcDiffuseDistribution( const Side aSide, 
    shared_ptr< const CBeamDirection > t_Direction,
    const size_t t_DirectionIndex ) {

    shared_ptr< CUniformDiffuseCell > aCell = cellAsUniformDiffuse();
    
    shared_ptr< CSquareMatrix > Tau = m_Results->Tau( aSide );
    shared_ptr< CSquareMatrix > Rho = m_Results->Rho( aSide );

    double aTau = aCell->T_dir_dif( aSide, t_Direction );
    double Ref = aCell->R_dir_dif( aSide, t_Direction );

    shared_ptr< CBSDFDirections > aDirections = m_BSDFHemisphere->getDirections( BSDFHemisphere::Incoming );
    size_t size = aDirections->size();

    for( size_t j = 0; j < size; ++j ) {
      //( *Tau )[ t_DirectionIndex ][ j ] += aTau / M_PI;
      //( *Rho )[ t_DirectionIndex ][ j ] += Ref / M_PI;
      ( *Tau )[ j ][ t_DirectionIndex ] += aTau / M_PI;
      ( *Rho )[ j ][ t_DirectionIndex ] += Ref / M_PI;
    }

  };

  void CUniformDiffuseBSDFLayer::calcDiffuseDistribution_wv( const Side aSide, 
    shared_ptr< const CBeamDirection > t_Direction,
    const size_t t_DirectionIndex ) {

    shared_ptr< CUniformDiffuseCell > aCell = cellAsUniformDiffuse();

    shared_ptr< vector< double > > aTau = aCell->T_dir_dif_band( aSide, t_Direction );
    shared_ptr< vector< double > > Ref = aCell->R_dir_dif_band( aSide, t_Direction );

    shared_ptr< CBSDFDirections > aDirections = m_BSDFHemisphere->getDirections( BSDFHemisphere::Incoming );
    size_t size = aDirections->size();

    for( size_t i = 0; i < size; ++i ) {
      size_t numWV = aTau->size();
      for( size_t j = 0; j < numWV; ++j ) {
        shared_ptr< CBSDFResults > aResults = nullptr;
        aResults = ( *m_WVResults )[ j ];
        assert( aResults != nullptr );
        shared_ptr< CSquareMatrix > Tau = aResults->Tau( aSide );
        shared_ptr< CSquareMatrix > Rho = aResults->Rho( aSide );
        // ( *Tau )[ t_DirectionIndex ][ i ] += ( *aTau )[ j ] / M_PI;
        // ( *Rho )[ t_DirectionIndex ][ i ] += ( *Ref )[ j ] / M_PI;
        ( *Tau )[ i ][ t_DirectionIndex ] += ( *aTau )[ j ] / M_PI;
        ( *Rho )[ i ][ t_DirectionIndex ] += ( *Ref )[ j ] / M_PI;
      }
    }

  };

}