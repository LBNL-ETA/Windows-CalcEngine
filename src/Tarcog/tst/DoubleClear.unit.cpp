#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "OutdoorEnvironment.hpp"
#include "IndoorEnvironment.hpp"
#include "IGUSolidLayer.hpp"
#include "IGUGapLayer.hpp"
#include "IGU.hpp"
#include "System.hpp"
#include "Surface.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;

// Example of double clear window with inital guess for solution
class TestDoubleClear : public testing::Test {

private:
  shared_ptr< CSystem > m_TarcogSystem;

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
    double solarRadiation = 789;

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
    double solidLayerThickness = 0.005715; // [m]
    double solidLayerConductance = 1;

    shared_ptr< CIGUSolidLayer > aSolidLayer1 = 
      make_shared< CIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
    aSolidLayer1->setSolarAbsorptance( 0.166707709432 );

    shared_ptr< CIGUSolidLayer > aSolidLayer2 = 
      make_shared< CIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
    aSolidLayer2->setSolarAbsorptance( 0.112737670541 );

    double gapThickness = 0.012;
    double gapPressure = 101325;
    shared_ptr< CBaseIGULayer > m_GapLayer = make_shared< CIGUGapLayer >( gapThickness, gapPressure );
    ASSERT_TRUE( m_GapLayer != nullptr );

    double windowWidth = 1;
    double windowHeight = 1;
    shared_ptr< CIGU > aIGU = make_shared< CIGU >( windowWidth, windowHeight );
    ASSERT_TRUE( aIGU != nullptr );
    aIGU->addLayer( aSolidLayer1 );
    aIGU->addLayer( m_GapLayer );
    aIGU->addLayer( aSolidLayer2 );    

    /////////////////////////////////////////////////////////
    // System
    /////////////////////////////////////////////////////////
    m_TarcogSystem = make_shared< CSystem >( aIGU, Indoor, Outdoor );
    ASSERT_TRUE( m_TarcogSystem != nullptr );
  }

public:
  shared_ptr< CSystem > GetSystem() { return m_TarcogSystem; };

};

TEST_F( TestDoubleClear, Test1 ) {
  SCOPED_TRACE( "Begin Test: Double Clear - Surface temperatures" );
  
  shared_ptr< CSystem > aSystem = GetSystem();
  ASSERT_TRUE( aSystem != nullptr );

  //////////////////////////////////////////////////////////////////////
  // UValue run
  //////////////////////////////////////////////////////////////////////

  System aRun = System::Uvalue;

  vector< double > Temperature = *aSystem->getTemperatures( aRun );
  vector< double > correctTemperature = { 258.756688, 259.359226, 279.178510, 279.781048 };
  ASSERT_EQ( correctTemperature.size(), Temperature.size() );

  for( auto i = 0; i < correctTemperature.size(); ++i ) {
    EXPECT_NEAR( correctTemperature[ i ], Temperature[ i ], 1e-5 );
  }

  vector< double > Radiosity = *aSystem->getRadiosities( aRun );
  vector< double > correctRadiosity = { 251.950834, 268.667346, 332.299338, 359.731700 };
  ASSERT_EQ( correctRadiosity.size(), Radiosity.size() );

  for( auto i = 0; i < correctRadiosity.size(); ++i ) {
    EXPECT_NEAR( correctRadiosity[ i ], Radiosity[ i ], 1e-5 );
  }

  size_t numOfIter = aSystem->getNumberOfIterations( aRun );
  EXPECT_EQ( 20, int( numOfIter ) );

  //////////////////////////////////////////////////////////////////////
  // SHGC run
  //////////////////////////////////////////////////////////////////////

  aRun = System::SHGC;

  Temperature = *aSystem->getTemperatures( aRun );
  correctTemperature = { 264.022835, 265.134421, 287.947300, 288.428857 };
  ASSERT_EQ( correctTemperature.size(), Temperature.size() );

  for( auto i = 0; i < correctTemperature.size(); ++i ) {
    EXPECT_NEAR( correctTemperature[ i ], Temperature[ i ], 1e-5 );
  }

  Radiosity = *aSystem->getRadiosities( aRun );
  correctRadiosity = { 269.869356, 295.289318, 374.655901, 397.518724 };
  ASSERT_EQ( correctRadiosity.size(), Radiosity.size() );

  for( auto i = 0; i < correctRadiosity.size(); ++i ) {
    EXPECT_NEAR( correctRadiosity[ i ], Radiosity[ i ], 1e-5 );
  }

  numOfIter = aSystem->getNumberOfIterations( aRun );
  EXPECT_EQ( 21, int( numOfIter ) );

  //////////////////////////////////////////////////////////////////////
  // General results
  //////////////////////////////////////////////////////////////////////
  double Uvalue = aSystem->getUValue();
  EXPECT_NEAR( Uvalue, 2.703359, 1e-5 );

  double SHGC = aSystem->getSHGC( 0.606897 );
  EXPECT_NEAR( SHGC, 0.690096, 1e-5 );
}
