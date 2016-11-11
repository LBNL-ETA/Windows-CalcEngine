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

class TestIndoorEnvironment : public testing::Test {

private:
  shared_ptr< CEnvironment > m_Indoor;
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

    shared_ptr< CEnvironment > Outdoor = 
      make_shared< COutdoorEnvironment >( airTemperature, pressure, airSpeed, solarRadiation, 
        airDirection, tSky, SkyModel::AllSpecified );
    ASSERT_TRUE( Outdoor != nullptr );
    Outdoor->setHCoeffModel( BoundaryConditionsCoeffModel::CalculateH );

    /////////////////////////////////////////////////////////
    // Indoor
    /////////////////////////////////////////////////////////

    double roomTemperature = 294.15;

    m_Indoor = make_shared< CIndoorEnvironment > ( roomTemperature, pressure );
    ASSERT_TRUE( m_Indoor != nullptr );

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
    m_TarcogSystem = make_shared< CSingleSystem >( aIGU, m_Indoor, Outdoor );
    m_TarcogSystem->solve();
    ASSERT_TRUE( m_TarcogSystem != nullptr );
  }

public:
  shared_ptr< CEnvironment > GetIndoors() { return m_Indoor; };

};

TEST_F( TestIndoorEnvironment, IndoorRadiosity ) {
  SCOPED_TRACE( "Begin Test: Indoors -> Radiosity" );
  
  shared_ptr< CEnvironment > aIndoor = nullptr;
  
  aIndoor = GetIndoors();
  ASSERT_TRUE( aIndoor != nullptr );

  double radiosity = aIndoor->getEnvironmentIR();
  EXPECT_NEAR( 424.458749869075, radiosity, 1e-6 );
}

TEST_F( TestIndoorEnvironment, IndoorConvection ) {
  SCOPED_TRACE( "Begin Test: Indoors -> Convection Flow" );
  
  shared_ptr< CEnvironment > aIndoor = nullptr;
  
  aIndoor = GetIndoors();
  ASSERT_TRUE( aIndoor != nullptr );

  double convectionFlow = aIndoor->getConvectionConductionFlow();
  EXPECT_NEAR( -5.826845, convectionFlow, 1e-6 );
}

TEST_F( TestIndoorEnvironment, IndoorHc ) {
  SCOPED_TRACE( "Begin Test: Indoors -> Convection Coefficient" );

  shared_ptr< CEnvironment > aIndoor = nullptr;

  aIndoor = GetIndoors();
  ASSERT_TRUE( aIndoor != nullptr );

  double hc = aIndoor->getHc();
  EXPECT_NEAR( 1.913874, hc, 1e-6 );
}
