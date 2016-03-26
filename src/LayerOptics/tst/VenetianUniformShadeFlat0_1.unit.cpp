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

// Creation of BSDF matrix for single layer venetian shade with given material and geometrical properties.
// Method for calculation is diffuse part of distribution is uniform.
class TestVenetianUniformShadeFlat0_1 : public testing::Test {

private:
  shared_ptr< CUniformDiffuseBSDFLayer > m_Shade;

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
    double slatTiltAngle = 0;
    double curvatureRadius = 0;
    size_t numOfSlatSegments = 1;

    shared_ptr< CVenetianCellDescription > aCellDescription = 
      make_shared< CVenetianCellDescription >( slatWidth, slatSpacing, slatTiltAngle, 
      curvatureRadius, numOfSlatSegments );

    shared_ptr< CVenetianCell > aCell = make_shared< CVenetianCell >( aMaterial, aCellDescription );

    shared_ptr< CBSDFHemisphere > aBSDF = make_shared< CBSDFHemisphere >( BSDFBasis::Quarter );

    m_Shade = make_shared< CUniformDiffuseBSDFLayer >( aCell, aBSDF );

  };

public:
  shared_ptr< CUniformDiffuseBSDFLayer > GetShade() { return m_Shade; };

};

TEST_F( TestVenetianUniformShadeFlat0_1, TestVenetian1 ) {
  SCOPED_TRACE( "Begin Test: Venetian cell (Flat, 45 degrees slats) - solar properties." );
  
  shared_ptr< CUniformDiffuseBSDFLayer > aShade = GetShade();

  shared_ptr< CBSDFResults > aResults = aShade->getResults();

  double tauDiff = aResults->TauDiff( Side::Front );
  EXPECT_NEAR( 0.65093991496438897, tauDiff, 1e-6 );

  double RfDiff = aResults->RhoDiff( Side::Front );
  EXPECT_NEAR( 0.188319, RfDiff, 1e-6 );

  shared_ptr< CSquareMatrix > aT = aResults->Tau( Side::Front );

  // Test only diagonal of transmittance matrix
  size_t size = aT->getSize();

  vector< double > correctResults;
  correctResults.push_back( 13.007243 );
  correctResults.push_back( 14.019711 );
  correctResults.push_back( 10.824270 );
  correctResults.push_back( 9.5006750 );
  correctResults.push_back( 10.824270 );
  correctResults.push_back( 14.019711 );
  correctResults.push_back( 10.824270 );
  correctResults.push_back( 9.5006750 );
  correctResults.push_back( 10.824270 );
  correctResults.push_back( 12.996987 );
  correctResults.push_back( 8.3160770 );
  correctResults.push_back( 4.8894140 );
  correctResults.push_back( 3.6351680 );
  correctResults.push_back( 4.8894140 );
  correctResults.push_back( 8.3160770 );
  correctResults.push_back( 12.996987 );
  correctResults.push_back( 8.3160770 );
  correctResults.push_back( 4.8894140 );
  correctResults.push_back( 3.6351680 );
  correctResults.push_back( 4.8894140 );
  correctResults.push_back( 8.3160770 );
  correctResults.push_back( 12.996987 );
  correctResults.push_back( 4.1293450 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 4.1293450 );
  correctResults.push_back( 12.996987 );
  correctResults.push_back( 4.1293450 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 4.1293450 );
  correctResults.push_back( 12.355109 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 12.355109 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 0.1115490 );

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

  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.025629 );
  correctResults.push_back( 0.036244 );
  correctResults.push_back( 0.025629 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.025629 );
  correctResults.push_back( 0.036244 );
  correctResults.push_back( 0.025629 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.040522 );
  correctResults.push_back( 0.070187 );
  correctResults.push_back( 0.081045 );
  correctResults.push_back( 0.070187 );
  correctResults.push_back( 0.040522 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.040522 );
  correctResults.push_back( 0.070187 );
  correctResults.push_back( 0.081045 );
  correctResults.push_back( 0.070187 );
  correctResults.push_back( 0.040522 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.076767 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.076767 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.076767 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.076767 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.111549 );

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

  correctResults.push_back( 13.007243 );
  correctResults.push_back( 14.019711 );
  correctResults.push_back( 10.824270 );
  correctResults.push_back( 9.5006750 );
  correctResults.push_back( 10.824270 );
  correctResults.push_back( 14.019711 );
  correctResults.push_back( 10.824270 );
  correctResults.push_back( 9.5006750 );
  correctResults.push_back( 10.824270 );
  correctResults.push_back( 12.996987 );
  correctResults.push_back( 8.3160770 );
  correctResults.push_back( 4.8894140 );
  correctResults.push_back( 3.6351680 );
  correctResults.push_back( 4.8894140 );
  correctResults.push_back( 8.3160770 );
  correctResults.push_back( 12.996987 );
  correctResults.push_back( 8.3160770 );
  correctResults.push_back( 4.8894140 );
  correctResults.push_back( 3.6351680 );
  correctResults.push_back( 4.8894140 );
  correctResults.push_back( 8.3160770 );
  correctResults.push_back( 12.996987 );
  correctResults.push_back( 4.1293450 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 4.1293450 );
  correctResults.push_back( 12.996987 );
  correctResults.push_back( 4.1293450 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 4.1293450 );
  correctResults.push_back( 12.355109 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 12.355109 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 0.1115490 );
  correctResults.push_back( 0.1115490 );

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

  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.025629 );
  correctResults.push_back( 0.036244 );
  correctResults.push_back( 0.025629 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.025629 );
  correctResults.push_back( 0.036244 );
  correctResults.push_back( 0.025629 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.040522 );
  correctResults.push_back( 0.070187 );
  correctResults.push_back( 0.081045 );
  correctResults.push_back( 0.070187 );
  correctResults.push_back( 0.040522 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.040522 );
  correctResults.push_back( 0.070187 );
  correctResults.push_back( 0.081045 );
  correctResults.push_back( 0.070187 );
  correctResults.push_back( 0.040522 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.076767 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.076767 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.076767 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.076767 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.000000 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.111549 );
  correctResults.push_back( 0.111549 );

  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( (*aRb)[i][i] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

}