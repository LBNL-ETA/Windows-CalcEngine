#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <assert.h>

#include "WovenCell.hpp"
#include "CellDescription.hpp"
#include "WovenCellDescription.hpp"
#include "MaterialDescription.hpp"
#include "MathFunctions.hpp"
#include "FenestrationCommon.hpp"
#include "BeamDirection.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace LayerOptics {

  ////////////////////////////////////////////////////////////////////////////////////////////
  //  CWovenBase
  ////////////////////////////////////////////////////////////////////////////////////////////
  CWovenBase::CWovenBase( shared_ptr< CMaterialBand > t_MaterialProperties, 
    shared_ptr< CCellDescription > t_Cell ) : 
    CBaseCell( t_MaterialProperties, t_Cell ), CUniformDiffuseCell( t_MaterialProperties, t_Cell ) {
    
  }

  shared_ptr< CWovenCellDescription > CWovenBase::getCellAsWoven() const {
    if( dynamic_pointer_cast< CWovenCellDescription >( m_CellDescription ) == NULL ) {
      assert("Incorrectly assigned cell description.");
    }

    shared_ptr< CWovenCellDescription > aCell = 
      dynamic_pointer_cast< CWovenCellDescription >( m_CellDescription );

    return aCell;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////
  //  CWovenCell
  ////////////////////////////////////////////////////////////////////////////////////////////
  CWovenCell::CWovenCell( shared_ptr< CMaterialBand > t_MaterialProperties, 
    shared_ptr< CCellDescription > t_Cell ) : CBaseCell( t_MaterialProperties, t_Cell ),
    CWovenBase( t_MaterialProperties, t_Cell ) {

  }

  double CWovenCell::T_dir_dir( const Side t_Side, shared_ptr< const CBeamDirection > t_Direction ) {
    return m_CellDescription->T_dir_dir( t_Side, t_Direction );
  }

  double CWovenCell::T_dir_dif( const Side t_Side, shared_ptr< const CBeamDirection > t_Direction ) {
    double T_material = CWovenBase::T_dir_dif( t_Side, t_Direction );
    double Tsct = Tscatter_single( t_Side, t_Direction );
    return T_material + Tsct;
  }

  double CWovenCell::R_dir_dif( const Side t_Side, shared_ptr< const CBeamDirection > t_Direction ) {
    double R_material = CWovenBase::R_dir_dif( t_Side, t_Direction );
    double Tsct = Tscatter_single( t_Side, t_Direction );
    return R_material - Tsct;
  }

  shared_ptr< vector< double > > CWovenCell::T_dir_dir_band( const Side t_Side, shared_ptr< const CBeamDirection > t_Direction ) {
    return CWovenBase::T_dir_dir_band( t_Side, t_Direction );
  }

  shared_ptr< vector< double > > CWovenCell::T_dir_dif_band( const Side t_Side, shared_ptr< const CBeamDirection > t_Direction ) {
    shared_ptr< vector< double > > T_material = CWovenBase::T_dir_dif_band( t_Side, t_Direction );
    shared_ptr< vector< double > > Tsct = Tscatter_range( t_Side, t_Direction );
    assert( Tsct->size() == T_material->size() );
    for( size_t i = 0; i < T_material->size(); ++i ) {
      ( *T_material )[ i ] = ( *T_material )[ i ] + ( *Tsct )[ i ];
    }
    return T_material;
  }

  shared_ptr< vector< double > > CWovenCell::R_dir_dif_band( const Side t_Side, shared_ptr< const CBeamDirection > t_Direction ) {
    shared_ptr< vector< double > > R_material = CWovenBase::R_dir_dif_band( t_Side, t_Direction );
    shared_ptr< vector< double > > Tsct = Tscatter_range( t_Side, t_Direction );
    assert( Tsct->size() == R_material->size() );
    for( size_t i = 0; i < R_material->size(); ++i ) {
      ( *R_material )[ i ] = ( *R_material )[ i ] - ( *Tsct )[ i ];
    }
    return R_material;
  }

  double CWovenCell::Tscatter_single( const Side t_Side, shared_ptr< const CBeamDirection > t_Direction ) {
    // Get matterial property from the opposite side of woven thread
    Side aScatterSide = oppositeSide( t_Side );
    double RScatter_mat = m_Material->getProperty( Property::R, aScatterSide );
    return Tscatter( t_Direction, RScatter_mat );
  }

  shared_ptr< vector< double > > CWovenCell::Tscatter_range( const Side t_Side, shared_ptr< const CBeamDirection > t_Direction ) {
    Side aScatterSide = oppositeSide( t_Side );
    shared_ptr< vector< double > > RScatter_mat = m_Material->getBandProperties( Property::R, aScatterSide );
    shared_ptr< vector< double > > aTsct = make_shared< vector< double > >();
    for( size_t i = 0; i < RScatter_mat->size(); ++i ) {
      double aTscatter = Tscatter( t_Direction, ( *RScatter_mat )[ i ] );
      aTsct->push_back( aTscatter );
    }
    return aTsct;
  }

  double CWovenCell::Tscatter( shared_ptr< const CBeamDirection > t_Direction, const double Rmat ) {
    double Tsct = 0;
    if( Rmat > 0 ) {
      double aAlt = degrees( t_Direction->Altitude() );
      double aAzm = degrees( t_Direction->Azimuth() );
      shared_ptr< CWovenCellDescription > aCell = getCellAsWoven();
      double gamma = aCell->gamma();

      if( gamma < 1 ) {
        double Tscattermax = 0.0229 * gamma + 0.2971 * Rmat - 0.03624 * pow( gamma, 2 ) + 
          0.04763 * pow( Rmat, 2 ) - 0.44416 * gamma * Rmat;
        double DeltaMax = 89.7 - 10 * gamma / 0.16;
        double Delta = pow( pow( aAlt, 2 ) + pow( aAzm, 2), 0.5 );
        double PeakRatio = 1 / ( 0.2 * Rmat * (1 - gamma) );

        double E = 0;
        if( Delta > DeltaMax ) {
          E = -( pow( fabs( Delta - DeltaMax ), 2 ) ) / 600;
          Tsct = -0.2 * Rmat * Tscattermax * ( 1 - gamma ) * max( 0.0, ( Delta - DeltaMax ) / ( 90 - DeltaMax ) );
        } else {
          E = -( pow( fabs( Delta - DeltaMax ), 2.5 ) ) / 600;
          Tsct = 0;
        }
        Tsct = Tsct + 0.2 * Rmat * Tscattermax * ( 1 - gamma ) * ( 1 + ( PeakRatio - 1 ) * exp( E ) );
      }

      if( Tsct < 0 ) {
        Tsct = 0;
      }
    }

    return Tsct;
  }

}