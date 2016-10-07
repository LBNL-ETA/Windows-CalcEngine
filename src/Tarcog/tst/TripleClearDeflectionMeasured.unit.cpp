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
#include "FenestrationCommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;

class TripleClearDeflectionMeasured : public testing::Test {

private:
  shared_ptr< CTarcogSystem > m_TarcogSystem;

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

    shared_ptr< CBaseIGUTarcogLayer > aSolidLayer1 = 
      make_shared< CTarIGUSolidLayer > ( solidLayerThickness1, solidLayerConductance );

    shared_ptr< CBaseIGUTarcogLayer > aSolidLayer2 = 
      make_shared< CTarIGUSolidLayer > ( solidLayerThickness2, solidLayerConductance );

    shared_ptr< CBaseIGUTarcogLayer > aSolidLayer3 =
      make_shared< CTarIGUSolidLayer >( solidLayerThickness1, solidLayerConductance );

    double gapThickness = 0.0127;
    double gapPressure = 101325;
    shared_ptr< CBaseIGUTarcogLayer > aGapLayer1 = make_shared< CTarIGUGapLayer >( gapThickness, gapPressure );
    ASSERT_TRUE( aGapLayer1 != nullptr );

    shared_ptr< CBaseIGUTarcogLayer > aGapLayer2 = make_shared< CTarIGUGapLayer >( gapThickness, gapPressure );
    ASSERT_TRUE( aGapLayer2 != nullptr );

    double windowWidth = 1;
    double windowHeight = 1;
    shared_ptr< CTarIGU > aIGU = make_shared< CTarIGU >( windowWidth, windowHeight );
    ASSERT_TRUE( aIGU != nullptr );
    aIGU->addLayer( aSolidLayer1 );
    aIGU->addLayer( aGapLayer1 );
    aIGU->addLayer( aSolidLayer2 );
    aIGU->addLayer( aGapLayer2 );
    aIGU->addLayer( aSolidLayer3 );

    // Deflection properties
    vector< double > measuredGapsWidths = { 0.0135, 0.013 };
    aIGU->setDeflectionProperties( measuredGapsWidths );

    /////////////////////////////////////////////////////////
    // System
    /////////////////////////////////////////////////////////
    m_TarcogSystem = make_shared< CTarcogSystem >( aIGU, Indoor, Outdoor );
    ASSERT_TRUE( m_TarcogSystem != nullptr );

    m_TarcogSystem->solve();
  }

public:
  shared_ptr< CTarcogSystem > GetSystem() { return m_TarcogSystem; };

};

TEST_F( TripleClearDeflectionMeasured, Test1 ) {
  SCOPED_TRACE( "Begin Test: Triple Clear - Measured Deflection." );
  
  shared_ptr< CTarcogSystem > aSystem = nullptr;
  
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

  EXPECT_NEAR( 257.493976, Temperature, 1e-5 );
  EXPECT_NEAR( 247.813715, Radiosity, 1e-5 );
  EXPECT_NEAR( 0.00031076, MeanDeflection, 1e-5 );
  EXPECT_NEAR( 0.00074180, MaxDeflection, 1e-5 );

  aSurface = aLayers[ 0 ]->getSurface( Side::Back );
  ASSERT_TRUE( aSurface != nullptr );

  Temperature = aSurface->getTemperature();
  Radiosity = aSurface->J();
  MeanDeflection = aSurface->getMeanDeflection();
  MaxDeflection = aSurface->getMaxDeflection();

  EXPECT_NEAR( 257.702652, Temperature, 1e-5 );
  EXPECT_NEAR( 258.078374, Radiosity, 1e-5 );
  EXPECT_NEAR( 0.00031076, MeanDeflection, 1e-5 );
  EXPECT_NEAR( 0.00074180, MaxDeflection, 1e-5 );

  // Second layer
  aSurface = aLayers[ 1 ]->getSurface( Side::Front );
  ASSERT_TRUE( aSurface != nullptr );

  Temperature = aSurface->getTemperature();
  Radiosity = aSurface->J();
  MeanDeflection = aSurface->getMeanDeflection();
  MaxDeflection = aSurface->getMaxDeflection();

  EXPECT_NEAR( 271.535517, Temperature, 1e-5 );
  EXPECT_NEAR( 300.200818, Radiosity, 1e-5 );
  EXPECT_NEAR( -2.437e-05, MeanDeflection, 1e-7 );
  EXPECT_NEAR( -5.820e-05, MaxDeflection, 1e-7 );

  aSurface = aLayers[ 1 ]->getSurface( Side::Back );
  ASSERT_TRUE( aSurface != nullptr );

  Temperature = aSurface->getTemperature();
  Radiosity = aSurface->J();
  MeanDeflection = aSurface->getMeanDeflection();
  MaxDeflection = aSurface->getMaxDeflection();

  EXPECT_NEAR( 271.926785, Temperature, 1e-5 );
  EXPECT_NEAR( 318.403140, Radiosity, 1e-5 );
  EXPECT_NEAR( -2.437e-05, MeanDeflection, 1e-7 );
  EXPECT_NEAR( -5.820e-05, MaxDeflection, 1e-7 );

  // Third layer
  aSurface = aLayers[ 2 ]->getSurface( Side::Front );
  ASSERT_TRUE( aSurface != nullptr );

  Temperature = aSurface->getTemperature();
  Radiosity = aSurface->J();
  MeanDeflection = aSurface->getMeanDeflection();
  MaxDeflection = aSurface->getMaxDeflection();

  EXPECT_NEAR( 284.395405, Temperature, 1e-5 );
  EXPECT_NEAR( 362.495875, Radiosity, 1e-5 );
  EXPECT_NEAR( -0.0001501, MeanDeflection, 1e-5 );
  EXPECT_NEAR( -0.0003582, MaxDeflection, 1e-5 );

  aSurface = aLayers[ 2 ]->getSurface( Side::Back );
  ASSERT_TRUE( aSurface != nullptr );

  Temperature = aSurface->getTemperature();
  Radiosity = aSurface->J();
  MeanDeflection = aSurface->getMeanDeflection();
  MaxDeflection = aSurface->getMaxDeflection();

  EXPECT_NEAR( 284.604082, Temperature, 1e-5 );
  EXPECT_NEAR( 380.380188, Radiosity, 1e-5 );
  EXPECT_NEAR( -0.0001501, MeanDeflection, 1e-5 );
  EXPECT_NEAR( -0.0003582, MaxDeflection, 1e-5 );

  size_t numOfIter = aSystem->getNumberOfIterations();
  EXPECT_EQ( 20, int( numOfIter ) );
}
