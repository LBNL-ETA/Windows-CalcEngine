#include <memory>
#include <gtest/gtest.h>

#include "VenetianCell.hpp"
#include "VenetianCellDescription.hpp"
#include "MaterialDescription.hpp"
#include "FenestrationCommon.hpp"
#include "BeamDirection.hpp"
#include "SquareMatrix.hpp"

using namespace std;
using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestVenetianCellFlat45_1 : public testing::Test {

private:
  shared_ptr< CVenetianCell > m_Cell;

protected:
  virtual void SetUp() {
    // create material
    double Tmat = 0.1;
    double Rfmat = 0.7;
    double Rbmat = 0.7;
    double minLambda = 0.3;
    double maxLambda = 2.5;
    shared_ptr< CMaterialSingleBand > aMaterial = 
      make_shared< CMaterialSingleBand >( Tmat, Tmat, Rfmat, Rbmat, minLambda, maxLambda );
    
    
    // make cell geometry
    double slatWidth = 0.010; // m
    double slatSpacing = 0.010; // m
    double slatTiltAngle = 45;
    double curvatureRadius = 0;
    size_t numOfSlatSegments = 2;

    shared_ptr< CVenetianCellDescription > aCellDescription = 
      make_shared< CVenetianCellDescription >( slatWidth, slatSpacing, slatTiltAngle, 
      curvatureRadius, numOfSlatSegments );

    m_Cell = make_shared< CVenetianCell >( aMaterial, aCellDescription );

  }

public:
  shared_ptr< CVenetianCell > GetCell() { return m_Cell; };

};

TEST_F( TestVenetianCellFlat45_1, TestVenetian1 ) {
  SCOPED_TRACE( "Begin Test: Venetian cell (Flat, 45 degrees slats) - diffuse-diffuse." );
  
  shared_ptr< CVenetianCell > aCell = GetCell();

  // Front side
  Side aSide = Side::Front;
  double Tdif_dif = aCell->T_dif_dif( aSide );
  double Rdif_dif = aCell->R_dif_dif( aSide );

  EXPECT_NEAR( 0.47122586752693946, Tdif_dif, 1e-6 );
  EXPECT_NEAR( 0.34565694288233745, Rdif_dif, 1e-6 );

  // Back side
  aSide = Side::Back;
  Tdif_dif = aCell->T_dif_dif( aSide );
  Rdif_dif = aCell->R_dif_dif( aSide );

  EXPECT_NEAR( 0.47122586752693946, Tdif_dif, 1e-6 );
  EXPECT_NEAR( 0.34565694288233745, Rdif_dif, 1e-6 );

}

TEST_F( TestVenetianCellFlat45_1, TestVenetian2 ) {
  SCOPED_TRACE( "Begin Test: Venetian cell (Flat, 45 degrees slats) - direct-diffuse." );
  
  shared_ptr< CVenetianCell > aCell = GetCell();

  // Front side
  Side aSide = Side::Front;
  double Theta = 0;
  double Phi = 0;
  CBeamDirection aDirection = CBeamDirection( Theta, Phi );

  double Tdir_dir = aCell->T_dir_dir( aSide, aDirection );
  double Tdir_dif = aCell->T_dir_dif( aSide, aDirection );
  double Rdir_dif = aCell->R_dir_dif( aSide, aDirection );

  EXPECT_NEAR( 0.29289321881345237, Tdir_dir, 1e-6 );
  EXPECT_NEAR( 0.15853813605369510, Tdir_dif, 1e-6 );
  EXPECT_NEAR( 0.35939548999199644, Rdir_dif, 1e-6 );

  // Back side
  aSide = Side::Back;
  Tdir_dir = aCell->T_dir_dir( aSide, aDirection );
  Tdir_dif = aCell->T_dir_dif( aSide, aDirection );
  Rdir_dif = aCell->R_dir_dif( aSide, aDirection );

  EXPECT_NEAR( 0.29289321881345237, Tdir_dir, 1e-6 );
  EXPECT_NEAR( 0.15853813605369516, Tdir_dif, 1e-6 );
  EXPECT_NEAR( 0.35939548999199655, Rdir_dif, 1e-6 );

}

TEST_F( TestVenetianCellFlat45_1, TestVenetian3 ) {
  SCOPED_TRACE( "Begin Test: Venetian cell (Flat, 45 degrees slats) - direct-diffuse." );
  
  shared_ptr< CVenetianCell > aCell = GetCell();

  // Front side
  Side aSide = Side::Front;
  double Theta = 18;
  double Phi = 180;
  CBeamDirection aDirection = CBeamDirection( Theta, Phi );

  double Tdir_dir = aCell->T_dir_dir( aSide, aDirection );
  double Tdir_dif = aCell->T_dir_dif( aSide, aDirection );
  double Rdir_dif = aCell->R_dir_dif( aSide, aDirection );

  EXPECT_NEAR( 0.29289321881345237, Tdir_dir, 1e-6 );
  EXPECT_NEAR( 0.15853813605369510, Tdir_dif, 1e-6 );
  EXPECT_NEAR( 0.35939548999199644, Rdir_dif, 1e-6 );

  // Back side
  aSide = Side::Back;
  Tdir_dir = aCell->T_dir_dir( aSide, aDirection );
  Tdir_dif = aCell->T_dir_dif( aSide, aDirection );
  Rdir_dif = aCell->R_dir_dif( aSide, aDirection );

  EXPECT_NEAR( 0.29289321881345237, Tdir_dir, 1e-6 );
  EXPECT_NEAR( 0.15853813605369516, Tdir_dif, 1e-6 );
  EXPECT_NEAR( 0.35939548999199655, Rdir_dif, 1e-6 );

}