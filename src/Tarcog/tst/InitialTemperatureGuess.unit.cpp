#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "TarOutdoorEnvironment.hpp"
#include "TarIndoorEnvironment.hpp"
#include "TarSurface.hpp"
#include "TarIGUSolidLayer.hpp"
#include "TarIGUGapLayer.hpp"
#include "TarIGU.hpp"
#include "TarcogSystem.hpp"

using namespace Tarcog;
using namespace std;

class TestTemperatureInitialGuess : public testing::Test
{

private:
  shared_ptr< CTarIGU > m_TarcogIGU;
  shared_ptr< CTarcogSystem > m_TarcogSystem;
  shared_ptr< CTarIGUSolidLayer > m_solidLayer1;
  shared_ptr< CTarIGUSolidLayer > m_solidLayer2;

protected:
  virtual void SetUp()
  {
    try {
      /////////////////////////////////////////////////////////
      // Outdoor
      /////////////////////////////////////////////////////////
      double airTemperature = 255.15; // Kelvins
      double pressure = 101325; // Pascals
      double airSpeed = 5.5; // meters per second
      AirHorizontalDirection airDirection = AirHorizontalDirection::Windward;
      shared_ptr< CTarOutdoorEnvironment > Outdoor = 
        make_shared< CTarOutdoorEnvironment >( airTemperature, pressure, airSpeed, airDirection );
      ASSERT_TRUE( Outdoor != nullptr );

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
      double solidLayerThickness = 0.005715; // [m]
      double solidLayerConductance = 1;
      m_solidLayer1 = make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
      ASSERT_TRUE( m_solidLayer1 != nullptr );
      m_solidLayer2 = make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
      ASSERT_TRUE( m_solidLayer2 != nullptr );

      double gapThickness = 0.012;
      double gapPressure = 101325;
      shared_ptr< CTarIGUGapLayer> gapLayer = make_shared< CTarIGUGapLayer >( gapThickness, gapPressure );
      ASSERT_TRUE( gapLayer != nullptr );

      double windowWidth = 1;
      double windowHeight = 1;
      shared_ptr< CTarIGU > TarcogIGU = make_shared< CTarIGU >( windowWidth, windowHeight );
      ASSERT_TRUE( TarcogIGU != nullptr );
      TarcogIGU->addLayer( m_solidLayer1 );
      TarcogIGU->addLayer( gapLayer );
      TarcogIGU->addLayer( m_solidLayer2 );

      /////////////////////////////////////////////////////////
      // System
      /////////////////////////////////////////////////////////
      m_TarcogSystem = make_shared< CTarcogSystem >( TarcogIGU, Indoor, Outdoor );
      ASSERT_TRUE( m_TarcogSystem != nullptr );
    } catch( exception &e ) {
      cout << e.what() << endl;
      throw e;
    }
  };

public:
  shared_ptr< CTarIGUSolidLayer > getLayer1() { return m_solidLayer1; }
  shared_ptr< CTarIGUSolidLayer > getLayer2() { return m_solidLayer2; }

};

TEST_F( TestTemperatureInitialGuess, Test1 )
{
  try {
    SCOPED_TRACE( "Begin Test: Initial temperature and IR guess" );

    shared_ptr< CTarSurface > aSurface = getLayer1()->getSurface( SurfacePosition::Front );
    ASSERT_TRUE( aSurface != nullptr );
    double temperature = aSurface->getTemperature();
    double J = aSurface->J();
    EXPECT_NEAR( 256.282733081615, temperature, 1e-6 );
    EXPECT_NEAR( 244.589307222020, J, 1e-6 );

    aSurface = getLayer1()->getSurface( SurfacePosition::Back );
    ASSERT_TRUE( aSurface != nullptr );
    temperature = aSurface->getTemperature();
    J = aSurface->J();
    EXPECT_NEAR( 262.756302643044, temperature, 1e-6 );
    EXPECT_NEAR( 270.254322031419, J, 1e-6 );

    aSurface = getLayer2()->getSurface( SurfacePosition::Front );
    ASSERT_TRUE( aSurface != nullptr );
    temperature = aSurface->getTemperature();
    J = aSurface->J();
    EXPECT_NEAR( 276.349099622422, temperature, 1e-6 );
    EXPECT_NEAR( 330.668096601357, J, 1e-6 );

    aSurface = getLayer2()->getSurface( SurfacePosition::Back );
    ASSERT_TRUE( aSurface != nullptr );
    temperature = aSurface->getTemperature();
    J = aSurface->J();
    EXPECT_NEAR( 282.822669183851, temperature, 1e-6 );
    EXPECT_NEAR( 362.757956247504, J, 1e-6 );
  } catch( exception &e ) {
    cout << e.what() << endl;
  }

}