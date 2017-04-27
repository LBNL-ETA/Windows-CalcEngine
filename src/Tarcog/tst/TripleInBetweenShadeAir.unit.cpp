#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCEGases.hpp"
#include "WCETarcog.hpp"
#include "WCECommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;

class TestInBetweenShadeAir : public testing::Test {

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

    shared_ptr< CIGUSolidLayer > aLayer3 = make_shared< CIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
    ASSERT_TRUE( aLayer3 != nullptr );

    double shadeLayerThickness = 0.01;
    double shadeLayerConductance = 160;
    double Atop = 0.1;
    double Abot = 0.1;
    double Aleft = 0.1;
    double Aright = 0.1;
    double Afront = 0.2;

    shared_ptr< CIGUSolidLayer > aLayer2 = make_shared< CIGUShadeLayer >( shadeLayerThickness, shadeLayerConductance,
      make_shared< CShadeOpenings >( Atop, Abot, Aleft, Aright, Afront ) );

    ASSERT_TRUE( aLayer2 != nullptr );

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

TEST_F( TestInBetweenShadeAir, Test1 ) {
  SCOPED_TRACE( "Begin Test: InBetween Shade - Air" );

  shared_ptr< CSingleSystem > aSystem = GetSystem();
  
  vector< double > temperature = *aSystem->getTemperatures();
  vector< double > radiosity = *aSystem->getRadiosities();

  vector< double > correctTemp = { 257.908946, 258.369607, 271.538659, 271.542218, 283.615285, 284.075945 };
  vector< double > correctJ = { 249.166619, 260.320613, 300.571561, 316.335545, 358.760651, 378.995512 };

  EXPECT_EQ( correctTemp.size(), temperature.size() );
  EXPECT_EQ( correctJ.size(), radiosity.size() );
  
  for( size_t i = 0; i < temperature.size(); ++i ) {
    EXPECT_NEAR( correctTemp[ i ], temperature[ i ], 1e-6 );
    EXPECT_NEAR( correctJ[ i ], radiosity[ i ], 1e-6 );
  }

  size_t numOfIter = aSystem->getNumberOfIterations();
  EXPECT_EQ( 20, int( numOfIter ) );
}
