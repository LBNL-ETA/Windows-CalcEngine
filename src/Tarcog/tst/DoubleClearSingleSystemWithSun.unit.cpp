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
#include "FenestrationCommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;

class TestDoubleClearSingleSystemWithSun : public testing::Test {

private:
  shared_ptr< CSingleSystem > m_TarcogSystem;

protected:
  virtual void SetUp() {    
    /////////////////////////////////////////////////////////
    // Outdoor
    /////////////////////////////////////////////////////////
    double airTemperature = 305.15; // Kelvins
    double pressure = 101325; // Pascals
    double airSpeed = 2.75; // meters per second
    AirHorizontalDirection airDirection = AirHorizontalDirection::Windward;
    double tSky = 305.15; // Kelvins
    double solarRadiation = 783;

    shared_ptr< CEnvironment > Outdoor = 
      make_shared< COutdoorEnvironment >( airTemperature, pressure, airSpeed, solarRadiation,
        airDirection, tSky, SkyModel::AllSpecified );
    ASSERT_TRUE( Outdoor != nullptr );
    Outdoor->setHCoeffModel( BoundaryConditionsCoeffModel::CalculateH );

    /////////////////////////////////////////////////////////
    // Indoor
    /////////////////////////////////////////////////////////

    double roomTemperature = 297.15;

    shared_ptr< CEnvironment > Indoor = 
      make_shared< CIndoorEnvironment > ( roomTemperature, pressure );
    ASSERT_TRUE( Indoor != nullptr );

    /////////////////////////////////////////////////////////
    // IGU
    /////////////////////////////////////////////////////////
    double solidLayerThickness = 0.005715; // [m]
    double solidLayerConductance = 1;
    double solarAbsorptance = 0.187443971634;

    shared_ptr< CIGUSolidLayer > aSolidLayer1 =
      make_shared< CIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
    aSolidLayer1->setSolarAbsorptance( solarAbsorptance );

    solarAbsorptance = 0.054178960621;
    shared_ptr< CIGUSolidLayer > aSolidLayer2 =
      make_shared< CIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
    aSolidLayer2->setSolarAbsorptance( solarAbsorptance );

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

    m_TarcogSystem->solve();
  }

public:
  shared_ptr< CSingleSystem > GetSystem() { return m_TarcogSystem; };

};

TEST_F( TestDoubleClearSingleSystemWithSun, Test1 ) {
  SCOPED_TRACE( "Begin Test: Double Clear Single System - Surface temperatures" );
  
  shared_ptr< CSingleSystem > aSystem = GetSystem();
  ASSERT_TRUE( aSystem != nullptr );

  vector< double > Temperature = *aSystem->getTemperatures();
  vector< double > correctTemperature = { 310.818074, 311.064868, 306.799522, 306.505704 };
  ASSERT_EQ( correctTemperature.size(), Temperature.size() );

  for( auto i = 0u; i < correctTemperature.size(); ++i ) {
    EXPECT_NEAR( correctTemperature[ i ], Temperature[ i ], 1e-5 );
  }

  vector< double > Radiosity = *aSystem->getRadiosities();
  vector< double > correctRadiosity = { 523.148794, 526.906252, 506.252171, 491.059753 };
  ASSERT_EQ( correctRadiosity.size(), Radiosity.size() );

  for( auto i = 0u; i < correctRadiosity.size(); ++i ) {
    EXPECT_NEAR( correctRadiosity[ i ], Radiosity[ i ], 1e-5 );
  }

  double heatFlow = aSystem->getHeatFlow( Environment::Indoor );
  EXPECT_NEAR( -72.622787, heatFlow, 1e-5 );

  double Uvalue = aSystem->getUValue();
  EXPECT_NEAR( 9.077848, Uvalue, 1e-5 );

  size_t numOfIter = aSystem->getNumberOfIterations();
  EXPECT_EQ( 20u, numOfIter );
}
