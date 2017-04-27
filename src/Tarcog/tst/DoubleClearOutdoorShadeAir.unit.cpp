#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCEGases.hpp"
#include "WCETarcog.hpp"
#include "WCECommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;

class TestDoubleClearOutdoorShadeAir : public testing::Test {

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

    double shadeLayerThickness = 0.01;
    double shadeLayerConductance = 160;
    double dtop = 0.1;
    double dbot = 0.1;
    double dleft = 0.1;
    double dright = 0.1;
    double Afront = 0.2;

    shared_ptr< CIGUSolidLayer > aLayer1 = 
      make_shared< CIGUShadeLayer >( shadeLayerThickness, shadeLayerConductance,
      make_shared< CShadeOpenings >( dtop, dbot, dleft, dright, Afront ) );

    ASSERT_TRUE( aLayer1 != nullptr );

    shared_ptr< CIGUSolidLayer > aLayer2 = 
      make_shared< CIGUSolidLayer >( solidLayerThickness, solidLayerConductance );
    ASSERT_TRUE( aLayer2 != nullptr );

    shared_ptr< CIGUSolidLayer > aLayer3 = 
      make_shared< CIGUSolidLayer >( solidLayerThickness, solidLayerConductance );

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

TEST_F( TestDoubleClearOutdoorShadeAir, Test1 ) {
  SCOPED_TRACE( "Begin Test: Outdoor Shade - Air" );
  
  shared_ptr< CSingleSystem > aSystem = GetSystem();

  vector< double > temperature = *aSystem->getTemperatures();
  vector< double > radiosity = *aSystem->getRadiosities();

  vector< double > correctTemp = { 256.984174, 256.987521, 269.436767, 269.879607, 284.039136, 284.481976 };
  vector< double > correctJ = { 246.160566, 254.396303, 291.699410, 310.189965, 359.623545, 380.772533 };

  EXPECT_EQ( correctTemp.size(), temperature.size() );
  EXPECT_EQ( correctJ.size(), radiosity.size() );

  for( size_t i = 0; i < temperature.size(); ++i ) {
    EXPECT_NEAR( correctTemp[ i ], temperature[ i ], 1e-6 );
    EXPECT_NEAR( correctJ[ i ], radiosity[ i ], 1e-6 );
  }

  size_t numOfIter = aSystem->getNumberOfIterations();
  EXPECT_EQ( 23, int( numOfIter ) );

  double ventilatedFlow = aSystem->getVentilationFlow( Environment::Outdoor );
  EXPECT_NEAR( -23.931861, ventilatedFlow, 1e-6 );
}
