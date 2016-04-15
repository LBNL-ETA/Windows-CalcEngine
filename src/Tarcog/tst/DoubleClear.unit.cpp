#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "TarOutdoorEnvironment.hpp"
#include "TarIndoorEnvironment.hpp"
#include "TarIGUSolidLayer.hpp"
#include "TarIGUGapLayer.hpp"
#include "TarIGU.hpp"
#include "TarcogSystem.hpp"
#include "TarSurface.hpp"

using namespace Tarcog;
using namespace std;

class TestDoubleClear : public testing::Test {

private:
  shared_ptr< CTarIGUSolidLayer > m_SolidLayer1;
  shared_ptr< CTarIGUSolidLayer > m_SolidLayer2;
  shared_ptr< CTarcogSystem > m_TarcogSystem;

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
      double tSky = 255.15; // Kelvins

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

      shared_ptr< CTarIndoorEnvironment > Indoor = 
        make_shared< CTarIndoorEnvironment > ( roomTemperature, pressure, roomAirSpeed, airDirection );
      ASSERT_TRUE( Indoor != nullptr );

      /////////////////////////////////////////////////////////
      // IGU
      /////////////////////////////////////////////////////////
      double solidLayerThickness = 0.005715; // [m]
      double solidLayerConductance = 1;

      m_SolidLayer1 = make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
      ASSERT_TRUE( m_SolidLayer1 != nullptr );

      m_SolidLayer2 = make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
      ASSERT_TRUE( m_SolidLayer2 != nullptr );

      double gapThickness = 0.012;
      double gapPressure = 101325;
      shared_ptr< CTarIGUGapLayer > m_GapLayer = std::make_shared< CTarIGUGapLayer >( gapThickness, gapPressure );
      ASSERT_TRUE( m_GapLayer != nullptr );

      double windowWidth = 1;
      double windowHeight = 1;
      shared_ptr< CTarIGU > aIGU = make_shared< CTarIGU >( windowWidth, windowHeight );
      ASSERT_TRUE( aIGU != nullptr );
      aIGU->addLayer( m_SolidLayer1 );
      aIGU->addLayer( m_GapLayer );
      aIGU->addLayer( m_SolidLayer2 );

      /////////////////////////////////////////////////////////
      // System
      /////////////////////////////////////////////////////////
      m_TarcogSystem = make_shared< CTarcogSystem >( aIGU, Indoor, Outdoor );
      ASSERT_TRUE( m_TarcogSystem != nullptr );

      m_TarcogSystem->solve();

    } catch( exception &e ) {
      cout << e.what() << endl;
      throw &e;
    }
  }

public:
  shared_ptr< CTarcogSystem > GetSystem() { return m_TarcogSystem; };
  shared_ptr< CTarIGUSolidLayer > GetSolidLayer1() { return m_SolidLayer1; };
  shared_ptr< CTarIGUSolidLayer > GetSolidLayer2() { return m_SolidLayer2; };

};

TEST_F( TestDoubleClear, Test1 )
{
  try {
    SCOPED_TRACE( "Begin Test: Single Clear - U-value" );
    
    shared_ptr< CTarcogSystem > aSystem = nullptr;
    shared_ptr< CTarIGUSolidLayer > aLayer1 = nullptr;
    shared_ptr< CTarIGUSolidLayer > aLayer2 = nullptr;
    
    aSystem = GetSystem();
    ASSERT_TRUE( aSystem != nullptr );
    aSystem->solve();

    aLayer1 = GetSolidLayer1();

    ASSERT_TRUE( aLayer1 != nullptr );

    shared_ptr< CTarSurface > aSurface = aLayer1->getSurface( SurfacePosition::Front );
    ASSERT_TRUE( aSurface != nullptr );

    double Temperature = aSurface->getTemperature();
    double Radiosity = aSurface->J();

    EXPECT_NEAR( 258.756688, Temperature, 1e-5 );
    EXPECT_NEAR( 251.950834, Radiosity, 1e-5 );

    aSurface = aLayer1->getSurface( SurfacePosition::Back );
    ASSERT_TRUE( aSurface != nullptr );

    Temperature = aSurface->getTemperature();
    Radiosity = aSurface->J();

    EXPECT_NEAR( 259.359226, Temperature, 1e-5 );
    EXPECT_NEAR( 268.667346, Radiosity, 1e-5 );

  } catch( exception &e ) {
    cout << e.what() << endl;
  }

}