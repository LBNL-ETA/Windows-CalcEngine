#include <memory>
#include <gtest/gtest.h>

#include "UniformDiffuseBSDFLayer.hpp"
#include "CellDescription.hpp"
#include "PerfectDiffuseCellDescription.hpp"
#include "UniformDiffuseCell.hpp"
#include "MaterialDescription.hpp"
#include "BSDFDirections.hpp"
#include "SquareMatrix.hpp"
#include "BSDFResults.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestPerfectDiffuseShade1 : public testing::Test {

private:
  shared_ptr< CBSDFLayer > m_Shade;

protected:
  virtual void SetUp()
  {
    // create material
    double Tmat = 0.00;
    double Rfmat = 0.55;
    double Rbmat = 0.55;
    double minLambda = 0.3;
    double maxLambda = 2.5;
    shared_ptr< CMaterialBand > aMaterial = 
      make_shared< CMaterialSingleBand >( Tmat, Tmat, Rfmat, Rbmat, minLambda, maxLambda );

    // make cell
    shared_ptr< CCellDescription > aCellDescription = 
      make_shared< CPerfectDiffuseCellDescription >();

    shared_ptr< CBSDFHemisphere > aBSDF = make_shared< CBSDFHemisphere >( BSDFBasis::Quarter );

    shared_ptr< CUniformDiffuseCell > aCell = make_shared< CUniformDiffuseCell >( aMaterial, aCellDescription );
    
    m_Shade = make_shared< CUniformDiffuseBSDFLayer >( aCell, aBSDF );

  }

public:
  shared_ptr< CBSDFLayer > GetShade() { return m_Shade; };

};

TEST_F( TestPerfectDiffuseShade1, TestSolarProperties ) {
  SCOPED_TRACE( "Begin Test: Perfect diffuse shade - Solar properties." );
  
  shared_ptr< CBSDFLayer > aShade = GetShade();

  shared_ptr< CBSDFResults > aResults = aShade->getResults();

  double tauDiff = aResults->TauDiffDiff( Side::Front );
  EXPECT_NEAR( 0.000000000, tauDiff, 1e-6 );

  double RfDiff = aResults->RhoDiffDiff( Side::Front );
  EXPECT_NEAR( 0.550000000, RfDiff, 1e-6 );

  shared_ptr< CSquareMatrix > aT = aResults->Tau( Side::Front );

  // Test only diagonal of transmittance matrix
  size_t size = aT->getSize();

  vector< double > correctResults;
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );

  vector< double > calculatedResults;
  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( (*aT)[i][i] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

  shared_ptr< CSquareMatrix > aRf = aResults->Rho( Side::Front );

  correctResults.clear();
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );
  correctResults.push_back( 0.175070 );

  calculatedResults.clear();
  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( (*aRf)[i][i] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

}