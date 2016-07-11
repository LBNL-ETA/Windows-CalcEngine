#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "Gas.hpp"
#include "TarOutdoorEnvironment.hpp"
#include "TarIndoorEnvironment.hpp"
#include "TarIGUSolidLayer.hpp"
#include "TarIGUGapLayer.hpp"
#include "TarBaseShade.hpp"
#include "TarIGU.hpp"
#include "TarcogSystem.hpp"
#include "TarSurface.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;

class TestInBetweenShadeAir : public testing::Test {

private:
  shared_ptr< CTarcogSystem > m_TarcogSystem;
  shared_ptr< CTarIGUSolidLayer > m_Layer1;
  shared_ptr< CTarIGUSolidLayer > m_Layer2;
  shared_ptr< CTarIGUSolidLayer > m_Layer3;

protected:
  virtual void SetUp() {    
    /////////////////////////////////////////////////////////
    // Outdoor
    /////////////////////////////////////////////////////////
    double airTemperature = 255.15; // Kelvins
    double pressure = 101325; // Pascals
    double airSpeed = 5.5; // meters per second
    AirHorizontalDirection airDirection = AirHorizontalDirection::Windward;
    double tSky = 255.15; // Kelvins
    double solarRadiation = 0;

    shared_ptr< CTarEnvironment > Outdoor = 
      make_shared< CTarOutdoorEnvironment >( airTemperature, pressure, airSpeed, solarRadiation, 
        airDirection, tSky, SkyModel::AllSpecified );
    ASSERT_TRUE( Outdoor != nullptr );
    Outdoor->setHCoeffModel( BoundaryConditionsCoeffModel::CalculateH );

    /////////////////////////////////////////////////////////
    // Indoor
    /////////////////////////////////////////////////////////

    double roomTemperature = 295.15;

    shared_ptr< CTarEnvironment > Indoor = make_shared< CTarIndoorEnvironment > ( roomTemperature, pressure );
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
  }

public:
  //shared_ptr< CTarcogSystem > GetSystem() { return m_TarcogSystem; };
  shared_ptr< CTarIGUSolidLayer > GetLayer1() { return m_Layer1; };
  shared_ptr< CTarIGUSolidLayer > GetLayer2() { return m_Layer2; };
  shared_ptr< CTarIGUSolidLayer > GetLayer3() { return m_Layer3; };

};

TEST_F( TestInBetweenShadeAir, Test1 ) {
  SCOPED_TRACE( "Begin Test: InBetween Shade - Air" );
  
  shared_ptr< CTarIGUSolidLayer > aLayer = nullptr;

  shared_ptr< CTarSurface > aSurface = nullptr;
  
  aLayer = GetLayer1();
  ASSERT_TRUE( aLayer != nullptr );
  Side aPosition = Side::Front;
  double temp = aLayer->getTemperature( aPosition );
  double J = aLayer->J( aPosition );

  EXPECT_NEAR( 257.90894598978144, temp, 1e-6 );
  EXPECT_NEAR( 249.16661938338754, J, 1e-6 );

  aPosition = Side::Back;
  temp = aLayer->getTemperature( aPosition );
  J = aLayer->J( aPosition );

  EXPECT_NEAR( 258.36960655371689, temp, 1e-6 );
  EXPECT_NEAR( 260.32061280963615, J, 1e-6 );

  aLayer = GetLayer2();
  ASSERT_TRUE( aLayer != nullptr );
  aPosition = Side::Front;
  temp = aLayer->getTemperature( aPosition );
  J = aLayer->J( aPosition );

  EXPECT_NEAR( 271.53865884320373, temp, 1e-6 );
  EXPECT_NEAR( 300.57156128981001, J, 1e-6 );

  aPosition = Side::Back;
  temp = aLayer->getTemperature( aPosition );
  J = aLayer->J( aPosition );

  EXPECT_NEAR( 271.54221847389209, temp, 1e-6 );
  EXPECT_NEAR( 316.33554457280860, J, 1e-6 );

  aLayer = GetLayer3();
  ASSERT_TRUE( aLayer != nullptr );
  aPosition = Side::Front;
  temp = aLayer->getTemperature( aPosition );
  J = aLayer->J( aPosition );

  EXPECT_NEAR( 283.61528464773824, temp, 1e-6 );
  EXPECT_NEAR( 358.76065129183996, J, 1e-6 );

  aPosition = Side::Back;
  temp = aLayer->getTemperature( aPosition );
  J = aLayer->J( aPosition );

  EXPECT_NEAR( 284.07594521167317, temp, 1e-6 );
  EXPECT_NEAR( 378.99551190053882, J, 1e-6 );
}