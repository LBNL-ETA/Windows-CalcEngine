#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "TarOutdoorEnvironment.hpp"
#include "TarIndoorEnvironment.hpp"
#include "TarIGUSolidLayer.hpp"
#include "TarIGUGapLayer.hpp"
#include "TarIGU.hpp"
#include "TarcogSingleSystem.hpp"
#include "TarSurface.hpp"
#include "TarIGUSolidDeflection.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;

class DoubleClearDeflectionTPTest1 : public testing::Test {

private:
  shared_ptr< CTarcogSingleSystem > m_TarcogSystem;

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

    double roomTemperature = 294.15;

    shared_ptr< CTarEnvironment > Indoor = 
      make_shared< CTarIndoorEnvironment > ( roomTemperature, pressure );
    ASSERT_TRUE( Indoor != nullptr );

    /////////////////////////////////////////////////////////
    // IGU
    /////////////////////////////////////////////////////////
    double solidLayerThickness1 = 0.003048; // [m]
    double solidLayerThickness2 = 0.005715;
    double solidLayerConductance = 1;

    shared_ptr< CTarIGUSolidLayer > aSolidLayer1 =
      make_shared< CTarIGUSolidLayer > ( solidLayerThickness1, solidLayerConductance );

    // Introducing non default deflection properties
    double youngsModulus = 8.1e10;
    double poisonRatio = 0.16;
    aSolidLayer1 = make_shared< CTarIGUSolidLayerDeflection >( *aSolidLayer1, youngsModulus, poisonRatio );

    shared_ptr< CBaseIGUTarcogLayer > aSolidLayer2 = 
      make_shared< CTarIGUSolidLayer > ( solidLayerThickness2, solidLayerConductance );

    double gapThickness = 0.0127;
    double gapPressure = 101325;
    shared_ptr< CBaseIGUTarcogLayer > m_GapLayer = make_shared< CTarIGUGapLayer >( gapThickness, gapPressure );
    ASSERT_TRUE( m_GapLayer != nullptr );

    double windowWidth = 1;
    double windowHeight = 1;
    shared_ptr< CTarIGU > aIGU = make_shared< CTarIGU >( windowWidth, windowHeight );
    ASSERT_TRUE( aIGU != nullptr );
    aIGU->addLayer( aSolidLayer1 );
    aIGU->addLayer( m_GapLayer );
    aIGU->addLayer( aSolidLayer2 );

    // Deflection properties
    double Tini = 303.15;
    double Pini = 101325;
    aIGU->setDeflectionProperties( Tini, Pini );

    /////////////////////////////////////////////////////////
    // System
    /////////////////////////////////////////////////////////
    m_TarcogSystem = make_shared< CTarcogSingleSystem >( aIGU, Indoor, Outdoor );
    ASSERT_TRUE( m_TarcogSystem != nullptr );

    m_TarcogSystem->solve();
  }

public:
  shared_ptr< CTarcogSingleSystem > GetSystem() { return m_TarcogSystem; };

};

TEST_F( DoubleClearDeflectionTPTest1, Test1 ) {
  SCOPED_TRACE( "Begin Test: Double Clear - Calculated Deflection" );
  
  shared_ptr< CTarcogSingleSystem > aSystem = nullptr;
  
  aSystem = GetSystem();
  ASSERT_TRUE( aSystem != nullptr );

  vector< shared_ptr< CTarIGUSolidLayer > > aLayers = aSystem->getSolidLayers();

  // First layer
  shared_ptr< CTarSurface > aSurface = aLayers[ 0 ]->getSurface( Side::Front );
  ASSERT_TRUE( aSurface != nullptr );

  double Temperature = aSurface->getTemperature();
  double Radiosity = aSurface->J();
  double MeanDeflection = aSurface->getMeanDeflection();
  double MaxDeflection = aSurface->getMaxDeflection();

  EXPECT_NEAR( 258.811500, Temperature, 1e-5 );
  EXPECT_NEAR( 252.131797, Radiosity, 1e-5 );
  EXPECT_NEAR( -0.0012879, MeanDeflection, 1e-5 );
  EXPECT_NEAR( -0.0030742, MaxDeflection, 1e-5 );

  aSurface = aLayers[ 0 ]->getSurface( Side::Back );
  ASSERT_TRUE( aSurface != nullptr );

  Temperature = aSurface->getTemperature();
  Radiosity = aSurface->J();
  MeanDeflection = aSurface->getMeanDeflection();
  MaxDeflection = aSurface->getMaxDeflection();

  EXPECT_NEAR( 259.137749, Temperature, 1e-5 );
  EXPECT_NEAR( 267.765290, Radiosity, 1e-5 );
  EXPECT_NEAR( -0.0012879, MeanDeflection, 1e-5 );
  EXPECT_NEAR( -0.0030742, MaxDeflection, 1e-5 );

  // Second layer
  aSurface = aLayers[ 1 ]->getSurface( Side::Front );
  ASSERT_TRUE( aSurface != nullptr );

  Temperature = aSurface->getTemperature();
  Radiosity = aSurface->J();
  MeanDeflection = aSurface->getMeanDeflection();
  MaxDeflection = aSurface->getMaxDeflection();

  EXPECT_NEAR( 278.961419, Temperature, 1e-5 );
  EXPECT_NEAR( 331.256183, Radiosity, 1e-5 );
  EXPECT_NEAR( 0.00014072, MeanDeflection, 1e-5 );
  EXPECT_NEAR( 0.00033590, MaxDeflection, 1e-5 );

  aSurface = aLayers[ 1 ]->getSurface( Side::Back );
  ASSERT_TRUE( aSurface != nullptr );

  Temperature = aSurface->getTemperature();
  Radiosity = aSurface->J();
  MeanDeflection = aSurface->getMeanDeflection();
  MaxDeflection = aSurface->getMaxDeflection();

  EXPECT_NEAR( 279.573136, Temperature, 1e-5 );
  EXPECT_NEAR( 358.865247, Radiosity, 1e-5 );
  EXPECT_NEAR( 0.00014072, MeanDeflection, 1e-5 );
  EXPECT_NEAR( 0.00033590, MaxDeflection, 1e-5 );

  size_t numOfIter = aSystem->getNumberOfIterations();
  EXPECT_EQ( 27, int( numOfIter ) );
}
