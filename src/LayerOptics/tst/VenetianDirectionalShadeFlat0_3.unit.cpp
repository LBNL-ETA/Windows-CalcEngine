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

class TestVenetianDirectionalShadeFlat0_3 : public testing::Test {

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
    double slatWidth = 0.016; // m
    double slatSpacing = 0.010; // m
    double slatTiltAngle = 0;
    double curvatureRadius = 0;
    size_t numOfSlatSegments = 5;

    shared_ptr< CVenetianCellDescription > aCellDescription = 
      make_shared< CVenetianCellDescription >( slatWidth, slatSpacing, slatTiltAngle, 
      curvatureRadius, numOfSlatSegments );

    shared_ptr< CVenetianCell > aCell = make_shared< CVenetianCell >( aMaterial, aCellDescription );

    shared_ptr< CBSDFHemisphere > aBSDF = make_shared< CBSDFHemisphere >( BSDFBasis::Quarter );

    m_Shade = make_shared< CDirectionalDiffuseBSDFLayer >( aCell, aBSDF );

	};

public:
	shared_ptr< CDirectionalDiffuseBSDFLayer > GetShade() { return m_Shade; };

};

TEST_F( TestVenetianDirectionalShadeFlat0_3, TestVenetian1 ) {
	SCOPED_TRACE( "Begin Test: Venetian cell (Flat, 45 degrees slats) - solar properties." );
	
	shared_ptr< CDirectionalDiffuseBSDFLayer > aShade = GetShade();

  shared_ptr< CBSDFResults > aResults = aShade->getResults();

  double tauDiff = aResults->TauDiff( Side::Front );
  EXPECT_NEAR( 0.48775116654942097, tauDiff, 1e-6 );

  double RfDiff = aResults->RhoDiff( Side::Front );
  EXPECT_NEAR( 0.22509839868274970, RfDiff, 1e-6 );

  shared_ptr< CSquareMatrix > aT = aResults->Tau( Side::Front );

  // Test only diagonal of transmittance matrix
  size_t size = aT->getSize();

  vector< double > correctResults;
  correctResults.push_back( 13.00724272523622500 );
  correctResults.push_back( 14.01971111841941000 );
  correctResults.push_back( 8.879333707559864000 );
  correctResults.push_back( 6.757930491107369900 );
  correctResults.push_back( 8.879333707559864000 );
  correctResults.push_back( 14.01971111841941000 );
  correctResults.push_back( 8.879333707559864000 );
  correctResults.push_back( 6.757930491107371700 );
  correctResults.push_back( 8.879333707559864000 );
  correctResults.push_back( 12.99698697545699000 );
  correctResults.push_back( 5.476030728175789600 );
  correctResults.push_back( 0.098218010371277989 );
  correctResults.push_back( 0.095832434781168180 );
  correctResults.push_back( 0.098218010371277989 );
  correctResults.push_back( 5.476030728175789600 );
  correctResults.push_back( 12.99698697545699000 );
  correctResults.push_back( 5.476030728175789600 );
  correctResults.push_back( 0.098218010371277989 );
  correctResults.push_back( 0.095832434781168138 );
  correctResults.push_back( 0.098218010371277989 );
  correctResults.push_back( 5.476030728175789600 );
  correctResults.push_back( 12.99698697545700400 );
  correctResults.push_back( 0.094990521299407105 );
  correctResults.push_back( 0.062476409862337726 );
  correctResults.push_back( 0.052827014930858306 );
  correctResults.push_back( 0.062476409862337726 );
  correctResults.push_back( 0.094990521299407105 );
  correctResults.push_back( 12.99698697545700400 );
  correctResults.push_back( 0.094990521299407105 );
  correctResults.push_back( 0.062476409862337781 );
  correctResults.push_back( 0.052827014930858347 );
  correctResults.push_back( 0.062476409862337781 );
  correctResults.push_back( 0.094990521299407105 );
  correctResults.push_back( 12.35510909608257400 );
  correctResults.push_back( 0.034092954478813497 );
  correctResults.push_back( 0.026306763651200261 );
  correctResults.push_back( 0.034092954478813497 );
  correctResults.push_back( 12.35510909608257400 );
  correctResults.push_back( 0.034092954478813518 );
  correctResults.push_back( 0.026306763651200268 );
  correctResults.push_back( 0.034092954478813518 );

  vector< double > calculatedResults;
  for( int i = 0; i < size; ++i ) {
    calculatedResults.push_back( ( *aT )[ i ][ i ] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( int i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-6 );
  }

  // Front reflectance
  shared_ptr< CSquareMatrix > aRf = aResults->Rho( Side::Front );

  correctResults.clear();
  calculatedResults.clear();

  correctResults.push_back( 0.000000000000000000 );
  correctResults.push_back( 0.000000000000000000 );
  correctResults.push_back( 0.013333908639926589 );
  correctResults.push_back( 0.026667817279853189 );
  correctResults.push_back( 0.013333908639926589 );
  correctResults.push_back( 0.000000000000000000 );
  correctResults.push_back( 0.013333908639926589 );
  correctResults.push_back( 0.026667817279853193 );
  correctResults.push_back( 0.013333908639926589 );
  correctResults.push_back( 0.000000000000000000 );
  correctResults.push_back( 0.033334771599816487 );
  correctResults.push_back( 0.098232428791164006 );
  correctResults.push_back( 0.101397330494051680 );
  correctResults.push_back( 0.098232428791164006 );
  correctResults.push_back( 0.033334771599816487 );
  correctResults.push_back( 0.000000000000000000 );
  correctResults.push_back( 0.033334771599816487 );
  correctResults.push_back( 0.098232428791163950 );
  correctResults.push_back( 0.101397330494051680 );
  correctResults.push_back( 0.098232428791163950 );
  correctResults.push_back( 0.033334771599816487 );
  correctResults.push_back( 0.000000000000000000 );
  correctResults.push_back( 0.097542640608995437 );
  correctResults.push_back( 0.116785354563414950 );
  correctResults.push_back( 0.125238941198961200 );
  correctResults.push_back( 0.116785354563414950 );
  correctResults.push_back( 0.097542640608995437 );
  correctResults.push_back( 0.000000000000000000 );
  correctResults.push_back( 0.097542640608995465 );
  correctResults.push_back( 0.116785354563414910 );
  correctResults.push_back( 0.125238941198961260 );
  correctResults.push_back( 0.116785354563414910 );
  correctResults.push_back( 0.097542640608995465 );
  correctResults.push_back( 0.000000000000000000 );
  correctResults.push_back( 0.180352627561802150 );
  correctResults.push_back( 0.147864609583900520 );
  correctResults.push_back( 0.180352627561802150 );
  correctResults.push_back( 0.000000000000000000 );
  correctResults.push_back( 0.180352627561802200 );
  correctResults.push_back( 0.147864609583900520 );
  correctResults.push_back( 0.180352627561802200 );

  for( int i = 0; i < size; ++i ) {
    calculatedResults.push_back( ( *aRf )[ i ][ i ] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( int i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[ i ], calculatedResults[ i ], 1e-6 );
  }

}