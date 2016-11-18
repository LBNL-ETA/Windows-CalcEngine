#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "OutdoorEnvironment.hpp"
#include "IndoorEnvironment.hpp"
#include "IGUSolidLayer.hpp"
#include "IGU.hpp"
#include "System.hpp"
#include "Surface.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;

class TestSingleClear : public testing::Test {

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

    shared_ptr< CIGUSolidLayer > aSolidLayer =
      make_shared< CIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
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

TEST_F( TestSingleClear, Test1 ) {
  SCOPED_TRACE( "Begin Test: Single Clear - U-value" );

  shared_ptr< CSystem > aSystem = GetSystem();
  ASSERT_TRUE( aSystem != nullptr );

  /////////////////////////////////////////////////////////////////////////
  //  U-value run
  /////////////////////////////////////////////////////////////////////////
  vector< double > Temperature = *aSystem->getTemperatures( System::Uvalue );
  vector< double > correctTemperature = { 297.207035, 297.14470 };
  ASSERT_EQ( correctTemperature.size(), Temperature.size() );

  for( auto i = 0u; i < correctTemperature.size(); ++i ) {
    EXPECT_NEAR( correctTemperature[ i ], Temperature[ i ], 1e-5 );
  }

  vector< double > Radiosity = *aSystem->getRadiosities( System::Uvalue );
  vector< double > correctRadiosity = { 432.444546, 439.201749 };
  ASSERT_EQ( correctRadiosity.size(), Radiosity.size() );

  for( auto i = 0u; i < correctRadiosity.size(); ++i ) {
    EXPECT_NEAR( correctRadiosity[ i ], Radiosity[ i ], 1e-5 );
  }

  size_t numOfIterations = aSystem->getNumberOfIterations( System::Uvalue );
  EXPECT_EQ( 19u, numOfIterations );

  /////////////////////////////////////////////////////////////////////////
  //  SHGC run
  /////////////////////////////////////////////////////////////////////////
  Temperature = *aSystem->getTemperatures( System::SHGC );
  correctTemperature = { 299.116601, 299.121730 };
  ASSERT_EQ( correctTemperature.size(), Temperature.size() );

  for( auto i = 0u; i < correctTemperature.size(); ++i ) {
    EXPECT_NEAR( correctTemperature[ i ], Temperature[ i ], 1e-5 );
  }

  Radiosity = *aSystem->getRadiosities( System::SHGC );
  correctRadiosity = { 442.087153, 449.182158 };
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
  EXPECT_NEAR( heatFlow, -20.450949, 1e-5 );

  heatFlow = aSystem->getHeatFlow( System::Uvalue, Environment::Outdoor );
  EXPECT_NEAR( heatFlow, -20.450949, 1e-5 );

  heatFlow = aSystem->getHeatFlow( System::SHGC, Environment::Indoor );
  EXPECT_NEAR( heatFlow, -35.474878, 1e-5 );

  heatFlow = aSystem->getHeatFlow( System::SHGC, Environment::Outdoor );
  EXPECT_NEAR( heatFlow, 38.840370, 1e-5 );

  /////////////////////////////////////////////////////////////////////////
  //  System properties
  /////////////////////////////////////////////////////////////////////////
  double UValue = aSystem->getUValue();
  EXPECT_NEAR( UValue, 5.493806, 1e-5 );

  double SHGC = aSystem->getSHGC( 0.831249 );
  EXPECT_NEAR( SHGC, 0.850291, 1e-5 );
}
