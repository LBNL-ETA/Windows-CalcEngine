#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "OutdoorEnvironment.hpp"
#include "IndoorEnvironment.hpp"
#include "Surface.hpp"
#include "IGUSolidLayer.hpp"
#include "IGU.hpp"
#include "SingleSystem.hpp"

using namespace Tarcog;
using namespace std;

class TestOutdoorEnvironmentHCalcTSkySpecified : public testing::Test {

private:
  shared_ptr< CEnvironment > Outdoor;
  shared_ptr< CSingleSystem > m_TarcogSystem;

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
    double solarRadiation = 0;

    Outdoor = make_shared< COutdoorEnvironment >( airTemperature, pressure, airSpeed, solarRadiation, 
      airDirection, tSky, SkyModel::TSkySpecified );
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
    double solidLayerConductance = 100;

    shared_ptr< CIGUSolidLayer > aSolidLayer = make_shared< CIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
    ASSERT_TRUE( aSolidLayer != nullptr );

    double windowWidth = 1;
    double windowHeight = 1;
    shared_ptr< CIGU > aIGU = make_shared< CIGU >( windowWidth, windowHeight );
    ASSERT_TRUE( aIGU != nullptr );
    aIGU->addLayer( aSolidLayer );

    /////////////////////////////////////////////////////////
    // System
    /////////////////////////////////////////////////////////
    m_TarcogSystem = make_shared< CSingleSystem >( aIGU, Indoor, Outdoor );
    m_TarcogSystem->solve();
    ASSERT_TRUE( m_TarcogSystem != nullptr );
  }

public:
  shared_ptr< CEnvironment > GetOutdoors() { return Outdoor; };

};

TEST_F( TestOutdoorEnvironmentHCalcTSkySpecified, CalculateH_TSkySpecified ) {
  SCOPED_TRACE( "Begin Test: Outdoors -> H model = Calculate; Sky Model = TSky specified" );
  
  shared_ptr< CEnvironment > aOutdoor = nullptr;
  
  aOutdoor = GetOutdoors();
  ASSERT_TRUE( aOutdoor != nullptr );

  double radiosity = aOutdoor->getEnvironmentIR();
  EXPECT_NEAR( 380.278401885, radiosity, 1e-6 );

  double hc = aOutdoor->getHc();
  EXPECT_NEAR( 26, hc, 1e-6 );

  double outIR = aOutdoor->getRadiationFlow();
  EXPECT_NEAR( 52.1067777, outIR, 1e-6 );

  double outConvection = aOutdoor->getConvectionConductionFlow();
  EXPECT_NEAR( -72.92573417, outConvection, 1e-6 );

  double totalHeatFlow = aOutdoor->getHeatFlow();
  EXPECT_NEAR( -20.81895645, totalHeatFlow, 1e-6 );
}