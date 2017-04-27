#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"
#include "WCEChromogenics.hpp"
#include "WCECommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;
using namespace Chromogenics;

class TestSingleClearThermochromics : public testing::Test {

private:
  shared_ptr< CSystem >  m_TarcogSystem;

protected:
  virtual void SetUp() {
    /////////////////////////////////////////////////////////
    // Outdoor
    /////////////////////////////////////////////////////////
    double airTemperature = 300; // Kelvins
    double pressure = 101325; // Pascals
    double airSpeed = 5.5; // meters per second
    AirHorizontalDirection airDirection = AirHorizontalDirection::Windward;
    double tSky = 270; // Kelvins
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
    shared_ptr< CEnvironment > Indoor = make_shared< CIndoorEnvironment > ( roomTemperature, pressure );
    ASSERT_TRUE( Indoor != nullptr );

    /////////////////////////////////////////////////////////
    // IGU
    /////////////////////////////////////////////////////////
    double solidLayerThickness = 0.003048; // [m]
    double solidLayerConductance = 1;

    double transmittance = 0;
    double emissivity = 0.84;

    // Thermochromics property of the surface emissivity
    vector< pair< double, double > > emissivities = {
      make_pair( 288.15, 0.84 ),
      make_pair( 293.15, 0.74 ),
      make_pair( 296.15, 0.64 ),
      make_pair( 300.15, 0.54 ),
      make_pair( 303.15, 0.44 )
    };

    // Creates thermochromic surface on indoor side with variable emissivity and constant transmittance
    shared_ptr< ISurface > frontSurface = make_shared< CSurface >( emissivity, transmittance );
    shared_ptr< ISurface > backSurface = make_shared< CThermochromicSurface >( emissivities, transmittance );
    // shared_ptr< ISurface > backSurface = make_shared< CSurface >( 0.61350442289072993, transmittance );

    shared_ptr< CIGUSolidLayer > aSolidLayer =
      make_shared< CIGUSolidLayer >( solidLayerThickness, solidLayerConductance, frontSurface, backSurface );
    ASSERT_TRUE( aSolidLayer != nullptr );
    aSolidLayer->setSolarAbsorptance( 0.094189159572 );

    double windowWidth = 1;
    double windowHeight = 1;
    shared_ptr< CIGU > aIGU = make_shared< CIGU >( windowWidth, windowHeight );
    ASSERT_TRUE( aIGU != nullptr );
    aIGU->addLayer( aSolidLayer );

    /////////////////////////////////////////////////////////
    // System
    /////////////////////////////////////////////////////////
    m_TarcogSystem = make_shared< CSystem >( aIGU, Indoor, Outdoor );
    ASSERT_TRUE( m_TarcogSystem != nullptr );

    // m_TarcogSystem->solve();
  }

public:
  shared_ptr< CSystem > GetSystem() { return m_TarcogSystem; };

};

TEST_F( TestSingleClearThermochromics, Test1 ) {
  SCOPED_TRACE( "Begin Test: Single Clear Thermochromics - U-value" );

  shared_ptr< CSystem > aSystem = GetSystem();
  ASSERT_TRUE( aSystem != nullptr );

  /////////////////////////////////////////////////////////////////////////
  //  U-value run
  /////////////////////////////////////////////////////////////////////////
  vector< shared_ptr< CIGUSolidLayer > > aSolidLayers = aSystem->getSolidLayers( System::Uvalue );

  CIGUSolidLayer aLayer = *aSolidLayers[ 0 ];

  double emissivity = aLayer.getSurface( Side::Back )->getEmissivity();
  EXPECT_NEAR( emissivity, 0.610863, 1e-5 );

  vector< double > Temperature = *aSystem->getTemperatures( System::Uvalue );
  vector< double > correctTemperature = { 297.313984, 297.261756 };
  ASSERT_EQ( correctTemperature.size(), Temperature.size() );

  for( auto i = 0u; i < correctTemperature.size(); ++i ) {
    EXPECT_NEAR( correctTemperature[ i ], Temperature[ i ], 1e-5 );
  }

  vector< double > Radiosity = *aSystem->getRadiosities( System::Uvalue );
  vector< double > correctRadiosity = { 432.979711, 435.605837 };
  ASSERT_EQ( correctRadiosity.size(), Radiosity.size() );

  for( auto i = 0u; i < correctRadiosity.size(); ++i ) {
    EXPECT_NEAR( correctRadiosity[ i ], Radiosity[ i ], 1e-5 );
  }

  size_t numOfIterations = aSystem->getNumberOfIterations( System::Uvalue );
  EXPECT_EQ( 19u, numOfIterations );

  /////////////////////////////////////////////////////////////////////////
  //  SHGC run
  /////////////////////////////////////////////////////////////////////////
  aSolidLayers = aSystem->getSolidLayers( System::SHGC );

  aLayer = *aSolidLayers[ 0 ];

  emissivity = aLayer.getSurface( Side::Back )->getEmissivity();
  EXPECT_NEAR( emissivity, 0.561212, 1e-5 );

  Temperature = *aSystem->getTemperatures( System::SHGC );
  correctTemperature = { 299.333611, 299.359313 };
  ASSERT_EQ( correctTemperature.size(), Temperature.size() );

  for( auto i = 0u; i < correctTemperature.size(); ++i ) {
    EXPECT_NEAR( correctTemperature[ i ], Temperature[ i ], 1e-5 );
  }

  Radiosity = *aSystem->getRadiosities( System::SHGC );
  correctRadiosity = { 443.194727, 441.786960 };
  ASSERT_EQ( correctRadiosity.size(), Radiosity.size() );

  for( auto i = 0u; i < correctRadiosity.size(); ++i ) {
    EXPECT_NEAR( correctRadiosity[ i ], Radiosity[ i ], 1e-5 );
  }

  numOfIterations = aSystem->getNumberOfIterations( System::SHGC );
  EXPECT_EQ( 19u, numOfIterations );

  /////////////////////////////////////////////////////////////////////////
  //  Heat flows
  /////////////////////////////////////////////////////////////////////////
  double heatFlow = aSystem->getHeatFlow( System::Uvalue, Environment::Indoor );
  EXPECT_NEAR( heatFlow, -17.135106, 1e-5 );

  heatFlow = aSystem->getHeatFlow( System::Uvalue, Environment::Outdoor );
  EXPECT_NEAR( heatFlow, -17.135106, 1e-5 );

  heatFlow = aSystem->getHeatFlow( System::SHGC, Environment::Indoor );
  EXPECT_NEAR( heatFlow, -28.725048, 1e-5 );

  heatFlow = aSystem->getHeatFlow( System::SHGC, Environment::Outdoor );
  EXPECT_NEAR( heatFlow, 45.590199, 1e-5 );

  /////////////////////////////////////////////////////////////////////////
  //  System properties
  /////////////////////////////////////////////////////////////////////////
  double UValue = aSystem->getUValue();
  EXPECT_NEAR( UValue, 4.604300, 1e-5 );

  double SHGC = aSystem->getSHGC( 0.831249 );
  EXPECT_NEAR( SHGC, 0.845938, 1e-5 );
}
