#include <memory>
#include <stdexcept>
#include <gtest\gtest.h>

#include "Gas.hpp"
#include "TarOutdoorEnvironment.hpp"
#include "TarIndoorEnvironment.hpp"
#include "TarIGUSolidLayer.hpp"
#include "TarIGUGapLayer.hpp"
#include "TarBaseShade.hpp"
#include "TarIGU.hpp"
#include "TarcogSystem.hpp"
#include "TarSurface.hpp"

using namespace Tarcog;
using namespace std;

class TestInBetweenShadeAir : public testing::Test
{

private:
  shared_ptr< CTarcogSystem > m_TarcogSystem;
  shared_ptr< CTarIGUSolidLayer > m_Layer1;
  shared_ptr< CTarIGUSolidLayer > m_Layer2;
  shared_ptr< CTarIGUSolidLayer > m_Layer3;

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

      double roomTemperature = 295.15;
      double roomAirSpeed = 0;

      shared_ptr< CTarIndoorEnvironment > Indoor = 
        make_shared< CTarIndoorEnvironment > ( roomTemperature, pressure, roomAirSpeed, airDirection );
      ASSERT_TRUE( Indoor != nullptr );

      /////////////////////////////////////////////////////////
      // IGU
      /////////////////////////////////////////////////////////
      double solidLayerThickness = 0.005715; // [m]
      double solidLayerConductance = 1;

      m_Layer1 = make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
      ASSERT_TRUE( m_Layer1 != nullptr );

      m_Layer3 = make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
      ASSERT_TRUE( m_Layer3 != nullptr );

      double shadeLayerThickness = 0.01;
      double shadeLayerConductance = 160;
      double Atop = 0.1;
      double Abot = 0.1;
      double Aleft = 0.1;
      double Aright = 0.1;
      double Afront = 0.2;

      m_Layer2 = make_shared< CTarIGUShadeLayer >( shadeLayerThickness, shadeLayerConductance,
        make_shared< CShadeOpenings >( Atop, Abot, Aleft, Aright, Afront ) );

      ASSERT_TRUE( m_Layer2 != nullptr );

      double gapThickness = 0.0127;
      double gapPressure = 101325;
      shared_ptr< CTarIGUGapLayer > GapLayer1 = std::make_shared< CTarIGUGapLayer >( gapThickness, gapPressure );
      ASSERT_TRUE( GapLayer1 != nullptr );

      shared_ptr< CTarIGUGapLayer > GapLayer2 = std::make_shared< CTarIGUGapLayer >( gapThickness, gapPressure );
      ASSERT_TRUE( GapLayer2 != nullptr );

      double windowWidth = 1;
      double windowHeight = 1;
      shared_ptr< CTarIGU > aIGU = make_shared< CTarIGU >( windowWidth, windowHeight );
      ASSERT_TRUE( aIGU != nullptr );
      aIGU->addLayer( m_Layer1 );
      aIGU->addLayer( GapLayer1 );
      aIGU->addLayer( m_Layer2 );
      aIGU->addLayer( GapLayer2 );
      aIGU->addLayer( m_Layer3 );

      /////////////////////////////////////////////////////////
      // System
      /////////////////////////////////////////////////////////
      m_TarcogSystem = make_shared< CTarcogSystem >( aIGU, Indoor, Outdoor );
      ASSERT_TRUE( m_TarcogSystem != nullptr );

      m_TarcogSystem->solve();

    } catch( exception &e ) {
      cout << e.what() << endl;
      throw e;
    }
  };

public:
  //shared_ptr< CTarcogSystem > GetSystem() { return m_TarcogSystem; };
  shared_ptr< CTarIGUSolidLayer > GetLayer1() { return m_Layer1; };
  shared_ptr< CTarIGUSolidLayer > GetLayer2() { return m_Layer2; };
  shared_ptr< CTarIGUSolidLayer > GetLayer3() { return m_Layer3; };

};

TEST_F( TestInBetweenShadeAir, Test1 )
{
  try {
    SCOPED_TRACE( "Begin Test: InBetween Shade - Air" );
    
    shared_ptr< CTarIGUSolidLayer > aLayer = nullptr;

    shared_ptr< CTarSurface > aSurface = nullptr;
    
    aLayer = GetLayer1();
    ASSERT_TRUE( aLayer != nullptr );
    aSurface = aLayer->getSurface( SurfacePosition::Front );

    EXPECT_NEAR( 257.90894598978144, aSurface->getTemperature(), 1e-6 );
    EXPECT_NEAR( 249.16661938338754, aSurface->J(), 1e-6 );

    aSurface = aLayer->getSurface( SurfacePosition::Back );

    EXPECT_NEAR( 258.36960655371689, aSurface->getTemperature(), 1e-6 );
    EXPECT_NEAR( 260.32061280963615, aSurface->J(), 1e-6 );

    aLayer = GetLayer2();
    ASSERT_TRUE( aLayer != nullptr );
    aSurface = aLayer->getSurface( SurfacePosition::Front );

    EXPECT_NEAR( 271.53865884320373, aSurface->getTemperature(), 1e-6 );
    EXPECT_NEAR( 300.57156128981001, aSurface->J(), 1e-6 );

    aSurface = aLayer->getSurface( SurfacePosition::Back );

    EXPECT_NEAR( 271.54221847389209, aSurface->getTemperature(), 1e-6 );
    EXPECT_NEAR( 316.33554457280860, aSurface->J(), 1e-6 );

    aLayer = GetLayer3();
    ASSERT_TRUE( aLayer != nullptr );
    aSurface = aLayer->getSurface( SurfacePosition::Front );

    EXPECT_NEAR( 283.61528464773824, aSurface->getTemperature(), 1e-6 );
    EXPECT_NEAR( 358.76065129183996, aSurface->J(), 1e-6 );

    aSurface = aLayer->getSurface( SurfacePosition::Back );

    EXPECT_NEAR( 284.07594521167317, aSurface->getTemperature(), 1e-6 );
    EXPECT_NEAR( 378.99551190053882, aSurface->J(), 1e-6 );

  } catch( exception &e ) {
    cout << e.what() << endl;
  }

}