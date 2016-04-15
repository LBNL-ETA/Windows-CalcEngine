#include <memory>
#include <gtest/gtest.h>

#include "WovenCell.hpp"
#include "WovenCellDescription.hpp"
#include "MaterialDescription.hpp"
#include "FenestrationCommon.hpp"
#include "BeamDirection.hpp"

using namespace std;
using namespace LayerOptics;
using namespace FenestrationCommon;

class TestWovenCell2 : public testing::Test
{

private:
  shared_ptr< CWovenCell > m_Cell;

protected:
  virtual void SetUp() {
    // create material
    double Tmat = 0.15;
    double Rfmat = 0.8;
    double Rbmat = 0.6;
    double minLambda = 0.3;
    double maxLambda = 2.5;
    shared_ptr< CMaterialSingleBand > aMaterial = 
      make_shared< CMaterialSingleBand >( Tmat, Tmat, Rfmat, Rbmat, minLambda, maxLambda );

    // make cell geometry
    double diameter = 6.35; // mm
    double spacing = 19.05; // mm
    shared_ptr< CWovenCellDescription > aCell = 
      make_shared< CWovenCellDescription >( diameter, spacing );

    m_Cell = make_shared< CWovenCell >( aMaterial, aCell );
  }

public:
  shared_ptr< CWovenCell > GetCell() { return m_Cell; };

};

TEST_F( TestWovenCell2, TestWoven1 ) {
  SCOPED_TRACE( "Begin Test: Woven cell (Theta = 0, Phi = 0)." );
  
  shared_ptr< CWovenCell > aCell = GetCell();

  double Theta = 0; // deg
  double Phi = 0; // deg
  Side aFrontSide = Side::Front;
  Side aBackSide = Side::Back;

  shared_ptr< CBeamDirection > aDirection = make_shared< CBeamDirection >( Theta, Phi );
  
  double Tdir_dir = aCell->T_dir_dir( aFrontSide, aDirection );
  EXPECT_NEAR( 0.444444444, Tdir_dir, 1e-6 );

  Tdir_dir = aCell->T_dir_dir( aBackSide, aDirection );
  EXPECT_NEAR( 0.444444444, Tdir_dir, 1e-6 );
  
  double Tdir_dif = aCell->T_dir_dif( aFrontSide, aDirection );
  EXPECT_NEAR( 0.092147851, Tdir_dif, 1e-6 );

  Tdir_dif = aCell->T_dir_dif( aBackSide, aDirection );
  EXPECT_NEAR( 0.099688235, Tdir_dif, 1e-6 );

  double Rdir_dif = aCell->R_dir_dif( aFrontSide, aDirection );
  EXPECT_NEAR( 0.435629927, Rdir_dif, 1e-6 );

  Rdir_dif = aCell->R_dir_dif( aBackSide, aDirection );
  EXPECT_NEAR( 0.316978432, Rdir_dif, 1e-6 );

}

TEST_F( TestWovenCell2, TestWoven2 ) {
  SCOPED_TRACE( "Begin Test: Woven cell (Theta = 45, Phi = 0)." );
  
  shared_ptr< CWovenCell > aCell = GetCell();

  double Theta = 45; // deg
  double Phi = 0; // deg
  Side aFrontSide = Side::Front;
  Side aBackSide = Side::Back;

  shared_ptr< CBeamDirection > aDirection = make_shared< CBeamDirection >( Theta, Phi );
  
  double Tdir_dir = aCell->T_dir_dir( aFrontSide, aDirection );
  EXPECT_NEAR( 0.352396986, Tdir_dir, 1e-6 );

  Tdir_dir = aCell->T_dir_dir( aBackSide, aDirection );
  EXPECT_NEAR( 0.352396986, Tdir_dir, 1e-6 );
  
  double Tdir_dif = aCell->T_dir_dif( aFrontSide, aDirection );
  EXPECT_NEAR( 0.106935928, Tdir_dif, 1e-6 );

  Tdir_dif = aCell->T_dir_dif( aBackSide, aDirection );
  EXPECT_NEAR( 0.114820875, Tdir_dif, 1e-6 );

  double Rdir_dif = aCell->R_dir_dif( aFrontSide, aDirection );
  EXPECT_NEAR( 0.508286936, Rdir_dif, 1e-6 );

  Rdir_dif = aCell->R_dir_dif( aBackSide, aDirection );
  EXPECT_NEAR( 0.370881385, Rdir_dif, 1e-6 );

}

TEST_F( TestWovenCell2, TestWoven3 ) {
  SCOPED_TRACE( "Begin Test: Woven cell (Theta = 78, Phi = 45)." );
  
  shared_ptr< CWovenCell > aCell = GetCell();

  double Theta = 78; // deg
  double Phi = 45; // deg
  Side aFrontSide = Side::Front;
  Side aBackSide = Side::Back;

  shared_ptr< CBeamDirection > aDirection = make_shared< CBeamDirection >( Theta, Phi );
  
  double Tdir_dir = aCell->T_dir_dir( aFrontSide, aDirection );
  EXPECT_NEAR( 0.0, Tdir_dir, 1e-6 );

  Tdir_dir = aCell->T_dir_dir( aBackSide, aDirection );
  EXPECT_NEAR( 0.0, Tdir_dir, 1e-6 );
  
  double Tdir_dif = aCell->T_dir_dif( aFrontSide, aDirection );
  EXPECT_NEAR( 0.216418818, Tdir_dif, 1e-6 );

  Tdir_dif = aCell->T_dir_dif( aBackSide, aDirection );
  EXPECT_NEAR( 0.240727922, Tdir_dif, 1e-6 );

  double Rdir_dif = aCell->R_dir_dif( aFrontSide, aDirection );
  EXPECT_NEAR( 0.733581182, Rdir_dif, 1e-6 );

  Rdir_dif = aCell->R_dir_dif( aBackSide, aDirection );
  EXPECT_NEAR( 0.509272078, Rdir_dif, 1e-6 );

}