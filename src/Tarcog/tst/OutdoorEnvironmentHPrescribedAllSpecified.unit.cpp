#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

using namespace Tarcog;
using namespace std;

class TestOutdoorEnvironmentHPrescribedAllSpecified : public testing::Test {

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
    double hout = 20;

    Outdoor = make_shared< COutdoorEnvironment >( airTemperature, pressure, airSpeed, solarRadiation, 
      airDirection, tSky, SkyModel::AllSpecified );
    ASSERT_TRUE( Outdoor != nullptr );
    Outdoor->setHCoeffModel( BoundaryConditionsCoeffModel::HPrescribed, hout );

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

    shared_ptr< CIGUSolidLayer > aSolidLayer = 
      make_shared< CIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
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
  std::shared_ptr< CEnvironment > GetOutdoors() { return Outdoor; };

};

TEST_F( TestOutdoorEnvironmentHPrescribedAllSpecified, HPrescribed_AllSpecified ) {
  SCOPED_TRACE( "Begin Test: Outdoors -> H model = Prescribed; Sky Model = All Specified" );
  
  shared_ptr< CEnvironment > aOutdoor = nullptr;
  
  aOutdoor = GetOutdoors();
  ASSERT_TRUE( aOutdoor != nullptr );

  double radiosity = aOutdoor->getEnvironmentIR();
  EXPECT_NEAR( 459.2457, radiosity, 1e-5 );

  double hc = aOutdoor->getHc();
  EXPECT_NEAR( 14.895502, hc, 1e-5 );

  double outIR = aOutdoor->getRadiationFlow();
  EXPECT_NEAR( -7.777658, outIR, 1e-5 );

  double outConvection = aOutdoor->getConvectionConductionFlow();
  EXPECT_NEAR( -22.696083, outConvection, 1e-5 );

  double totalHeatFlow = aOutdoor->getHeatFlow();
  EXPECT_NEAR( -30.473740, totalHeatFlow, 1e-5 );
}