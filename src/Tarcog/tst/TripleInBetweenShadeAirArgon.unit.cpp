#include <memory>
#include <stdexcept>
#include <gtest\gtest.h>

#include "GasData.hpp"
#include "GasItem.hpp"
#include "GasProperties.hpp"
#include "Gas.hpp"
#include "TarOutdoorEnvironment.hpp"
#include "TarIndoorEnvironment.hpp"
#include "TarIGUSolidLayer.hpp"
#include "TarIGUGapLayer.hpp"
#include "TarBaseShade.hpp"
#include "TarIGU.hpp"
#include "TarcogSystem.hpp"
#include "TarSurface.hpp"

using namespace std;
using namespace Gasses;
using namespace Tarcog;

class TestInBetweenShadeAirArgon : public testing::Test
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

      // Solid layers
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

      // gap layers

      // Create coefficients for Air
      shared_ptr< CIntCoeff > AirCon = make_shared< CIntCoeff >( 2.8733e-03, 7.76e-05, 0.0 );
      shared_ptr< CIntCoeff > AirCp = make_shared< CIntCoeff >( 1.002737e+03, 1.2324e-02, 0.0 );
      shared_ptr< CIntCoeff > AirVisc = make_shared< CIntCoeff >( 3.7233e-06, 4.94e-08, 0.0 );

      shared_ptr< CGasData > AirData = make_shared< CGasData >( "Air", 28.97, 1.4, AirCp, AirCon, AirVisc );

      // Create coefficients for Argon
      shared_ptr< CIntCoeff > ArgonCon = make_shared< CIntCoeff >( 2.2848e-03, 5.1486e-05, 0.0 );
      shared_ptr< CIntCoeff > ArgonCp = make_shared< CIntCoeff >( 5.21929e+02, 0.0, 0.0 );
      shared_ptr< CIntCoeff > ArgonVisc = make_shared< CIntCoeff >( 3.3786e-06, 6.4514e-08, 0.0 );

      shared_ptr< CGasData > ArgonData = make_shared< CGasData >( "Argon", 39.948, 1.67, ArgonCp, ArgonCon, ArgonVisc );

      shared_ptr< CGasItem > Air = make_shared< CGasItem >( 0.1, AirData );
      shared_ptr< CGasItem > Argon = make_shared< CGasItem >( 0.9, ArgonData );

      // Create gas mixture
      shared_ptr< CGas > Gas1 = make_shared< CGas >();

      Gas1->addGasItem( Air );
      Gas1->addGasItem( Argon );

      shared_ptr< CGas > Gas2 = std::make_shared< CGas >();
      ( *Gas2 ) = ( *Gas1 );

      double gapThickness = 0.0127;
      double gapPressure = 101325;
      shared_ptr< CTarIGUGapLayer > GapLayer1 = make_shared< CTarIGUGapLayer >( gapThickness, gapPressure, Gas1 );
      ASSERT_TRUE( GapLayer1 != nullptr );

      shared_ptr< CTarIGUGapLayer > GapLayer2 = make_shared< CTarIGUGapLayer >( gapThickness, gapPressure, Gas2 );
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

TEST_F( TestInBetweenShadeAirArgon, Test1 )
{
  try {
    SCOPED_TRACE( "Begin Test: InBetween Shade - Air(10%)/Argon(90%)" );
    
    shared_ptr< CTarIGUSolidLayer > aLayer = nullptr;

    shared_ptr< CTarSurface > aSurface = nullptr;
    
    aLayer = GetLayer1();
    ASSERT_TRUE( aLayer != nullptr );
    aSurface = aLayer->getSurface( SurfacePosition::Front );

    EXPECT_NEAR( 257.70807679264135, aSurface->getTemperature(), 1e-6 );
    EXPECT_NEAR( 248.51092039068794, aSurface->J(), 1e-6 );

    aSurface = aLayer->getSurface( SurfacePosition::Back );

    EXPECT_NEAR( 258.13514288283039, aSurface->getTemperature(), 1e-6 );
    EXPECT_NEAR( 259.76088456656476, aSurface->J(), 1e-6 );

    aLayer = GetLayer2();
    ASSERT_TRUE( aLayer != nullptr );
    aSurface = aLayer->getSurface( SurfacePosition::Front );

    EXPECT_NEAR( 271.90484753651458, aSurface->getTemperature(), 1e-6 );
    EXPECT_NEAR( 301.88152030138281, aSurface->J(), 1e-6 );

    aSurface = aLayer->getSurface( SurfacePosition::Back );

    EXPECT_NEAR( 271.90828841323201, aSurface->getTemperature(), 1e-6 );
    EXPECT_NEAR( 318.34444233867350, aSurface->J(), 1e-6 );

    aLayer = GetLayer3();
    ASSERT_TRUE( aLayer != nullptr );
    aSurface = aLayer->getSurface( SurfacePosition::Front );

    EXPECT_NEAR( 284.41487117900050, aSurface->getTemperature(), 1e-6 );
    EXPECT_NEAR( 362.57178896039937, aSurface->J(), 1e-6 );

    aSurface = aLayer->getSurface( SurfacePosition::Back );

    EXPECT_NEAR( 284.84193726918704, aSurface->getTemperature(), 1e-6 );
    EXPECT_NEAR( 382.35430412032650, aSurface->J(), 1e-6 );

  } catch( exception &e ) {
    cout << e.what() << endl;
  }

}