#include <memory>
#include <gtest/gtest.h>

#include "UniformDiffuseBSDFLayer.hpp"
#include "PerforatedCell.hpp"
#include "PerforatedCellDescription.hpp"
#include "MaterialDescription.hpp"
#include "BSDFDirections.hpp"
#include "SquareMatrix.hpp"
#include "BSDFResults.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace LayerOptics;
using namespace FenestrationCommon;

class TestRectangularPerforatedShade1 : public testing::Test {

private:
  shared_ptr< CBSDFLayer > m_Shade;

protected:
  virtual void SetUp()
  {
    // create material
    double Tmat = 0.0;
    double Rfmat = 0.7;
    double Rbmat = 0.7;
    double minLambda = 0.3;
    double maxLambda = 2.5;
    shared_ptr< CMaterial > aMaterial = 
      make_shared< CMaterialSingleBand >( Tmat, Tmat, Rfmat, Rbmat, minLambda, maxLambda );

    // make cell geometry
    double x = 19.05; // mm
    double y = 19.05; // mm
    double thickness = 0.6; // mm
    double xHole = 3.175; // mm
    double yHole = 6.35; // mm
    shared_ptr< CCellDescription > aCellDescription = 
      make_shared< CRectangularCellDescription >( x, y, thickness, xHole, yHole );

    shared_ptr< CBSDFHemisphere > aBSDF = make_shared< CBSDFHemisphere >( BSDFBasis::Quarter );

    shared_ptr< CUniformDiffuseCell > aCell = make_shared< CPerforatedCell >( aMaterial, aCellDescription );
    
    m_Shade = make_shared< CUniformDiffuseBSDFLayer >( aCell, aBSDF );

  }

public:
  shared_ptr< CBSDFLayer > GetShade() { return m_Shade; };

};

TEST_F( TestRectangularPerforatedShade1, TestSolarProperties ) {
  SCOPED_TRACE( "Begin Test: Rectangular perforated cell - Solar properties." );
  
  shared_ptr< CBSDFLayer > aShade = GetShade();

  shared_ptr< CBSDFResults > aResults = aShade->getResults();

  double tauDiff = aResults->TauDiff( Side::Front );
  EXPECT_NEAR( 0.041876313, tauDiff, 1e-6 );

  double RfDiff = aResults->RhoDiff( Side::Front );
  EXPECT_NEAR( 0.670686365, RfDiff, 1e-6 );

  shared_ptr< CSquareMatrix > aT = aResults->Tau( Side::Front );

  // Test only diagonal of transmittance matrix
  size_t size = aT->getSize();

  vector< double > correctResults;
  correctResults.push_back( 0.722625 );
  correctResults.push_back( 0.731048 );
  correctResults.push_back( 0.728881 );
  correctResults.push_back( 0.754961 );
  correctResults.push_back( 0.728881 );
  correctResults.push_back( 0.731048 );
  correctResults.push_back( 0.728881 );
  correctResults.push_back( 0.754961 );
  correctResults.push_back( 0.728881 );
  correctResults.push_back( 0.622917 );
  correctResults.push_back( 0.614362 );
  correctResults.push_back( 0.632505 );
  correctResults.push_back( 0.672486 );
  correctResults.push_back( 0.632505 );
  correctResults.push_back( 0.614362 );
  correctResults.push_back( 0.622917 );
  correctResults.push_back( 0.614362 );
  correctResults.push_back( 0.632505 );
  correctResults.push_back( 0.672486 );
  correctResults.push_back( 0.632505 );
  correctResults.push_back( 0.614362 );
  correctResults.push_back( 0.534246 );
  correctResults.push_back( 0.523031 );
  correctResults.push_back( 0.557403 );
  correctResults.push_back( 0.628150 );
  correctResults.push_back( 0.557403 );
  correctResults.push_back( 0.523031 );
  correctResults.push_back( 0.534246 );
  correctResults.push_back( 0.523031 );
  correctResults.push_back( 0.557403 );
  correctResults.push_back( 0.628150 );
  correctResults.push_back( 0.557403 );
  correctResults.push_back( 0.523031 );
  correctResults.push_back( 0.146104 );
  correctResults.push_back( 0.219651 );
  correctResults.push_back( 0.416249 );
  correctResults.push_back( 0.219651 );
  correctResults.push_back( 0.146104 );
  correctResults.push_back( 0.219651 );
  correctResults.push_back( 0.416249 );
  correctResults.push_back( 0.219651 );

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
  correctResults.push_back( 0.210438 );
  correctResults.push_back( 0.211198 );
  correctResults.push_back( 0.211233 );
  correctResults.push_back( 0.210818 );
  correctResults.push_back( 0.211233 );
  correctResults.push_back( 0.211198 );
  correctResults.push_back( 0.211233 );
  correctResults.push_back( 0.210818 );
  correctResults.push_back( 0.211233 );
  correctResults.push_back( 0.212138 );
  correctResults.push_back( 0.212284 );
  correctResults.push_back( 0.211973 );
  correctResults.push_back( 0.211288 );
  correctResults.push_back( 0.211973 );
  correctResults.push_back( 0.212284 );
  correctResults.push_back( 0.212138 );
  correctResults.push_back( 0.212284 );
  correctResults.push_back( 0.211973 );
  correctResults.push_back( 0.211288 );
  correctResults.push_back( 0.211973 );
  correctResults.push_back( 0.212284 );
  correctResults.push_back( 0.213658 );
  correctResults.push_back( 0.213850 );
  correctResults.push_back( 0.213261 );
  correctResults.push_back( 0.212048 );
  correctResults.push_back( 0.213261 );
  correctResults.push_back( 0.213850 );
  correctResults.push_back( 0.213658 );
  correctResults.push_back( 0.213850 );
  correctResults.push_back( 0.213261 );
  correctResults.push_back( 0.212048 );
  correctResults.push_back( 0.213261 );
  correctResults.push_back( 0.213850 );
  correctResults.push_back( 0.220182 );
  correctResults.push_back( 0.218856 );
  correctResults.push_back( 0.215310 );
  correctResults.push_back( 0.218856 );
  correctResults.push_back( 0.220182 );
  correctResults.push_back( 0.218856 );
  correctResults.push_back( 0.215310 );
  correctResults.push_back( 0.218856 );

  calculatedResults.clear();
  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( (*aRf)[i][i] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

}