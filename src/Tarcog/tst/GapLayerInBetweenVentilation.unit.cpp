#include <memory>
#include <gtest/gtest.h>

#include "OutdoorEnvironment.hpp"
#include "IndoorEnvironment.hpp"
#include "IGUSolidLayer.hpp"
#include "BaseShade.hpp"
#include "IGUGapLayer.hpp"
#include "IGUVentilatedGapLayer.hpp"
#include "IGU.hpp"
#include "SingleSystem.hpp"

using namespace std;
using namespace Tarcog;

class TestGapLayerInBetweenVentilation : public testing::Test {

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

    shared_ptr< CEnvironment > Indoor = 
      make_shared< CIndoorEnvironment > ( roomTemperature, pressure );
    ASSERT_TRUE( Indoor != nullptr );

    // IGU
    double solidLayerThickness = 0.005715; // [m]
    double solidLayerConductance = 1;

    shared_ptr< CIGUSolidLayer > SolidLayer1 = make_shared< CIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
    ASSERT_TRUE( SolidLayer1 != nullptr );

    shared_ptr< CIGUSolidLayer > SolidLayer2 = make_shared< CIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
    ASSERT_TRUE( SolidLayer2 != nullptr );

    double shadeLayerThickness = 0.01;
    double shadeLayerConductance = 160;
    double Atop = 0.1;
    double Abot = 0.1;
    double Aleft = 0.1;
    double Aright = 0.1;
    double Afront = 0.2;

    shared_ptr< CIGUShadeLayer > shadeLayer = make_shared< CIGUShadeLayer >( shadeLayerThickness, shadeLayerConductance,
      make_shared< CShadeOpenings >( Atop, Abot, Aleft, Aright, Afront ) );

    ASSERT_TRUE( shadeLayer != nullptr );

    double gapThickness = 0.0127;
    double gapPressure = 101325;
    shared_ptr< CBaseIGULayer > m_GapLayer1 = 
      make_shared< CIGUGapLayer >( gapThickness, gapPressure );
    ASSERT_TRUE( m_GapLayer1 != nullptr );

    shared_ptr< CBaseIGULayer > m_GapLayer2 = 
      make_shared< CIGUGapLayer >( gapThickness, gapPressure );
    ASSERT_TRUE( m_GapLayer2 != nullptr );

    double windowWidth = 1;
    double windowHeight = 1;
    shared_ptr< CIGU > aIGU = make_shared< CIGU >( windowWidth, windowHeight );
    ASSERT_TRUE( aIGU != nullptr );
    aIGU->addLayer( SolidLayer1 );
    aIGU->addLayer( m_GapLayer1 );
    aIGU->addLayer( shadeLayer );
    aIGU->addLayer( m_GapLayer2 );
    aIGU->addLayer( SolidLayer2 );

    /////////////////////////////////////////////////////////
    // System
    /////////////////////////////////////////////////////////
    m_TarcogSystem = make_shared< CSingleSystem >( aIGU, Indoor, Outdoor );
    ASSERT_TRUE( m_TarcogSystem != nullptr );
  }

public:
  shared_ptr< CBaseIGULayer > GetGap1() { return m_TarcogSystem->getGapLayers()[ 0 ]; };
  shared_ptr< CBaseIGULayer > GetGap2() { return m_TarcogSystem->getGapLayers()[ 1 ]; };

};

TEST_F( TestGapLayerInBetweenVentilation, VentilationFlow ) {
  try {
    SCOPED_TRACE( "Begin Test: Test Ventilated Gap Layer - Intial Airflow" );
    
    shared_ptr< CBaseIGULayer > aLayer = GetGap1();

    // Airflow iterations are set to 1e-4 and it cannot exceed that precision
    
    ASSERT_TRUE( aLayer != nullptr );
    double gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR( 32.414571203538848, gainEnergy, 1e-4 );

    aLayer = GetGap2();
    ASSERT_TRUE( aLayer != nullptr );
    gainEnergy = aLayer->getGainFlow();
    EXPECT_NEAR( -32.414571203538848, gainEnergy, 1e-4 );


  } catch( exception &e ) {
    cout << e.what() << endl;
  }

}