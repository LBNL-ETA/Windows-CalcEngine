#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "TarOutdoorEnvironment.hpp"
#include "TarIndoorEnvironment.hpp"
#include "TarIGUSolidLayer.hpp"
#include "TarIGUGapLayer.hpp"
#include "TarIGU.hpp"
#include "TarcogSystem.hpp"
#include "TarSurface.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;

class TestDoubleClear : public testing::Test {

private:
  // shared_ptr< CBaseIGUTarcogLayer > m_SolidLayer1;
  // shared_ptr< CBaseIGUTarcogLayer > m_SolidLayer2;
  shared_ptr< CTarcogSystem > m_TarcogSystem;

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
    m_TarcogSystem = make_shared< CTarcogSystem >( aIGU, Indoor, Outdoor );
    ASSERT_TRUE( m_TarcogSystem != nullptr );

    m_TarcogSystem->solve();
  }

public:
  shared_ptr< CTarcogSystem > GetSystem() { return m_TarcogSystem; };

};

TEST_F( TestDoubleClear, Test1 )
{
  SCOPED_TRACE( "Begin Test: Double Clear - Surface temperatures" );
  
  shared_ptr< CTarcogSystem > aSystem = nullptr;
  
  aSystem = GetSystem();
  ASSERT_TRUE( aSystem != nullptr );

  vector< shared_ptr< CTarIGUSolidLayer > > aLayers = aSystem->getSolidLayers();

  // First layer
  shared_ptr< CTarSurface > aSurface = aLayers[ 0 ]->getSurface( Side::Front );
  ASSERT_TRUE( aSurface != nullptr );

  double Temperature = aSurface->getTemperature();
  double Radiosity = aSurface->J();

  EXPECT_NEAR( 258.756688, Temperature, 1e-5 );
  EXPECT_NEAR( 251.950834, Radiosity, 1e-5 );

  aSurface = aLayers[ 0 ]->getSurface( Side::Back );
  ASSERT_TRUE( aSurface != nullptr );

  Temperature = aSurface->getTemperature();
  Radiosity = aSurface->J();

  EXPECT_NEAR( 259.359226, Temperature, 1e-5 );
  EXPECT_NEAR( 268.667346, Radiosity, 1e-5 );

  // Second layer
  aSurface = aLayers[ 1 ]->getSurface( Side::Front );
  ASSERT_TRUE( aSurface != nullptr );

  Temperature = aSurface->getTemperature();
  Radiosity = aSurface->J();

  EXPECT_NEAR( 279.178510, Temperature, 1e-5 );
  EXPECT_NEAR( 332.299338, Radiosity, 1e-5 );

  aSurface = aLayers[ 1 ]->getSurface( Side::Back );
  ASSERT_TRUE( aSurface != nullptr );

  Temperature = aSurface->getTemperature();
  Radiosity = aSurface->J();

  EXPECT_NEAR( 279.781048, Temperature, 1e-5 );
  EXPECT_NEAR( 359.731700, Radiosity, 1e-5 );
}