#include <memory>
#include <gtest/gtest.h>

#include "VenetianCellDescription.hpp"
#include "MaterialDescription.hpp"
#include "FenestrationCommon.hpp"
#include "BSDFDirections.hpp"
#include "BSDFResults.hpp"
#include "SquareMatrix.hpp"
#include "BSDFLayer.hpp"
#include "BSDFLayerMaker.hpp"

using namespace std;
using namespace SingleLayerOptics;
using namespace FenestrationCommon;

class TestVenetianDirectionalShadeFlat45_5 : public testing::Test {

private:
  shared_ptr< CBSDFLayer > m_Shade;

protected:
  virtual void SetUp() {
    // create material
    double Tmat = 0.2;
    double Rfmat = 0.6;
    double Rbmat = 0.6;
    double minLambda = 0.3;
    double maxLambda = 2.5;
    shared_ptr< CMaterialBand > aMaterial = 
      make_shared< CMaterialSingleBand >( Tmat, Tmat, Rfmat, Rbmat, minLambda, maxLambda );
    
    // make cell geometry
    double slatWidth = 0.016; // m
    double slatSpacing = 0.012; // m
    double slatTiltAngle = 45;
    double curvatureRadius = 0;
    size_t numOfSlatSegments = 5;

    shared_ptr< CCellDescription > aCellDescription = 
      make_shared< CVenetianCellDescription >( slatWidth, slatSpacing, slatTiltAngle, 
      curvatureRadius, numOfSlatSegments );

    // Method
    DistributionMethod aDistribution = DistributionMethod::DirectionalDiffuse;

    // create BSDF
    shared_ptr< CBSDFHemisphere > aBSDF = make_shared< CBSDFHemisphere >( BSDFBasis::Quarter );

    // make layer
    CBSDFLayerMaker aMaker = CBSDFLayerMaker( aMaterial, aBSDF, aCellDescription, aDistribution );
    m_Shade = aMaker.getLayer();

  }

public:
  shared_ptr< CBSDFLayer > GetShade() { return m_Shade; };

};

TEST_F( TestVenetianDirectionalShadeFlat45_5, TestVenetian1 ) {
  SCOPED_TRACE( "Begin Test: Venetian cell (Flat, 45 degrees slats) - single band properties." );
  
  shared_ptr< CBSDFLayer > aShade = GetShade();

  shared_ptr< CBSDFResults > aResults = aShade->getResults();

  double tauDiff = aResults->TauDiffDiff( Side::Front );
  EXPECT_NEAR( 0.38194085830991792, tauDiff, 1e-6 );

  double RfDiff = aResults->RhoDiffDiff( Side::Front );
  EXPECT_NEAR( 0.37327866349094058, RfDiff, 1e-6 );

  shared_ptr< CSquareMatrix > aT = aResults->Tau( Side::Front );

  // Test only diagonal of transmittance matrix
  size_t size = aT->getSize();

  vector< double > correctResults;
  correctResults.push_back( 0.836275880754398400 );
  correctResults.push_back( 0.894179918528413050 );
  correctResults.push_back( 3.893461019742295200 );
  correctResults.push_back( 5.138660438286230200 );
  correctResults.push_back( 3.893461019742295200 );
  correctResults.push_back( 0.894179918528413050 );
  correctResults.push_back( 0.088417695248090525 );
  correctResults.push_back( 0.088474910851309277 );
  correctResults.push_back( 0.088417695248090525 );
  correctResults.push_back( 0.835689344595229480 );
  correctResults.push_back( 5.232171511989904800 );
  correctResults.push_back( 8.466077293836708200 );
  correctResults.push_back( 9.653035509249107800 );
  correctResults.push_back( 8.466077293836708200 );
  correctResults.push_back( 5.232171511989904800 );
  correctResults.push_back( 0.835689344595229480 );
  correctResults.push_back( 0.083366975858566325 );
  correctResults.push_back( 0.061722153521553885 );
  correctResults.push_back( 0.058886242955618555 );
  correctResults.push_back( 0.061722153521553885 );
  correctResults.push_back( 0.083366975858566325 );
  correctResults.push_back( 0.835689344595230370 );
  correctResults.push_back( 9.185161324998347300 );
  correctResults.push_back( 10.64790998036374100 );
  correctResults.push_back( 8.398011303285587200 );
  correctResults.push_back( 10.64790998036374100 );
  correctResults.push_back( 9.185161324998347300 );
  correctResults.push_back( 0.835689344595230370 );
  correctResults.push_back( 0.059735559077405055 );
  correctResults.push_back( 0.027508020796803857 );
  correctResults.push_back( 0.024511810701834273 );
  correctResults.push_back( 0.027508020796803857 );
  correctResults.push_back( 0.059735559077405055 );
  correctResults.push_back( 0.798979733486533950 );
  correctResults.push_back( 0.093292300851503973 );
  correctResults.push_back( 0.079357844380076134 );
  correctResults.push_back( 0.093292300851503973 );
  correctResults.push_back( 0.798979733486533950 );
  correctResults.push_back( 0.017310537462612144 );
  correctResults.push_back( 0.018209736057913754 );
  correctResults.push_back( 0.017310537462612144 );

  vector< double > calculatedResults;
  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( ( *aT )[ i ][ i ] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[ i ], calculatedResults[ i ], 1e-6 );
  }

  // Front reflectance
  shared_ptr< CSquareMatrix > aRf = aResults->Rho( Side::Front );

  correctResults.clear();
  calculatedResults.clear();

  correctResults.push_back( 0.165903843794496520 );
  correctResults.push_back( 0.165903843794496520 );
  correctResults.push_back( 0.127956545925703070 );
  correctResults.push_back( 0.117843124975481240 );
  correctResults.push_back( 0.127956545925703070 );
  correctResults.push_back( 0.165903843794496520 );
  correctResults.push_back( 0.134906386909191520 );
  correctResults.push_back( 0.117926403568254980 );
  correctResults.push_back( 0.134906386909191520 );
  correctResults.push_back( 0.165903843794496520 );
  correctResults.push_back( 0.108225759077810250 );
  correctResults.push_back( 0.060355829056359361 );
  correctResults.push_back( 0.046206747713723524 );
  correctResults.push_back( 0.060355829056359361 );
  correctResults.push_back( 0.108225759077810250 );
  correctResults.push_back( 0.165903843794496520 );
  correctResults.push_back( 0.111117195342986560 );
  correctResults.push_back( 0.064266151443683694 );
  correctResults.push_back( 0.047276114717840656 );
  correctResults.push_back( 0.064266151443683694 );
  correctResults.push_back( 0.111117195342986560 );
  correctResults.push_back( 0.165903843794496520 );
  correctResults.push_back( 0.051783295867258577 );
  correctResults.push_back( 0.014108177373773591 );
  correctResults.push_back( 0.027952498113038276 );
  correctResults.push_back( 0.014108177373773591 );
  correctResults.push_back( 0.051783295867258577 );
  correctResults.push_back( 0.165903843794496520 );
  correctResults.push_back( 0.053960092644205690 );
  correctResults.push_back( 0.016014184991125564 );
  correctResults.push_back( 0.030913799813821443 );
  correctResults.push_back( 0.016014184991125564 );
  correctResults.push_back( 0.053960092644205690 );
  correctResults.push_back( 0.165903843794496520 );
  correctResults.push_back( 0.107196920174651770 );
  correctResults.push_back( 0.181881555366362770 );
  correctResults.push_back( 0.107196920174651770 );
  correctResults.push_back( 0.165903843794496520 );
  correctResults.push_back( 0.128611259539473550 );
  correctResults.push_back( 0.180142928161266060 );
  correctResults.push_back( 0.128611259539473550 );

  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( ( *aRf )[ i ][ i ] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

}