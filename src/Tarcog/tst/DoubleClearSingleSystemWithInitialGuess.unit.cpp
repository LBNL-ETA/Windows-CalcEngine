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

// Example of double clear window with inital guess for solution
class TestDoubleClearSingleSystemWithInitialGuess : public testing::Test {

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
    double solidLayerThickness = 0.005715; // [m]
    double solidLayerConductance = 1;

    shared_ptr< CBaseIGULayer > aSolidLayer1 = 
      make_shared< CIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );

    shared_ptr< CBaseIGULayer > aSolidLayer2 = 
      make_shared< CIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );

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
    m_TarcogSystem = make_shared< CSingleSystem >( aIGU, Indoor, Outdoor );
    ASSERT_TRUE( m_TarcogSystem != nullptr );

    // set up initial guess. It was taken to be close to solution.
    shared_ptr< vector < double > > aTemperatures = make_shared< vector< double > >();
    aTemperatures->push_back( 258.75 );
    aTemperatures->push_back( 259.36 );
    aTemperatures->push_back( 279.18 );
    aTemperatures->push_back( 279.78 );
    m_TarcogSystem->setInitialGuess( aTemperatures );

    m_TarcogSystem->solve();
  }

public:
  shared_ptr< CSingleSystem > GetSystem() { return m_TarcogSystem; };

};

TEST_F( TestDoubleClearSingleSystemWithInitialGuess, Test1 ) {
  SCOPED_TRACE( "Begin Test: Double Clear Single System (Initial guess) - Surface temperatures" );
  
  shared_ptr< CSingleSystem > aSystem = GetSystem();
  ASSERT_TRUE( aSystem != nullptr );

  vector< double > Temperature = *aSystem->getTemperatures();
  vector< double > correctTemperature = { 258.756688, 259.359226, 279.178510, 279.781048 };
  ASSERT_EQ( correctTemperature.size(), Temperature.size() );

  for( auto i = 0u; i < correctTemperature.size(); ++i ) {
    EXPECT_NEAR( correctTemperature[ i ], Temperature[ i ], 1e-5 );
  }

  vector< double > Radiosity = *aSystem->getRadiosities();
  vector< double > correctRadiosity = { 251.950834, 268.667346, 332.299338, 359.731700 };
  ASSERT_EQ( correctRadiosity.size(), Radiosity.size() );

  for( auto i = 0u; i < correctRadiosity.size(); ++i ) {
    EXPECT_NEAR( correctRadiosity[ i ], Radiosity[ i ], 1e-5 );
  }

  size_t numOfIter = aSystem->getNumberOfIterations();
  EXPECT_EQ( 17u, numOfIter );
}
