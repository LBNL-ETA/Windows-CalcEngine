#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "TarOutdoorEnvironment.hpp"
#include "TarIndoorEnvironment.hpp"
#include "TarSurface.hpp"
#include "TarIGUSolidLayer.hpp"
#include "TarIGU.hpp"
#include "TarcogSystem.hpp"

using namespace Tarcog;
using namespace std;

class TestIndoorEnvironment : public testing::Test {

private:
  shared_ptr< CTarIndoorEnvironment > m_Indoor;
  shared_ptr< CTarcogSystem > m_TarcogSystem;

protected:
  virtual void SetUp() {
    try {
      /////////////////////////////////////////////////////////
      // Outdoor
      /////////////////////////////////////////////////////////
      double airTemperature = 300; // Kelvins
      double pressure = 101325; // Pascals
      double airSpeed = 5.5; // meters per second
      AirHorizontalDirection airDirection = AirHorizontalDirection::Windward;
      double tSky = 270; // Kelvins

      shared_ptr< CTarOutdoorEnvironment > Outdoor = 
        make_shared< CTarOutdoorEnvironment >( airTemperature, pressure, airSpeed, airDirection );
      ASSERT_TRUE( Outdoor != nullptr );
      Outdoor->setSkyTemperature( tSky );
      Outdoor->setHCoeffModel( BoundaryConditionsCoeffModel::CalculateH );
      Outdoor->setSkyModel( SkyModel::AllSpecified );

      /////////////////////////////////////////////////////////
      // Indoor
      /////////////////////////////////////////////////////////

      double roomTemperature = 294.15;
      double roomAirSpeed = 0;

      m_Indoor = make_shared< CTarIndoorEnvironment > ( roomTemperature, pressure, roomAirSpeed, airDirection );
      ASSERT_TRUE( m_Indoor != nullptr );

      /////////////////////////////////////////////////////////
      // IGU
      /////////////////////////////////////////////////////////
      double solidLayerThickness = 0.003048; // [m]
      double solidLayerConductance = 100;

      shared_ptr< CTarIGUSolidLayer > aSolidLayer = make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
      ASSERT_TRUE( aSolidLayer != nullptr );

      double windowWidth = 1;
      double windowHeight = 1;
      shared_ptr< CTarIGU > aIGU = make_shared< CTarIGU >( windowWidth, windowHeight );
      ASSERT_TRUE( aIGU != nullptr );
      aIGU->addLayer( aSolidLayer );

      /////////////////////////////////////////////////////////
      // System
      /////////////////////////////////////////////////////////
      m_TarcogSystem = make_shared< CTarcogSystem >( aIGU, m_Indoor, Outdoor );
      ASSERT_TRUE( m_TarcogSystem != nullptr );
    } catch( exception &e ) {
      cout << e.what() << endl;
      throw &e;
    }
  }

public:
  shared_ptr< CTarIndoorEnvironment > GetIndoors() { return m_Indoor; };

};

TEST_F( TestIndoorEnvironment, IndoorRadiosity ) {
  try {
    SCOPED_TRACE( "Begin Test: Indoors -> Radiosity" );
    
    shared_ptr< CTarIndoorEnvironment > aIndoor = nullptr;
    
    aIndoor = GetIndoors();
    ASSERT_TRUE( aIndoor != nullptr );

    double radiosity = aIndoor->getEnvironmentRadiosity();
    EXPECT_NEAR( 424.458749869075, radiosity, 1e-6 );
  } catch( exception &e ) {
    cout << e.what() << endl;
  }
}

TEST_F( TestIndoorEnvironment, IndoorConvection ) {
  try {
    SCOPED_TRACE( "Begin Test: Indoors -> Convection" );
    
    shared_ptr< CTarIndoorEnvironment > aIndoor = nullptr;
    
    aIndoor = GetIndoors();
    ASSERT_TRUE( aIndoor != nullptr );

    double convectionFlow = aIndoor->getConvectionConductionFlow();
    EXPECT_NEAR( -8.616755444, convectionFlow, 1e-6 );
  } catch( exception &e ) {
    cout << e.what() << endl;
  }
}