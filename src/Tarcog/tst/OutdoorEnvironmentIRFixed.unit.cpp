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

class TestOutdoorEnvironmentIRFixed : public testing::Test {

private:
  shared_ptr< CTarOutdoorEnvironment > Outdoor;
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
    double IRRadiation = 370; // [ W/m2 ]

    Outdoor = make_shared< CTarOutdoorEnvironment >( airTemperature, pressure, airSpeed, airDirection );
    ASSERT_TRUE( Outdoor != nullptr );
    Outdoor->setInfraredRadiation( IRRadiation );

    /////////////////////////////////////////////////////////
    // Indoor
    /////////////////////////////////////////////////////////

    double roomTemperature = 294.15;
    double roomAirSpeed = 0;

    shared_ptr< CTarIndoorEnvironment > Indoor = 
      make_shared< CTarIndoorEnvironment > ( roomTemperature, pressure, roomAirSpeed, airDirection );
    ASSERT_TRUE( Indoor != nullptr );

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
    m_TarcogSystem = make_shared< CTarcogSystem >( aIGU, Indoor, Outdoor );
    ASSERT_TRUE( m_TarcogSystem != nullptr );
    } catch( exception &e ) {
      cout << e.what() << endl;
      throw &e;
    }
  }

public:
  std::shared_ptr< CTarOutdoorEnvironment > GetOutdoors() { return Outdoor; };

};

TEST_F( TestOutdoorEnvironmentIRFixed, CalculateIRFixed ) {

    SCOPED_TRACE( "Begin Test: Outdoors -> Infrared radiation fixed (user input)." );
    
    shared_ptr< CTarOutdoorEnvironment > aOutdoor = nullptr;
    
    aOutdoor = GetOutdoors();
    ASSERT_TRUE( aOutdoor != nullptr );

    double radiosity = aOutdoor->getIRRadiation();

    EXPECT_NEAR( 370, radiosity, 1e-6 );

}