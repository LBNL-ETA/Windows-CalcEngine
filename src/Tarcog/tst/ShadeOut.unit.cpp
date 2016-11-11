#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "OutdoorEnvironment.hpp"
#include "IndoorEnvironment.hpp"
#include "IGUSolidLayer.hpp"
#include "BaseShade.hpp"
#include "IGUGapLayer.hpp"
#include "IGU.hpp"
#include "SingleSystem.hpp"
#include "Surface.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;

class TestShadeOut : public testing::Test {

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

    shared_ptr< COutdoorEnvironment > Outdoor = 
      make_shared< COutdoorEnvironment >( airTemperature, pressure, airSpeed, solarRadiation, 
        airDirection, tSky, SkyModel::AllSpecified );
    ASSERT_TRUE( Outdoor != nullptr );
    Outdoor->setHCoeffModel( BoundaryConditionsCoeffModel::CalculateH );

    /////////////////////////////////////////////////////////
    // Indoor
    /////////////////////////////////////////////////////////

    double roomTemperature = 294.15;

    shared_ptr< CIndoorEnvironment > Indoor = make_shared< CIndoorEnvironment > ( roomTemperature, pressure );
    ASSERT_TRUE( Indoor != nullptr );

    /////////////////////////////////////////////////////////
    // IGU
    /////////////////////////////////////////////////////////
    double emissivity = 0.832855582237;
    double transmittance = 0.074604861438;
    shared_ptr< CSurface > surface1 = make_shared< CSurface > ( emissivity, transmittance );
    shared_ptr< CSurface > surface2 = make_shared< CSurface > ( emissivity, transmittance );

    double shadeLayerThickness = 0.0006;
    double shadeLayerConductance = 160;
    double Atop = 0.0;
    double Abot = 0.0;
    double Aleft = 0.0;
    double Aright = 0.0;
    double Afront = 0.5;

    shared_ptr< CIGUSolidLayer > aSolidLayer1 = make_shared< CIGUShadeLayer >( 
      shadeLayerThickness, shadeLayerConductance,
      make_shared< CShadeOpenings >( Atop, Abot, Aleft, Aright, Afront ), surface1, surface2 );

    ASSERT_TRUE( aSolidLayer1 != nullptr );

    double solidLayerThickness = 0.0056134; // [m]
    double solidLayerConductance = 1;
    double emissivity1 = 0.84;
    double emissivity2 = 0.038798544556;
    transmittance = 0.0;

    shared_ptr< CSurface > surface3 = make_shared< CSurface > ( emissivity1, transmittance );
    shared_ptr< CSurface > surface4 = make_shared< CSurface > ( emissivity2, transmittance );

    shared_ptr< CIGUSolidLayer > aSolidLayer2 = 
      make_shared< CIGUSolidLayer > ( solidLayerThickness, solidLayerConductance, surface3, surface4 );
    ASSERT_TRUE( aSolidLayer2 != nullptr );

    double gapThickness = 0.0127;
    double gapPressure = 101325;
    shared_ptr< CIGUGapLayer > aGapLayer = make_shared< CIGUGapLayer >( gapThickness, gapPressure );
    ASSERT_TRUE( aGapLayer != nullptr );

    double windowWidth = 1;
    double windowHeight = 1;
    shared_ptr< CIGU > aIGU = make_shared< CIGU >( windowWidth, windowHeight );
    ASSERT_TRUE( aIGU != nullptr );
    aIGU->addLayer( aSolidLayer1 );
    aIGU->addLayer( aGapLayer );
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

TEST_F( TestShadeOut, Test1 ) {
  SCOPED_TRACE( "Begin Test: Single Clear - U-value" );
  
  shared_ptr< CSingleSystem > aSystem = GetSystem();
  ASSERT_TRUE( aSystem != nullptr );

  vector< double > Temperature = *aSystem->getTemperatures();
  vector< double > correctTemperature = { 256.991924, 256.992140, 269.666330, 270.128394 };
  ASSERT_EQ( correctTemperature.size(), Temperature.size() );

  for( auto i = 0; i < correctTemperature.size(); ++i ) {
    EXPECT_NEAR( correctTemperature[ i ], Temperature[ i ], 1e-5 );
  }

  vector< double > Radiosity = *aSystem->getRadiosities();
  vector< double > correctRadiosity = { 249.993042, 250.921069, 291.999868, 419.703053 };
  ASSERT_EQ( correctRadiosity.size(), Radiosity.size() );

  for( auto i = 0; i < correctRadiosity.size(); ++i ) {
    EXPECT_NEAR( correctRadiosity[ i ], Radiosity[ i ], 1e-5 );
  }

}