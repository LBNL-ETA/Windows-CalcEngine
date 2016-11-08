#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "TarOutdoorEnvironment.hpp"
#include "TarIndoorEnvironment.hpp"
#include "TarSurface.hpp"
#include "TarIGUSolidLayer.hpp"
#include "TarIGU.hpp"
#include "TarcogSingleSystem.hpp"

using namespace Tarcog;
using namespace std;

class TestIndoorEnvironment : public testing::Test {

private:
  shared_ptr< CTarEnvironment > m_Indoor;
  shared_ptr< CTarcogSingleSystem > m_TarcogSystem;

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

    shared_ptr< CTarEnvironment > Outdoor = 
      make_shared< CTarOutdoorEnvironment >( airTemperature, pressure, airSpeed, solarRadiation, 
        airDirection, tSky, SkyModel::AllSpecified );
    ASSERT_TRUE( Outdoor != nullptr );
    Outdoor->setHCoeffModel( BoundaryConditionsCoeffModel::CalculateH );

    /////////////////////////////////////////////////////////
    // Indoor
    /////////////////////////////////////////////////////////

    double roomTemperature = 294.15;

    m_Indoor = make_shared< CTarIndoorEnvironment > ( roomTemperature, pressure );
    ASSERT_TRUE( m_Indoor != nullptr );

    /////////////////////////////////////////////////////////
    // IGU
    /////////////////////////////////////////////////////////
    double solidLayerThickness = 0.003048; // [m]
    double solidLayerConductance = 100;

    shared_ptr< CTarIGUSolidLayer > aSolidLayer = 
      make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
    ASSERT_TRUE( aSolidLayer != nullptr );

    double windowWidth = 1;
    double windowHeight = 1;
    shared_ptr< CTarIGU > aIGU = make_shared< CTarIGU >( windowWidth, windowHeight );
    ASSERT_TRUE( aIGU != nullptr );
    aIGU->addLayer( aSolidLayer );

    /////////////////////////////////////////////////////////
    // System
    /////////////////////////////////////////////////////////
    m_TarcogSystem = make_shared< CTarcogSingleSystem >( aIGU, m_Indoor, Outdoor );
    m_TarcogSystem->solve();
    ASSERT_TRUE( m_TarcogSystem != nullptr );
  }

public:
  shared_ptr< CTarEnvironment > GetIndoors() { return m_Indoor; };

};

TEST_F( TestIndoorEnvironment, IndoorRadiosity ) {
  SCOPED_TRACE( "Begin Test: Indoors -> Radiosity" );
  
  shared_ptr< CTarEnvironment > aIndoor = nullptr;
  
  aIndoor = GetIndoors();
  ASSERT_TRUE( aIndoor != nullptr );

  double radiosity = aIndoor->getEnvironmentIR();
  EXPECT_NEAR( 424.458749869075, radiosity, 1e-6 );
}

TEST_F( TestIndoorEnvironment, IndoorConvection ) {
  SCOPED_TRACE( "Begin Test: Indoors -> Convection Flow" );
  
  shared_ptr< CTarEnvironment > aIndoor = nullptr;
  
  aIndoor = GetIndoors();
  ASSERT_TRUE( aIndoor != nullptr );

  double convectionFlow = aIndoor->getConvectionConductionFlow();
  EXPECT_NEAR( -5.826845, convectionFlow, 1e-6 );
}

TEST_F( TestIndoorEnvironment, IndoorHc ) {
  SCOPED_TRACE( "Begin Test: Indoors -> Convection Coefficient" );

  shared_ptr< CTarEnvironment > aIndoor = nullptr;

  aIndoor = GetIndoors();
  ASSERT_TRUE( aIndoor != nullptr );

  double hc = aIndoor->getHc();
  EXPECT_NEAR( 1.913874, hc, 1e-6 );
}
