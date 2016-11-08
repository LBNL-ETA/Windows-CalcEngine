#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

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
#include "TarcogSingleSystem.hpp"
#include "TarSurface.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace Gases;
using namespace Tarcog;
using namespace FenestrationCommon;

class TestInBetweenShadeAirArgon : public testing::Test {

private:
  shared_ptr< CTarcogSingleSystem > m_TarcogSystem;
  // shared_ptr< CBaseIGUTarcogLayer > m_Layer1;
  // shared_ptr< CBaseIGUTarcogLayer > m_Layer2;
  // shared_ptr< CBaseIGUTarcogLayer > m_Layer3;

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

    shared_ptr< CTarEnvironment > Indoor = 
      make_shared< CTarIndoorEnvironment > ( roomTemperature, pressure );
    ASSERT_TRUE( Indoor != nullptr );

    /////////////////////////////////////////////////////////
    // IGU
    /////////////////////////////////////////////////////////

    // Solid layers
    double solidLayerThickness = 0.005715; // [m]
    double solidLayerConductance = 1;

    shared_ptr< CBaseIGUTarcogLayer > aLayer1 = 
      make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
    ASSERT_TRUE( aLayer1 != nullptr );

    shared_ptr< CBaseIGUTarcogLayer > aLayer3 = 
      make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
    ASSERT_TRUE( aLayer3 != nullptr );

    double shadeLayerThickness = 0.01;
    double shadeLayerConductance = 160;
    double Atop = 0.1;
    double Abot = 0.1;
    double Aleft = 0.1;
    double Aright = 0.1;
    double Afront = 0.2;

    shared_ptr< CBaseIGUTarcogLayer > aLayer2 = make_shared< CTarIGUShadeLayer >( shadeLayerThickness, 
      shadeLayerConductance, make_shared< CShadeOpenings >( Atop, Abot, Aleft, Aright, Afront ) );

    ASSERT_TRUE( aLayer2 != nullptr );

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
    shared_ptr< CBaseIGUTarcogLayer > GapLayer1 = 
      make_shared< CTarIGUGapLayer >( gapThickness, gapPressure, Gas1 );
    ASSERT_TRUE( GapLayer1 != nullptr );

    shared_ptr< CBaseIGUTarcogLayer > GapLayer2 = 
      make_shared< CTarIGUGapLayer >( gapThickness, gapPressure, Gas2 );
    ASSERT_TRUE( GapLayer2 != nullptr );

    double windowWidth = 1;
    double windowHeight = 1;
    shared_ptr< CTarIGU > aIGU = make_shared< CTarIGU >( windowWidth, windowHeight );
    ASSERT_TRUE( aIGU != nullptr );
    aIGU->addLayer( aLayer1 );
    aIGU->addLayer( GapLayer1 );
    aIGU->addLayer( aLayer2 );
    aIGU->addLayer( GapLayer2 );
    aIGU->addLayer( aLayer3 );

    /////////////////////////////////////////////////////////
    // System
    /////////////////////////////////////////////////////////
    m_TarcogSystem = make_shared< CTarcogSingleSystem >( aIGU, Indoor, Outdoor );
    ASSERT_TRUE( m_TarcogSystem != nullptr );

    m_TarcogSystem->solve();
  }

public:
  shared_ptr< CTarcogSingleSystem > GetSystem() { return m_TarcogSystem; };
  shared_ptr< CBaseIGUTarcogLayer > GetLayer1() { return m_TarcogSystem->getSolidLayers()[ 0 ]; };
  shared_ptr< CBaseIGUTarcogLayer > GetLayer2() { return m_TarcogSystem->getSolidLayers()[ 1 ]; };
  shared_ptr< CBaseIGUTarcogLayer > GetLayer3() { return m_TarcogSystem->getSolidLayers()[ 2 ]; };

};

TEST_F( TestInBetweenShadeAirArgon, Test1 ) {
  SCOPED_TRACE( "Begin Test: InBetween Shade - Air(10%)/Argon(90%)" );
  
  shared_ptr< CBaseIGUTarcogLayer > aLayer = nullptr;

  shared_ptr< CTarSurface > aSurface = nullptr;
  
  aLayer = GetLayer1();
  ASSERT_TRUE( aLayer != nullptr );
  aSurface = aLayer->getSurface( Side::Front );

  EXPECT_NEAR( 257.70858602501283, aSurface->getTemperature(), 1e-6 );
  EXPECT_NEAR( 248.51258065870940, aSurface->J(), 1e-6 );

  aSurface = aLayer->getSurface( Side::Back );

  EXPECT_NEAR( 258.13573727149321, aSurface->getTemperature(), 1e-6 );
  EXPECT_NEAR( 259.76235979463536, aSurface->J(), 1e-6 );

  aLayer = GetLayer2();
  ASSERT_TRUE( aLayer != nullptr );
  aSurface = aLayer->getSurface( Side::Front );

  EXPECT_NEAR( 271.90401485639518, aSurface->getTemperature(), 1e-6 );
  EXPECT_NEAR( 301.87856774037959, aSurface->J(), 1e-6 );

  aSurface = aLayer->getSurface( Side::Back );

  EXPECT_NEAR( 271.90745499217974, aSurface->getTemperature(), 1e-6 );
  EXPECT_NEAR( 318.33970629247358, aSurface->J(), 1e-6 );

  aLayer = GetLayer3();
  ASSERT_TRUE( aLayer != nullptr );
  aSurface = aLayer->getSurface( Side::Front );

  EXPECT_NEAR( 284.41284098002097, aSurface->getTemperature(), 1e-6 );
  EXPECT_NEAR( 362.56213463202778, aSurface->J(), 1e-6 );

  aSurface = aLayer->getSurface( Side::Back );

  EXPECT_NEAR( 284.83999222650090, aSurface->getTemperature(), 1e-6 );
  EXPECT_NEAR( 382.34574241337458, aSurface->J(), 1e-6 );

  size_t numOfIter = GetSystem()->getNumberOfIterations();
  EXPECT_EQ( 21, int( numOfIter ) );
}
