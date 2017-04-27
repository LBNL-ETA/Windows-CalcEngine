#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCEGases.hpp"
#include "WCETarcog.hpp"
#include "WCECommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;

class TestDoubleClearIndoorShadeAir : public testing::Test {

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

    double roomTemperature = 295.15;

    shared_ptr< CEnvironment > Indoor = make_shared< CIndoorEnvironment > ( roomTemperature, pressure );
    ASSERT_TRUE( Indoor != nullptr );

    /////////////////////////////////////////////////////////
    // IGU
    /////////////////////////////////////////////////////////
    double solidLayerThickness = 0.005715; // [m]
    double solidLayerConductance = 1;

    shared_ptr< CIGUSolidLayer > aLayer1 = make_shared< CIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
    ASSERT_TRUE( aLayer1 != nullptr );

    shared_ptr< CIGUSolidLayer > aLayer2 = make_shared< CIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );

    double shadeLayerThickness = 0.01;
    double shadeLayerConductance = 160;
    double dtop = 0.1;
    double dbot = 0.1;
    double dleft = 0.1;
    double dright = 0.1;
    double Afront = 0.2;

    shared_ptr< CIGUSolidLayer > aLayer3 = make_shared< CIGUShadeLayer >( shadeLayerThickness, shadeLayerConductance,
      make_shared< CShadeOpenings >( dtop, dbot, dleft, dright, Afront ) );

    ASSERT_TRUE( aLayer3 != nullptr );

    double gapThickness = 0.0127;
    double gapPressure = 101325;
    shared_ptr< CIGUGapLayer > GapLayer1 = std::make_shared< CIGUGapLayer >( gapThickness, gapPressure );
    ASSERT_TRUE( GapLayer1 != nullptr );

    shared_ptr< CIGUGapLayer > GapLayer2 = std::make_shared< CIGUGapLayer >( gapThickness, gapPressure );
    ASSERT_TRUE( GapLayer2 != nullptr );

    double windowWidth = 1;
    double windowHeight = 1;
    shared_ptr< CIGU > aIGU = make_shared< CIGU >( windowWidth, windowHeight );
    ASSERT_TRUE( aIGU != nullptr );
    aIGU->addLayer( aLayer1 );
    aIGU->addLayer( GapLayer1 );
    aIGU->addLayer( aLayer2 );
    aIGU->addLayer( GapLayer2 );
    aIGU->addLayer( aLayer3 );

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

TEST_F( TestDoubleClearIndoorShadeAir, Test1 ) {
  SCOPED_TRACE( "Begin Test: Indoor Shade - Air" );
  
  shared_ptr< CSingleSystem > aSystem = GetSystem();

  vector< double > temperature = *aSystem->getTemperatures();
  vector< double > radiosity = *aSystem->getRadiosities();

  vector< double > correctTemp = { 258.2265788, 258.7403799, 276.1996405, 276.7134416, 288.1162677, 288.1193825 };
  vector< double > correctJ = { 250.2066021, 264.5687123, 319.49179, 340.4531177, 382.6512706, 397.0346045 };

  EXPECT_EQ( correctTemp.size(), temperature.size() );
  EXPECT_EQ( correctJ.size(), radiosity.size() );

  for( size_t i = 0; i < temperature.size(); ++i ) {
    EXPECT_NEAR( correctTemp[ i ], temperature[ i ], 1e-6 );
    EXPECT_NEAR( correctJ[ i ], radiosity[ i ], 1e-6 );
  }

  size_t numOfIter = aSystem->getNumberOfIterations();
  EXPECT_EQ( 5, int( numOfIter ) );

  double ventilatedFlow = aSystem->getVentilationFlow( Environment::Indoor );
  EXPECT_NEAR( 40.066869, ventilatedFlow, 1e-6 );
}
