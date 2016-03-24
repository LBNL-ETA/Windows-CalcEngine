#include <memory>
#include <gtest\gtest.h>

#include "DirectionalDiffuseBSDFLayer.hpp"
#include "VenetianCell.hpp"
#include "VenetianCellDescription.hpp"
#include "MaterialDescription.hpp"
#include "FenestrationCommon.hpp"
#include "BSDFDirections.hpp"
#include "BSDFResults.hpp"
#include "SquareMatrix.hpp"

using namespace std;
using namespace LayerOptics;
using namespace FenestrationCommon;

class TestVenetianDirectionalShadeFlat0_2 : public testing::Test {

private:
  shared_ptr< CDirectionalDiffuseBSDFLayer > m_Shade;

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
    size_t numOfSlatSegments = 1;

    shared_ptr< CVenetianCellDescription > aCellDescription = 
      make_shared< CVenetianCellDescription >( slatWidth, slatSpacing, slatTiltAngle, 
      curvatureRadius, numOfSlatSegments );

    shared_ptr< CVenetianCell > aCell = make_shared< CVenetianCell >( aMaterial, aCellDescription );

    shared_ptr< CBSDFHemisphere > aBSDF = make_shared< CBSDFHemisphere >( BSDFBasis::Full );

    m_Shade = make_shared< CDirectionalDiffuseBSDFLayer >( aCell, aBSDF );

  };

public:
  shared_ptr< CDirectionalDiffuseBSDFLayer > GetShade() { return m_Shade; };

};

TEST_F( TestVenetianDirectionalShadeFlat0_2, TestVenetian1 ) {
  SCOPED_TRACE( "Begin Test: Venetian cell (Flat, 45 degrees slats) - solar properties." );
  
  shared_ptr< CDirectionalDiffuseBSDFLayer > aShade = GetShade();

  shared_ptr< CBSDFResults > aResults = aShade->getResults();

  double tauDiff = aResults->TauDiff( Side::Front );
  EXPECT_NEAR( 0.45408806110142574, tauDiff, 1e-6 );

  double RfDiff = aResults->RhoDiff( Side::Front );
  EXPECT_NEAR( 0.27657763790935469, RfDiff, 1e-6 );

}