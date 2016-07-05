#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "TarOutdoorEnvironment.hpp"
#include "TarIndoorEnvironment.hpp"
#include "TarIGUSolidLayer.hpp"
#include "TarBaseShade.hpp"
#include "TarIGUGapLayer.hpp"
#include "TarIGU.hpp"
#include "TarcogSystem.hpp"
#include "TarSurface.hpp"

using namespace Tarcog;
using namespace std;

class TestShadeOut : public testing::Test {

private:
  shared_ptr< CTarIGUSolidLayer > m_SolidLayer1;
  shared_ptr< CTarIGUSolidLayer > m_SolidLayer2;
  shared_ptr< CTarcogSystem > m_TarcogSystem;

protected:
  virtual void SetUp() {
    try {
      /////////////////////////////////////////////////////////
      // Outdoor
      /////////////////////////////////////////////////////////
      double airTemperature = 255.15; // Kelvins
      double pressure = 101325; // Pascals
      double airSpeed = 5.5; // meters per second
      AirHorizontalDirection airDirection = AirHorizontalDirection::Windward;
      double tSky = 255.15; // Kelvins
      double solarRadiation = 0;

      shared_ptr< CTarOutdoorEnvironment > Outdoor = 
        make_shared< CTarOutdoorEnvironment >( airTemperature, pressure, airSpeed, solarRadiation, 
          airDirection, tSky, SkyModel::AllSpecified );
      ASSERT_TRUE( Outdoor != nullptr );
      Outdoor->setHCoeffModel( BoundaryConditionsCoeffModel::CalculateH );

      /////////////////////////////////////////////////////////
      // Indoor
      /////////////////////////////////////////////////////////

      double roomTemperature = 294.15;

      shared_ptr< CTarIndoorEnvironment > Indoor = make_shared< CTarIndoorEnvironment > ( roomTemperature, pressure );
      ASSERT_TRUE( Indoor != nullptr );

      /////////////////////////////////////////////////////////
      // IGU
      /////////////////////////////////////////////////////////
      double emissivity = 0.832855582237;
      double transmittance = 0.074604861438;
      shared_ptr< CTarSurface > surface1 = make_shared< CTarSurface > ( emissivity, transmittance );
      shared_ptr< CTarSurface > surface2 = make_shared< CTarSurface > ( emissivity, transmittance );

      double shadeLayerThickness = 0.0006;
      double shadeLayerConductance = 160;
      double Atop = 0.0;
      double Abot = 0.0;
      double Aleft = 0.0;
      double Aright = 0.0;
      double Afront = 0.5;

      m_SolidLayer1 = make_shared< CTarIGUShadeLayer >( shadeLayerThickness, shadeLayerConductance,
        make_shared< CShadeOpenings >( Atop, Abot, Aleft, Aright, Afront ), surface1, surface2 );

      ASSERT_TRUE( m_SolidLayer1 != nullptr );

      double solidLayerThickness = 0.0056134; // [m]
      double solidLayerConductance = 1;
      double emissivity1 = 0.84;
      double emissivity2 = 0.038798544556;
      transmittance = 0.0;

      shared_ptr< CTarSurface > surface3 = make_shared< CTarSurface > ( emissivity1, transmittance );
      shared_ptr< CTarSurface > surface4 = make_shared< CTarSurface > ( emissivity2, transmittance );

      m_SolidLayer2 = make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance, surface3, surface4 );
      ASSERT_TRUE( m_SolidLayer2 != nullptr );

      double gapThickness = 0.0127;
      double gapPressure = 101325;
      shared_ptr< CTarIGUGapLayer > m_GapLayer = make_shared< CTarIGUGapLayer >( gapThickness, gapPressure );
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

TEST_F( TestShadeOut, Test1 ) {
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

    EXPECT_NEAR( 256.99192369743673, Temperature, 1e-5 );
    EXPECT_NEAR( 249.99304229996574, Radiosity, 1e-5 );

    aSurface = aLayer1->getSurface( SurfacePosition::Back );
    ASSERT_TRUE( aSurface != nullptr );

    Temperature = aSurface->getTemperature();
    Radiosity = aSurface->J();

    EXPECT_NEAR( 256.99213965756041, Temperature, 1e-5 );
    EXPECT_NEAR( 250.92106915533645, Radiosity, 1e-5 );

  } catch( exception &e ) {
    cout << e.what() << endl;
  }

}