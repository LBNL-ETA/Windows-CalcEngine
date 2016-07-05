#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "TarOutdoorEnvironment.hpp"
#include "TarIndoorEnvironment.hpp"
#include "TarIGUSolidLayer.hpp"
#include "TarIGU.hpp"
#include "TarcogSystem.hpp"
#include "TarSurface.hpp"

using namespace Tarcog;
using namespace std;

class TestSingleClearSolar : public testing::Test {

private:
  shared_ptr< CTarIGUSolidLayer > m_SolidLayer;
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
      double solarRadiation = 1000;

      shared_ptr< CTarEnvironment > Outdoor = 
        make_shared< CTarOutdoorEnvironment >( airTemperature, pressure, airSpeed, solarRadiation, 
          airDirection, tSky, SkyModel::AllSpecified );
      ASSERT_TRUE( Outdoor != nullptr );
      Outdoor->setHCoeffModel( BoundaryConditionsCoeffModel::CalculateH );

      /////////////////////////////////////////////////////////
      // Indoor
      /////////////////////////////////////////////////////////

      double roomTemperature = 294.15;

      shared_ptr< CTarEnvironment > Indoor = make_shared< CTarIndoorEnvironment > ( roomTemperature, pressure );
      ASSERT_TRUE( Indoor != nullptr );

      /////////////////////////////////////////////////////////
      // IGU
      /////////////////////////////////////////////////////////
      double solidLayerThickness = 0.003048; // [m]
      double solidLayerConductance = 100;
      double solarAbsorptance = 0.094189159572;

      m_SolidLayer = make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
      m_SolidLayer->setSolarAbsorptance( solarAbsorptance );
      ASSERT_TRUE( m_SolidLayer != nullptr );

      double windowWidth = 1;
      double windowHeight = 1;
      shared_ptr< CTarIGU > aIGU = make_shared< CTarIGU >( windowWidth, windowHeight );
      ASSERT_TRUE( aIGU != nullptr );
      aIGU->addLayer( m_SolidLayer );

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
  shared_ptr< CTarIGUSolidLayer > GetSolidLayer() { return m_SolidLayer; };

};

TEST_F( TestSingleClearSolar, Test1 ) {
  SCOPED_TRACE( "Begin Test: Single Clear (Solar Radiation) - U-value" );
  
  shared_ptr< CTarcogSystem > aSystem = nullptr;
  shared_ptr< CTarIGUSolidLayer > aLayer = nullptr;
  
  aSystem = GetSystem();
  ASSERT_TRUE( aSystem != nullptr );
  aSystem->solve();

  aLayer = GetSolidLayer();

  ASSERT_TRUE( aLayer != nullptr );

  SurfacePosition aPosition = SurfacePosition::Front;
  double Temperature = aLayer->getTemperature( aPosition );
  double Radiosity = aLayer->J( aPosition );

  EXPECT_NEAR( 299.627742, Temperature, 1e-5 );
  EXPECT_NEAR( 444.699763, Radiosity, 1e-5 );

  aPosition = SurfacePosition::Back;
  Temperature = aLayer->getTemperature( aPosition );
  Radiosity = aLayer->J( aPosition );

  EXPECT_NEAR( 299.627975, Temperature, 1e-5 );
  EXPECT_NEAR( 451.769813, Radiosity, 1e-5 );
}