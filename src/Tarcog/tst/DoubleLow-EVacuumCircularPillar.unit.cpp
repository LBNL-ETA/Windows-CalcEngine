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
#include "SupportPillar.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;

class DoubleLowEVacuumCircularPillar : public testing::Test {

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
    double solidLayerThickness = 0.004; // [m]
    double solidLayerConductance = 1;
    double TransmittanceIR = 0;
    double emissivityFrontIR = 0.84;
    double emissivityBackIR = 0.036749500781;

    shared_ptr< CBaseIGUTarcogLayer > aSolidLayer1 = 
      make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance,
        emissivityFrontIR, TransmittanceIR, emissivityBackIR, TransmittanceIR );

    solidLayerThickness = 0.003962399904;
    emissivityBackIR = 0.84;

    shared_ptr< CBaseIGUTarcogLayer > aSolidLayer2 = 
      make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance,
        emissivityFrontIR, TransmittanceIR, emissivityBackIR, TransmittanceIR );

    double gapThickness = 0.0001;
    double gapPressure = 0.1333;
    CTarIGUGapLayer aGapLayer = CTarIGUGapLayer( gapThickness, gapPressure );

    // Add support pillars
    double pillarConductivity = 999;
    double pillarSpacing = 0.03;
    double pillarRadius = 0.0002;
    shared_ptr< CBaseIGUTarcogLayer > m_GapLayer = make_shared< CCircularPillar >( aGapLayer, 
      pillarConductivity, pillarSpacing, pillarRadius );

    ASSERT_TRUE( m_GapLayer != nullptr );

    double windowWidth = 1; //[m]
    double windowHeight = 1;
    shared_ptr< CTarIGU > aIGU = make_shared< CTarIGU >( windowWidth, windowHeight );
    ASSERT_TRUE( aIGU != nullptr );
    aIGU->addLayer( aSolidLayer1 );
    aIGU->addLayer( m_GapLayer );
    aIGU->addLayer( aSolidLayer2 );

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

TEST_F( DoubleLowEVacuumCircularPillar, Test1 ) {
  SCOPED_TRACE( "Begin Test: Double Low-E - vacuum with circular pillar support" );
  
  shared_ptr< CTarcogSystem > aSystem = nullptr;
  
  aSystem = GetSystem();
  ASSERT_TRUE( aSystem != nullptr );

  vector< shared_ptr< CTarIGUSolidLayer > > aLayers = aSystem->getSolidLayers();

  // First layer
  shared_ptr< CTarSurface > aSurface = aLayers[ 0 ]->getSurface( Side::Front );
  ASSERT_TRUE( aSurface != nullptr );

  double Temperature = aSurface->getTemperature();
  double Radiosity = aSurface->J();

  EXPECT_NEAR( 255.997063, Temperature, 1e-5 );
  EXPECT_NEAR( 242.987484, Radiosity, 1e-5 );

  aSurface = aLayers[ 0 ]->getSurface( Side::Back );
  ASSERT_TRUE( aSurface != nullptr );

  Temperature = aSurface->getTemperature();
  Radiosity = aSurface->J();

  EXPECT_NEAR( 256.095933, Temperature, 1e-5 );
  EXPECT_NEAR( 396.293176, Radiosity, 1e-5 );

  // Second layer
  aSurface = aLayers[ 1 ]->getSurface( Side::Front );
  ASSERT_TRUE( aSurface != nullptr );

  Temperature = aSurface->getTemperature();
  Radiosity = aSurface->J();

  EXPECT_NEAR( 290.398479, Temperature, 1e-5 );
  EXPECT_NEAR( 402.108090, Radiosity, 1e-5 );

  aSurface = aLayers[ 1 ]->getSurface( Side::Back );
  ASSERT_TRUE( aSurface != nullptr );

  Temperature = aSurface->getTemperature();
  Radiosity = aSurface->J();

  EXPECT_NEAR( 290.496419, Temperature, 1e-5 );
  EXPECT_NEAR( 407.071738, Radiosity, 1e-5 );

  size_t numOfIter = aSystem->getNumberOfIterations();
  EXPECT_EQ( 21, int( numOfIter ) );
}
