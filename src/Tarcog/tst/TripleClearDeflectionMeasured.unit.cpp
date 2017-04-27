#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "OutdoorEnvironment.hpp"
#include "IndoorEnvironment.hpp"
#include "IGUSolidLayer.hpp"
#include "IGUGapLayer.hpp"
#include "IGU.hpp"
#include "SingleSystem.hpp"
#include "Surface.hpp"
#include "WCECommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;

class TripleClearDeflectionMeasured : public testing::Test {

private:
  shared_ptr< CSingleSystem > m_TarcogSystem;

protected:
  virtual void SetUp() {    
    /////////////////////////////////////////////////////////
    // Outdoor
    /////////////////////////////////////////////////////////
    double airTemperature = 255.15; // Kelvins
    double pressure = 101325; // Pascals
    double airSpeed = 5.5; // meters per second
    AirHorizontalDirection airDirection = AirHorizontalDirection::Windward;
    double tSky = 255.15; // Kelvins
    double solarRadiation = 0;

    shared_ptr< CEnvironment > Outdoor = 
      make_shared< COutdoorEnvironment >( airTemperature, pressure, airSpeed, solarRadiation,
        airDirection, tSky, SkyModel::AllSpecified );
    ASSERT_TRUE( Outdoor != nullptr );
    Outdoor->setHCoeffModel( BoundaryConditionsCoeffModel::CalculateH );

    /////////////////////////////////////////////////////////
    // Indoor
    /////////////////////////////////////////////////////////

    double roomTemperature = 294.15;

    shared_ptr< CEnvironment > Indoor = 
      make_shared< CIndoorEnvironment > ( roomTemperature, pressure );
    ASSERT_TRUE( Indoor != nullptr );

    /////////////////////////////////////////////////////////
    // IGU
    /////////////////////////////////////////////////////////
    double solidLayerThickness1 = 0.003048; // [m]
    double solidLayerThickness2 = 0.005715;
    double solidLayerConductance = 1;

    shared_ptr< CBaseIGULayer > aSolidLayer1 = 
      make_shared< CIGUSolidLayer > ( solidLayerThickness1, solidLayerConductance );

    shared_ptr< CBaseIGULayer > aSolidLayer2 = 
      make_shared< CIGUSolidLayer > ( solidLayerThickness2, solidLayerConductance );

    shared_ptr< CBaseIGULayer > aSolidLayer3 =
      make_shared< CIGUSolidLayer >( solidLayerThickness1, solidLayerConductance );

    double gapThickness = 0.0127;
    double gapPressure = 101325;
    shared_ptr< CBaseIGULayer > aGapLayer1 = make_shared< CIGUGapLayer >( gapThickness, gapPressure );
    ASSERT_TRUE( aGapLayer1 != nullptr );

    shared_ptr< CBaseIGULayer > aGapLayer2 = make_shared< CIGUGapLayer >( gapThickness, gapPressure );
    ASSERT_TRUE( aGapLayer2 != nullptr );

    double windowWidth = 1;
    double windowHeight = 1;
    shared_ptr< CIGU > aIGU = make_shared< CIGU >( windowWidth, windowHeight );
    ASSERT_TRUE( aIGU != nullptr );
    aIGU->addLayer( aSolidLayer1 );
    aIGU->addLayer( aGapLayer1 );
    aIGU->addLayer( aSolidLayer2 );
    aIGU->addLayer( aGapLayer2 );
    aIGU->addLayer( aSolidLayer3 );

    // Deflection properties
    vector< double > measuredGapsWidths = { 0.0135, 0.013 };
    aIGU->setDeflectionProperties( measuredGapsWidths );

    /////////////////////////////////////////////////////////
    // System
    /////////////////////////////////////////////////////////
    m_TarcogSystem = make_shared< CSingleSystem >( aIGU, Indoor, Outdoor );
    ASSERT_TRUE( m_TarcogSystem != nullptr );

    m_TarcogSystem->solve();
  }

public:
  shared_ptr< CSingleSystem > GetSystem() { return m_TarcogSystem; };

};

TEST_F( TripleClearDeflectionMeasured, Test1 ) {
  SCOPED_TRACE( "Begin Test: Triple Clear - Measured Deflection." );
  
  shared_ptr< CSingleSystem > aSystem = GetSystem();
  ASSERT_TRUE( aSystem != nullptr );

  vector< double > Temperature = *aSystem->getTemperatures();
  vector< double > correctTemperature = { 257.493976, 257.702652, 271.535517, 271.926785, 284.395405, 284.604082 };
  ASSERT_EQ( correctTemperature.size(), Temperature.size() );

  for( auto i = 0u; i < correctTemperature.size(); ++i ) {
    EXPECT_NEAR( correctTemperature[ i ], Temperature[ i ], 1e-5 );
  }

  vector< double > Radiosity = *aSystem->getRadiosities();
  vector< double > correctRadiosity = { 247.813715, 258.078374, 300.200818, 318.403140, 362.495875, 380.380188 };
  ASSERT_EQ( correctRadiosity.size(), Radiosity.size() );

  for( auto i = 0u; i < correctRadiosity.size(); ++i ) {
    EXPECT_NEAR( correctRadiosity[ i ], Radiosity[ i ], 1e-5 );
  }

  vector< double > MaxDeflection = *aSystem->getMaxDeflections();
  vector< double > correctMaxDeflection = { 0.00074180, -5.820e-05, -0.0003582 };
  ASSERT_EQ( correctMaxDeflection.size(), MaxDeflection.size() );

  for( auto i = 0u; i < correctMaxDeflection.size(); ++i ) {
    EXPECT_NEAR( correctMaxDeflection[ i ], MaxDeflection[ i ], 1e-7 );
  }

  vector< double > MeanDeflection = *aSystem->getMeanDeflections();
  vector< double > correctMeanDeflection = { 0.00031076, -2.437e-05, -0.0001501 };
  ASSERT_EQ( correctMeanDeflection.size(), MeanDeflection.size() );

  for( auto i = 0u; i < correctMaxDeflection.size(); ++i ) {
    EXPECT_NEAR( correctMeanDeflection[ i ], MeanDeflection[ i ], 1e-7 );
  }

  size_t numOfIter = aSystem->getNumberOfIterations();
  EXPECT_EQ( 20u, numOfIter );
}
