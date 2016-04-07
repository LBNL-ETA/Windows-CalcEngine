#include <memory>
#include <gtest/gtest.h>

#include "UniformDiffuseBSDFLayer.hpp"
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

class TestVenetianUniformShadeFlat45_1 : public testing::Test {

private:
  shared_ptr< CBSDFLayer > m_Shade;

protected:
  virtual void SetUp() {
    // create material
    double Tmat = 0.1;
    double Rfmat = 0.7;
    double Rbmat = 0.7;
    double minLambda = 0.3;
    double maxLambda = 2.5;
    shared_ptr< CMaterial > aMaterial = 
      make_shared< CMaterialSingleBand >( Tmat, Tmat, Rfmat, Rbmat, minLambda, maxLambda );
        
    // make cell geometry
    double slatWidth = 0.010; // m
    double slatSpacing = 0.010; // m
    double slatTiltAngle = 45;
    double curvatureRadius = 0;
    size_t numOfSlatSegments = 1;

    shared_ptr< CCellDescription > aCellDescription = 
      make_shared< CVenetianCellDescription >( slatWidth, slatSpacing, slatTiltAngle, 
      curvatureRadius, numOfSlatSegments );

    shared_ptr< CUniformDiffuseCell > aCell = make_shared< CVenetianCell >( aMaterial, aCellDescription );

    shared_ptr< CBSDFHemisphere > aBSDF = make_shared< CBSDFHemisphere >( BSDFBasis::Quarter );

    m_Shade = make_shared< CUniformDiffuseBSDFLayer >( aCell, aBSDF );

  };

public:
  shared_ptr< CBSDFLayer > GetShade() { return m_Shade; };

};

TEST_F( TestVenetianUniformShadeFlat45_1, TestVenetian1 ) {
  SCOPED_TRACE( "Begin Test: Venetian cell (Flat, 45 degrees slats) - solar properties." );
  
  shared_ptr< CBSDFLayer > aShade = GetShade();

  shared_ptr< CBSDFResults > aResults = aShade->getResults();

  double tauDiff = aResults->TauDiff( Side::Front );
  EXPECT_NEAR( 0.47624006362615717, tauDiff, 1e-6 );

  double RfDiff = aResults->RhoDiff( Side::Front );
  EXPECT_NEAR( 0.33488359240717491, RfDiff, 1e-6 );

  shared_ptr< CSquareMatrix > aT = aResults->Tau( Side::Front );

  // Test only diagonal of transmittance matrix
  size_t size = aT->getSize();

  vector< double > correctResults;
  correctResults.push_back( 3.861585 );
  correctResults.push_back( 4.158130 );
  correctResults.push_back( 6.423857 );
  correctResults.push_back( 7.362352 );
  correctResults.push_back( 6.423857 );
  correctResults.push_back( 4.158130 );
  correctResults.push_back( 1.892403 );
  correctResults.push_back( 0.953908 );
  correctResults.push_back( 1.892403 );
  correctResults.push_back( 3.858581 );
  correctResults.push_back( 7.178301 );
  correctResults.push_back( 9.608505 );
  correctResults.push_back( 10.49802 );
  correctResults.push_back( 9.608505 );
  correctResults.push_back( 7.178301 );
  correctResults.push_back( 3.858581 );
  correctResults.push_back( 0.538861 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.538861 );
  correctResults.push_back( 3.858581 );
  correctResults.push_back( 10.14755 );
  correctResults.push_back( 11.25443 );
  correctResults.push_back( 9.580691 );
  correctResults.push_back( 11.25443 );
  correctResults.push_back( 10.14755 );
  correctResults.push_back( 3.858581 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 3.670579 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 3.670579 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );

  vector< double > calculatedResults;
  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( (*aT)[i][i] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

  // Front reflectance
  shared_ptr< CSquareMatrix > aRf = aResults->Rho( Side::Front );

  correctResults.clear();
  calculatedResults.clear();

  correctResults.push_back( 0.113584 );
  correctResults.push_back( 0.113584 );
  correctResults.push_back( 0.087488 );
  correctResults.push_back( 0.076678 );
  correctResults.push_back( 0.087488 );
  correctResults.push_back( 0.113584 );
  correctResults.push_back( 0.139680 );
  correctResults.push_back( 0.150490 );
  correctResults.push_back( 0.139680 );
  correctResults.push_back( 0.113584 );
  correctResults.push_back( 0.072322 );
  correctResults.push_back( 0.042116 );
  correctResults.push_back( 0.031060 );
  correctResults.push_back( 0.042116 );
  correctResults.push_back( 0.072322 );
  correctResults.push_back( 0.113584 );
  correctResults.push_back( 0.154846 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.154846 );
  correctResults.push_back( 0.113584 );
  correctResults.push_back( 0.035417 );
  correctResults.push_back( 0.009955 );
  correctResults.push_back( 0.019516 );
  correctResults.push_back( 0.009955 );
  correctResults.push_back( 0.035417 );
  correctResults.push_back( 0.113584 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.113584 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.113584 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );

  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( (*aRf)[i][i] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

  // Back transmittance
  shared_ptr< CSquareMatrix > aTb = aResults->Tau( Side::Back );

  correctResults.clear();
  calculatedResults.clear();

  correctResults.push_back( 3.861585 );
  correctResults.push_back( 4.158130 );
  correctResults.push_back( 1.892403 );
  correctResults.push_back( 0.953908 );
  correctResults.push_back( 1.892403 );
  correctResults.push_back( 4.158130 );
  correctResults.push_back( 6.423857 );
  correctResults.push_back( 7.362352 );
  correctResults.push_back( 6.423857 );
  correctResults.push_back( 3.858581 );
  correctResults.push_back( 0.538861 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.538861 );
  correctResults.push_back( 3.858581 );
  correctResults.push_back( 7.178301 );
  correctResults.push_back( 9.608505 );
  correctResults.push_back( 10.49802 );
  correctResults.push_back( 9.608505 );
  correctResults.push_back( 7.178301 );
  correctResults.push_back( 3.858581 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 3.858581 );
  correctResults.push_back( 10.14755 );
  correctResults.push_back( 11.25443 );
  correctResults.push_back( 9.580691 );
  correctResults.push_back( 11.25443 );
  correctResults.push_back( 10.14755 );
  correctResults.push_back( 3.670579 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 3.670579 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );

  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( (*aTb)[i][i] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

  // Back reflectance
  shared_ptr< CSquareMatrix > aRb = aResults->Rho( Side::Back );

  correctResults.clear();
  calculatedResults.clear();

  correctResults.push_back( 0.113584 );
  correctResults.push_back( 0.113584 );
  correctResults.push_back( 0.139680 );
  correctResults.push_back( 0.150490 );
  correctResults.push_back( 0.139680 );
  correctResults.push_back( 0.113584 );
  correctResults.push_back( 0.087488 );
  correctResults.push_back( 0.076678 );
  correctResults.push_back( 0.087488 );
  correctResults.push_back( 0.113584 );
  correctResults.push_back( 0.154846 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.154846 );
  correctResults.push_back( 0.113584 );
  correctResults.push_back( 0.072322 );
  correctResults.push_back( 0.042116 );
  correctResults.push_back( 0.031060 );
  correctResults.push_back( 0.042116 );
  correctResults.push_back( 0.072322 );
  correctResults.push_back( 0.113584 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.113584 );
  correctResults.push_back( 0.035417 );
  correctResults.push_back( 0.009955 );
  correctResults.push_back( 0.019516 );
  correctResults.push_back( 0.009955 );
  correctResults.push_back( 0.035417 );
  correctResults.push_back( 0.113584 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.160632 );
  correctResults.push_back( 0.113584 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );
  correctResults.push_back( 0.073329 );

  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( (*aRb)[i][i] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

}