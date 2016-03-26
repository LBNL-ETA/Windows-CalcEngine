#include <memory>
#include <gtest/gtest.h>

#include "SpecularBSDFLayer.hpp"
#include "SpectralSample.hpp"
#include "SpectralProperties.hpp"
#include "SpecularCell.hpp"
#include "SpecularLayer.hpp"
#include "SpecularCellDescription.hpp"
#include "MeasuredSampleData.hpp"
#include "MaterialDescription.hpp"
#include "FenestrationCommon.hpp"
#include "BSDFDirections.hpp"
#include "BSDFResults.hpp"
#include "SquareMatrix.hpp"

using namespace std;
using namespace LayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;

// Test of calculating BSDF matrices over the entire wavelength range
class TestSpecularLayerMultiWavelength_102 : public testing::Test {

private:
  shared_ptr< CSpecularBSDFLayer > m_Layer;

protected:
  virtual void SetUp() {
    shared_ptr< CSpectralProperties >  aSolarRadiation = make_shared< CSpectralProperties >();
    
    // Full ASTM E891-87 Table 1
    aSolarRadiation->addProperty( 0.3000, 0.0  );
    aSolarRadiation->addProperty( 0.3050, 3.4  );
    aSolarRadiation->addProperty( 0.3100, 15.6 );
    aSolarRadiation->addProperty( 0.3150, 41.1 );
    aSolarRadiation->addProperty( 0.3200, 71.2 );

    shared_ptr< CSpectralSampleData > aMeasurements = make_shared< CSpectralSampleData >();

    aMeasurements->addRecord( 0.300, 0.0020, 0.0470, 0.0480 );
    aMeasurements->addRecord( 0.305, 0.0030, 0.0470, 0.0480 );
    aMeasurements->addRecord( 0.310, 0.0090, 0.0470, 0.0480 );
    aMeasurements->addRecord( 0.315, 0.0350, 0.0470, 0.0480 );
    aMeasurements->addRecord( 0.320, 0.1000, 0.0470, 0.0480 );

    shared_ptr< CSpectralSample > aSample = make_shared< CSpectralSample >( aMeasurements, aSolarRadiation );

    double thickness = 3.048e-3; // [m]
    SpecularMaterialType aType = SpecularMaterialType::Monolithic;
    // WavelengthRange aRange = WavelengthRange::Solar;
    double minLambda = 0.3;
    double maxLambda = 2.5;
    shared_ptr< CMaterialSample > aMaterial = 
      make_shared< CMaterialSample >( aSample, thickness, aType, minLambda, maxLambda );

    shared_ptr< CSpecularCellDescription > aCellDescription = make_shared< CSpecularCellDescription >();

    shared_ptr< CSpecularCell > aCell = make_shared< CSpecularCell >( aMaterial, aCellDescription );

    shared_ptr< CBSDFHemisphere > aBSDF = make_shared< CBSDFHemisphere >( BSDFBasis::Quarter );

    m_Layer = make_shared< CSpecularBSDFLayer >( aCell, aBSDF );

  };

public:
  shared_ptr< CSpecularBSDFLayer > getLayer() { return m_Layer; };

};

TEST_F( TestSpecularLayerMultiWavelength_102, TestSpecular1 ) {
  SCOPED_TRACE( "Begin Test: Specular layer - BSDF." );
  
  shared_ptr< CSpecularBSDFLayer > aLayer = getLayer();

  shared_ptr< vector< shared_ptr< CBSDFResults > > > aResults = aLayer->getWavelengthResults();
  
  size_t correctSize = 5;

  EXPECT_EQ( correctSize, aResults->size() );

  ///////////////////////////////////////////////////////////////////////
  //  Wavelength number 1
  ///////////////////////////////////////////////////////////////////////

  shared_ptr< CSquareMatrix > aT = ( *aResults )[ 0 ]->Tau( Side::Front );

  // Test only diagonal of transmittance matrix
  size_t size = aT->getSize();

  vector< double > correctResults;
  correctResults.push_back( 0.026014 );
  correctResults.push_back( 0.024743 );
  correctResults.push_back( 0.024743 );
  correctResults.push_back( 0.024743 );
  correctResults.push_back( 0.024743 );
  correctResults.push_back( 0.024743 );
  correctResults.push_back( 0.024743 );
  correctResults.push_back( 0.024743 );
  correctResults.push_back( 0.024743 );
  correctResults.push_back( 0.015794 );
  correctResults.push_back( 0.015794 );
  correctResults.push_back( 0.015794 );
  correctResults.push_back( 0.015794 );
  correctResults.push_back( 0.015794 );
  correctResults.push_back( 0.015794 );
  correctResults.push_back( 0.015794 );
  correctResults.push_back( 0.015794 );
  correctResults.push_back( 0.015794 );
  correctResults.push_back( 0.015794 );
  correctResults.push_back( 0.015794 );
  correctResults.push_back( 0.015794 );
  correctResults.push_back( 0.008638 );
  correctResults.push_back( 0.008638 );
  correctResults.push_back( 0.008638 );
  correctResults.push_back( 0.008638 );
  correctResults.push_back( 0.008638 );
  correctResults.push_back( 0.008638 );
  correctResults.push_back( 0.008638 );
  correctResults.push_back( 0.008638 );
  correctResults.push_back( 0.008638 );
  correctResults.push_back( 0.008638 );
  correctResults.push_back( 0.008638 );
  correctResults.push_back( 0.008638 );
  correctResults.push_back( 0.002528 );
  correctResults.push_back( 0.002528 );
  correctResults.push_back( 0.002528 );
  correctResults.push_back( 0.002528 );
  correctResults.push_back( 0.002528 );
  correctResults.push_back( 0.002528 );
  correctResults.push_back( 0.002528 );
  correctResults.push_back( 0.002528 );

  vector< double > calculatedResults;
  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( (*aT)[i][i] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

  // Front reflectance
  shared_ptr< CSquareMatrix > aRf = ( *aResults )[ 0 ]->Rho( Side::Front );

  correctResults.clear();
  calculatedResults.clear();

  correctResults.push_back( 0.61134  );
  correctResults.push_back( 0.661508 );
  correctResults.push_back( 0.661508 );
  correctResults.push_back( 0.661508 );
  correctResults.push_back( 0.661508 );
  correctResults.push_back( 0.661508 );
  correctResults.push_back( 0.661508 );
  correctResults.push_back( 0.661508 );
  correctResults.push_back( 0.661508 );
  correctResults.push_back( 0.659003 );
  correctResults.push_back( 0.659003 );
  correctResults.push_back( 0.659003 );
  correctResults.push_back( 0.659003 );
  correctResults.push_back( 0.659003 );
  correctResults.push_back( 0.659003 );
  correctResults.push_back( 0.659003 );
  correctResults.push_back( 0.659003 );
  correctResults.push_back( 0.659003 );
  correctResults.push_back( 0.659003 );
  correctResults.push_back( 0.659003 );
  correctResults.push_back( 0.659003 );
  correctResults.push_back( 0.974838 );
  correctResults.push_back( 0.974838 );
  correctResults.push_back( 0.974838 );
  correctResults.push_back( 0.974838 );
  correctResults.push_back( 0.974838 );
  correctResults.push_back( 0.974838 );
  correctResults.push_back( 0.974838 );
  correctResults.push_back( 0.974838 );
  correctResults.push_back( 0.974838 );
  correctResults.push_back( 0.974838 );
  correctResults.push_back( 0.974838 );
  correctResults.push_back( 0.974838 );
  correctResults.push_back( 3.654452 );
  correctResults.push_back( 3.654452 );
  correctResults.push_back( 3.654452 );
  correctResults.push_back( 3.654452 );
  correctResults.push_back( 3.654452 );
  correctResults.push_back( 3.654452 );
  correctResults.push_back( 3.654452 );
  correctResults.push_back( 3.654452 );

  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( (*aRf)[i][i] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

  ///////////////////////////////////////////////////////////////////////
  //  Wavelength number 2
  ///////////////////////////////////////////////////////////////////////

  aT = ( *aResults )[ 1 ]->Tau( Side::Front );

  // Test only diagonal of transmittance matrix
  size = aT->getSize();

  correctResults.clear();
  calculatedResults.clear();

  correctResults.push_back( 0.039022 );
  correctResults.push_back( 0.037422 );
  correctResults.push_back( 0.037422 );
  correctResults.push_back( 0.037422 );
  correctResults.push_back( 0.037422 );
  correctResults.push_back( 0.037422 );
  correctResults.push_back( 0.037422 );
  correctResults.push_back( 0.037422 );
  correctResults.push_back( 0.037422 );
  correctResults.push_back( 0.024475 );
  correctResults.push_back( 0.024475 );
  correctResults.push_back( 0.024475 );
  correctResults.push_back( 0.024475 );
  correctResults.push_back( 0.024475 );
  correctResults.push_back( 0.024475 );
  correctResults.push_back( 0.024475 );
  correctResults.push_back( 0.024475 );
  correctResults.push_back( 0.024475 );
  correctResults.push_back( 0.024475 );
  correctResults.push_back( 0.024475 );
  correctResults.push_back( 0.024475 );
  correctResults.push_back( 0.01389  );
  correctResults.push_back( 0.01389  );
  correctResults.push_back( 0.01389  );
  correctResults.push_back( 0.01389  );
  correctResults.push_back( 0.01389  );
  correctResults.push_back( 0.01389  );
  correctResults.push_back( 0.01389  );
  correctResults.push_back( 0.01389  );
  correctResults.push_back( 0.01389  );
  correctResults.push_back( 0.01389  );
  correctResults.push_back( 0.01389  );
  correctResults.push_back( 0.01389  );
  correctResults.push_back( 0.004252 );
  correctResults.push_back( 0.004252 );
  correctResults.push_back( 0.004252 );
  correctResults.push_back( 0.004252 );
  correctResults.push_back( 0.004252 );
  correctResults.push_back( 0.004252 );
  correctResults.push_back( 0.004252 );
  correctResults.push_back( 0.004252 );

  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( (*aT)[i][i] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

  // Front reflectance
  aRf = ( *aResults )[ 1 ]->Rho( Side::Front );

  correctResults.clear();
  calculatedResults.clear();

  correctResults.push_back( 0.61134  );
  correctResults.push_back( 0.661507 );
  correctResults.push_back( 0.661507 );
  correctResults.push_back( 0.661507 );
  correctResults.push_back( 0.661507 );
  correctResults.push_back( 0.661507 );
  correctResults.push_back( 0.661507 );
  correctResults.push_back( 0.661507 );
  correctResults.push_back( 0.661507 );
  correctResults.push_back( 0.659001 );
  correctResults.push_back( 0.659001 );
  correctResults.push_back( 0.659001 );
  correctResults.push_back( 0.659001 );
  correctResults.push_back( 0.659001 );
  correctResults.push_back( 0.659001 );
  correctResults.push_back( 0.659001 );
  correctResults.push_back( 0.659001 );
  correctResults.push_back( 0.659001 );
  correctResults.push_back( 0.659001 );
  correctResults.push_back( 0.659001 );
  correctResults.push_back( 0.659001 );
  correctResults.push_back( 0.974835 );
  correctResults.push_back( 0.974835 );
  correctResults.push_back( 0.974835 );
  correctResults.push_back( 0.974835 );
  correctResults.push_back( 0.974835 );
  correctResults.push_back( 0.974835 );
  correctResults.push_back( 0.974835 );
  correctResults.push_back( 0.974835 );
  correctResults.push_back( 0.974835 );
  correctResults.push_back( 0.974835 );
  correctResults.push_back( 0.974835 );
  correctResults.push_back( 0.974835 );
  correctResults.push_back( 3.654449 );
  correctResults.push_back( 3.654449 );
  correctResults.push_back( 3.654449 );
  correctResults.push_back( 3.654449 );
  correctResults.push_back( 3.654449 );
  correctResults.push_back( 3.654449 );
  correctResults.push_back( 3.654449 );
  correctResults.push_back( 3.654449 );

  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( (*aRf)[i][i] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

  ///////////////////////////////////////////////////////////////////////
  //  Wavelength number 3
  ///////////////////////////////////////////////////////////////////////

  aT = ( *aResults )[ 2 ]->Tau( Side::Front );

  // Test only diagonal of transmittance matrix
  size = aT->getSize();

  correctResults.clear();
  calculatedResults.clear();

  correctResults.push_back( 0.117065 );
  correctResults.push_back( 0.114808 );
  correctResults.push_back( 0.114808 );
  correctResults.push_back( 0.114808 );
  correctResults.push_back( 0.114808 );
  correctResults.push_back( 0.114808 );
  correctResults.push_back( 0.114808 );
  correctResults.push_back( 0.114808 );
  correctResults.push_back( 0.114808 );
  correctResults.push_back( 0.080195 );
  correctResults.push_back( 0.080195 );
  correctResults.push_back( 0.080195 );
  correctResults.push_back( 0.080195 );
  correctResults.push_back( 0.080195 );
  correctResults.push_back( 0.080195 );
  correctResults.push_back( 0.080195 );
  correctResults.push_back( 0.080195 );
  correctResults.push_back( 0.080195 );
  correctResults.push_back( 0.080195 );
  correctResults.push_back( 0.080195 );
  correctResults.push_back( 0.080195 );
  correctResults.push_back( 0.050298 );
  correctResults.push_back( 0.050298 );
  correctResults.push_back( 0.050298 );
  correctResults.push_back( 0.050298 );
  correctResults.push_back( 0.050298 );
  correctResults.push_back( 0.050298 );
  correctResults.push_back( 0.050298 );
  correctResults.push_back( 0.050298 );
  correctResults.push_back( 0.050298 );
  correctResults.push_back( 0.050298 );
  correctResults.push_back( 0.050298 );
  correctResults.push_back( 0.050298 );
  correctResults.push_back( 0.017391 );
  correctResults.push_back( 0.017391 );
  correctResults.push_back( 0.017391 );
  correctResults.push_back( 0.017391 );
  correctResults.push_back( 0.017391 );
  correctResults.push_back( 0.017391 );
  correctResults.push_back( 0.017391 );
  correctResults.push_back( 0.017391 );

  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( (*aT)[i][i] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

  // Front reflectance
  aRf = ( *aResults )[ 2 ]->Rho( Side::Front );

  correctResults.clear();
  calculatedResults.clear();

  correctResults.push_back( 0.61134  );
  correctResults.push_back( 0.661498 );
  correctResults.push_back( 0.661498 );
  correctResults.push_back( 0.661498 );
  correctResults.push_back( 0.661498 );
  correctResults.push_back( 0.661498 );
  correctResults.push_back( 0.661498 );
  correctResults.push_back( 0.661498 );
  correctResults.push_back( 0.661498 );
  correctResults.push_back( 0.658976 );
  correctResults.push_back( 0.658976 );
  correctResults.push_back( 0.658976 );
  correctResults.push_back( 0.658976 );
  correctResults.push_back( 0.658976 );
  correctResults.push_back( 0.658976 );
  correctResults.push_back( 0.658976 );
  correctResults.push_back( 0.658976 );
  correctResults.push_back( 0.658976 );
  correctResults.push_back( 0.658976 );
  correctResults.push_back( 0.658976 );
  correctResults.push_back( 0.658976 );
  correctResults.push_back( 0.974793 );
  correctResults.push_back( 0.974793 );
  correctResults.push_back( 0.974793 );
  correctResults.push_back( 0.974793 );
  correctResults.push_back( 0.974793 );
  correctResults.push_back( 0.974793 );
  correctResults.push_back( 0.974793 );
  correctResults.push_back( 0.974793 );
  correctResults.push_back( 0.974793 );
  correctResults.push_back( 0.974793 );
  correctResults.push_back( 0.974793 );
  correctResults.push_back( 0.974793 );
  correctResults.push_back( 3.654404 );
  correctResults.push_back( 3.654404 );
  correctResults.push_back( 3.654404 );
  correctResults.push_back( 3.654404 );
  correctResults.push_back( 3.654404 );
  correctResults.push_back( 3.654404 );
  correctResults.push_back( 3.654404 );
  correctResults.push_back( 3.654404 );

  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( (*aRf)[i][i] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

  ///////////////////////////////////////////////////////////////////////
  //  Wavelength number 4
  ///////////////////////////////////////////////////////////////////////

  aT = ( *aResults )[ 3 ]->Tau( Side::Front );

  // Test only diagonal of transmittance matrix
  size = aT->getSize();

  correctResults.clear();
  calculatedResults.clear();

  correctResults.push_back( 0.455253 );
  correctResults.push_back( 0.458994 );
  correctResults.push_back( 0.458994 );
  correctResults.push_back( 0.458994 );
  correctResults.push_back( 0.458994 );
  correctResults.push_back( 0.458994 );
  correctResults.push_back( 0.458994 );
  correctResults.push_back( 0.458994 );
  correctResults.push_back( 0.458994 );
  correctResults.push_back( 0.347745 );
  correctResults.push_back( 0.347745 );
  correctResults.push_back( 0.347745 );
  correctResults.push_back( 0.347745 );
  correctResults.push_back( 0.347745 );
  correctResults.push_back( 0.347745 );
  correctResults.push_back( 0.347745 );
  correctResults.push_back( 0.347745 );
  correctResults.push_back( 0.347745 );
  correctResults.push_back( 0.347745 );
  correctResults.push_back( 0.347745 );
  correctResults.push_back( 0.347745 );
  correctResults.push_back( 0.246756 );
  correctResults.push_back( 0.246756 );
  correctResults.push_back( 0.246756 );
  correctResults.push_back( 0.246756 );
  correctResults.push_back( 0.246756 );
  correctResults.push_back( 0.246756 );
  correctResults.push_back( 0.246756 );
  correctResults.push_back( 0.246756 );
  correctResults.push_back( 0.246756 );
  correctResults.push_back( 0.246756 );
  correctResults.push_back( 0.246756 );
  correctResults.push_back( 0.246756 );
  correctResults.push_back( 0.09914  );
  correctResults.push_back( 0.09914  );
  correctResults.push_back( 0.09914  );
  correctResults.push_back( 0.09914  );
  correctResults.push_back( 0.09914  );
  correctResults.push_back( 0.09914  );
  correctResults.push_back( 0.09914  );
  correctResults.push_back( 0.09914  );

  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( (*aT)[i][i] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

  // Front reflectance
  aRf = ( *aResults )[ 3 ]->Rho( Side::Front );

  correctResults.clear();
  calculatedResults.clear();

  correctResults.push_back( 0.61134  );
  correctResults.push_back( 0.661398 );
  correctResults.push_back( 0.661398 );
  correctResults.push_back( 0.661398 );
  correctResults.push_back( 0.661398 );
  correctResults.push_back( 0.661398 );
  correctResults.push_back( 0.661398 );
  correctResults.push_back( 0.661398 );
  correctResults.push_back( 0.661398 );
  correctResults.push_back( 0.658665 );
  correctResults.push_back( 0.658665 );
  correctResults.push_back( 0.658665 );
  correctResults.push_back( 0.658665 );
  correctResults.push_back( 0.658665 );
  correctResults.push_back( 0.658665 );
  correctResults.push_back( 0.658665 );
  correctResults.push_back( 0.658665 );
  correctResults.push_back( 0.658665 );
  correctResults.push_back( 0.658665 );
  correctResults.push_back( 0.658665 );
  correctResults.push_back( 0.658665 );
  correctResults.push_back( 0.974245 );
  correctResults.push_back( 0.974245 );
  correctResults.push_back( 0.974245 );
  correctResults.push_back( 0.974245 );
  correctResults.push_back( 0.974245 );
  correctResults.push_back( 0.974245 );
  correctResults.push_back( 0.974245 );
  correctResults.push_back( 0.974245 );
  correctResults.push_back( 0.974245 );
  correctResults.push_back( 0.974245 );
  correctResults.push_back( 0.974245 );
  correctResults.push_back( 0.974245 );
  correctResults.push_back( 3.653868 );
  correctResults.push_back( 3.653868 );
  correctResults.push_back( 3.653868 );
  correctResults.push_back( 3.653868 );
  correctResults.push_back( 3.653868 );
  correctResults.push_back( 3.653868 );
  correctResults.push_back( 3.653868 );
  correctResults.push_back( 3.653868 );

  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( (*aRf)[i][i] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

  ///////////////////////////////////////////////////////////////////////
  //  Wavelength number 5
  ///////////////////////////////////////////////////////////////////////

  aT = ( *aResults )[ 4 ]->Tau( Side::Front );

  // Test only diagonal of transmittance matrix
  size = aT->getSize();

  correctResults.clear();
  calculatedResults.clear();

  correctResults.push_back( 1.300724 );
  correctResults.push_back( 1.339503 );
  correctResults.push_back( 1.339503 );
  correctResults.push_back( 1.339503 );
  correctResults.push_back( 1.339503 );
  correctResults.push_back( 1.339503 );
  correctResults.push_back( 1.339503 );
  correctResults.push_back( 1.339503 );
  correctResults.push_back( 1.339503 );
  correctResults.push_back( 1.080006 );
  correctResults.push_back( 1.080006 );
  correctResults.push_back( 1.080006 );
  correctResults.push_back( 1.080006 );
  correctResults.push_back( 1.080006 );
  correctResults.push_back( 1.080006 );
  correctResults.push_back( 1.080006 );
  correctResults.push_back( 1.080006 );
  correctResults.push_back( 1.080006 );
  correctResults.push_back( 1.080006 );
  correctResults.push_back( 1.080006 );
  correctResults.push_back( 1.080006 );
  correctResults.push_back( 0.842257 );
  correctResults.push_back( 0.842257 );
  correctResults.push_back( 0.842257 );
  correctResults.push_back( 0.842257 );
  correctResults.push_back( 0.842257 );
  correctResults.push_back( 0.842257 );
  correctResults.push_back( 0.842257 );
  correctResults.push_back( 0.842257 );
  correctResults.push_back( 0.842257 );
  correctResults.push_back( 0.842257 );
  correctResults.push_back( 0.842257 );
  correctResults.push_back( 0.842257 );
  correctResults.push_back( 0.379491 );
  correctResults.push_back( 0.379491 );
  correctResults.push_back( 0.379491 );
  correctResults.push_back( 0.379491 );
  correctResults.push_back( 0.379491 );
  correctResults.push_back( 0.379491 );
  correctResults.push_back( 0.379491 );
  correctResults.push_back( 0.379491 );

  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( (*aT)[i][i] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

  // Front reflectance
  aRf = ( *aResults )[ 4 ]->Rho( Side::Front );

  correctResults.clear();
  calculatedResults.clear();

  correctResults.push_back( 0.61134  );
  correctResults.push_back( 0.660889 );
  correctResults.push_back( 0.660889 );
  correctResults.push_back( 0.660889 );
  correctResults.push_back( 0.660889 );
  correctResults.push_back( 0.660889 );
  correctResults.push_back( 0.660889 );
  correctResults.push_back( 0.660889 );
  correctResults.push_back( 0.660889 );
  correctResults.push_back( 0.657004 );
  correctResults.push_back( 0.657004 );
  correctResults.push_back( 0.657004 );
  correctResults.push_back( 0.657004 );
  correctResults.push_back( 0.657004 );
  correctResults.push_back( 0.657004 );
  correctResults.push_back( 0.657004 );
  correctResults.push_back( 0.657004 );
  correctResults.push_back( 0.657004 );
  correctResults.push_back( 0.657004 );
  correctResults.push_back( 0.657004 );
  correctResults.push_back( 0.657004 );
  correctResults.push_back( 0.971223 );
  correctResults.push_back( 0.971223 );
  correctResults.push_back( 0.971223 );
  correctResults.push_back( 0.971223 );
  correctResults.push_back( 0.971223 );
  correctResults.push_back( 0.971223 );
  correctResults.push_back( 0.971223 );
  correctResults.push_back( 0.971223 );
  correctResults.push_back( 0.971223 );
  correctResults.push_back( 0.971223 );
  correctResults.push_back( 0.971223 );
  correctResults.push_back( 0.971223 );
  correctResults.push_back( 3.65192  );
  correctResults.push_back( 3.65192  );
  correctResults.push_back( 3.65192  );
  correctResults.push_back( 3.65192  );
  correctResults.push_back( 3.65192  );
  correctResults.push_back( 3.65192  );
  correctResults.push_back( 3.65192  );
  correctResults.push_back( 3.65192  );

  for( size_t i = 0; i < size; ++i ) {
    calculatedResults.push_back( (*aRf)[i][i] );
  }

  EXPECT_EQ( correctResults.size(), calculatedResults.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[i], calculatedResults[i], 1e-5 );
  }

}