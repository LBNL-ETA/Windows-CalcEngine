#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "TarOutdoorEnvironment.hpp"
#include "TarIndoorEnvironment.hpp"
#include "TarIGUSolidLayer.hpp"
#include "TarIGUGapLayer.hpp"
#include "TarIGU.hpp"
#include "TarcogSingleSystem.hpp"
#include "TarSurface.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;

class TestDoubleClearSingleSystemNoSun : public testing::Test {

private:
  shared_ptr< CTarcogSingleSystem > m_TarcogSystem;

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

    shared_ptr< CTarEnvironment > Outdoor = 
      make_shared< CTarOutdoorEnvironment >( airTemperature, pressure, airSpeed, solarRadiation,
        airDirection, tSky, SkyModel::AllSpecified );
    ASSERT_TRUE( Outdoor != nullptr );
    Outdoor->setHCoeffModel( BoundaryConditionsCoeffModel::CalculateH );

    /////////////////////////////////////////////////////////
    // Indoor
    /////////////////////////////////////////////////////////

    double roomTemperature = 294.15;

    shared_ptr< CTarEnvironment > Indoor = 
      make_shared< CTarIndoorEnvironment > ( roomTemperature, pressure );
    ASSERT_TRUE( Indoor != nullptr );

    /////////////////////////////////////////////////////////
    // IGU
    /////////////////////////////////////////////////////////
    double solidLayerThickness = 0.005715; // [m]
    double solidLayerConductance = 1;

    shared_ptr< CBaseIGUTarcogLayer > aSolidLayer1 = 
      make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );

    shared_ptr< CBaseIGUTarcogLayer > aSolidLayer2 = 
      make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );

    double gapThickness = 0.012;
    double gapPressure = 101325;
    shared_ptr< CBaseIGUTarcogLayer > m_GapLayer = make_shared< CTarIGUGapLayer >( gapThickness, gapPressure );
    ASSERT_TRUE( m_GapLayer != nullptr );

    double windowWidth = 1;
    double windowHeight = 1;
    shared_ptr< CTarIGU > aIGU = make_shared< CTarIGU >( windowWidth, windowHeight );
    ASSERT_TRUE( aIGU != nullptr );
    aIGU->addLayer( aSolidLayer1 );
    aIGU->addLayer( m_GapLayer );
    aIGU->addLayer( aSolidLayer2 );

    /////////////////////////////////////////////////////////
    // System
    /////////////////////////////////////////////////////////
    m_TarcogSystem = make_shared< CTarcogSingleSystem >( aIGU, Indoor, Outdoor );
    ASSERT_TRUE( m_TarcogSystem != nullptr );

    m_TarcogSystem->solve();
  }

public:
  shared_ptr< CTarcogSingleSystem > GetSystem() { return m_TarcogSystem; };

};

TEST_F( TestDoubleClearSingleSystemNoSun, Test1 ) {
  SCOPED_TRACE( "Begin Test: Double Clear Single System - Surface temperatures" );
  
  shared_ptr< CTarcogSingleSystem > aSystem = GetSystem();
  ASSERT_TRUE( aSystem != nullptr );

  vector< double > Temperature = *aSystem->getTemperatures();
  vector< double > correctTemperature = { 258.756688, 259.359226, 279.178510, 279.781048 };
  ASSERT_EQ( correctTemperature.size(), Temperature.size() );

  for( auto i = 0; i < correctTemperature.size(); ++i ) {
    EXPECT_NEAR( correctTemperature[ i ], Temperature[ i ], 1e-5 );
  }

  vector< double > Radiosity = *aSystem->getRadiosities();
  vector< double > correctRadiosity = { 251.950834, 268.667346, 332.299338, 359.731700 };
  ASSERT_EQ( correctRadiosity.size(), Radiosity.size() );

  for( auto i = 0; i < correctRadiosity.size(); ++i ) {
    EXPECT_NEAR( correctRadiosity[ i ], Radiosity[ i ], 1e-5 );
  }

  double heatFlow = aSystem->getHeatFlow( Environment::Indoor );
  EXPECT_NEAR( 105.431019, heatFlow, 1e-5 );

  double Uvalue = aSystem->getUValue();
  EXPECT_NEAR( 2.703359, Uvalue, 1e-5 );

  size_t numOfIter = aSystem->getNumberOfIterations();
  EXPECT_EQ( 20, int( numOfIter ) );
}
