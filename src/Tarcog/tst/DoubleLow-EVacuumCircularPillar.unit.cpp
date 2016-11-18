#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "OutdoorEnvironment.hpp"
#include "IndoorEnvironment.hpp"
#include "IGUSolidLayer.hpp"
#include "IGUGapLayer.hpp"
#include "IGU.hpp"
#include "SingleSystem.hpp"
#include "Surface.hpp"
#include "SupportPillar.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;

class DoubleLowEVacuumCircularPillar : public testing::Test {

private:
  shared_ptr< CSingleSystem > m_TarcogSystem;

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

    shared_ptr< CEnvironment > Outdoor = 
      make_shared< COutdoorEnvironment >( airTemperature, pressure, airSpeed, solarRadiation,
        airDirection, tSky, SkyModel::AllSpecified );
    ASSERT_TRUE( Outdoor != nullptr );
    Outdoor->setHCoeffModel( BoundaryConditionsCoeffModel::CalculateH );

    /////////////////////////////////////////////////////////
    // Indoor
    /////////////////////////////////////////////////////////

    double roomTemperature = 294.15;

    shared_ptr< CEnvironment > Indoor = 
      make_shared< CIndoorEnvironment > ( roomTemperature, pressure );
    ASSERT_TRUE( Indoor != nullptr );

    /////////////////////////////////////////////////////////
    // IGU
    /////////////////////////////////////////////////////////
    double solidLayerThickness = 0.004; // [m]
    double solidLayerConductance = 1;
    double TransmittanceIR = 0;
    double emissivityFrontIR = 0.84;
    double emissivityBackIR = 0.036749500781;

    shared_ptr< CBaseIGULayer > aSolidLayer1 = 
      make_shared< CIGUSolidLayer > ( solidLayerThickness, solidLayerConductance,
        emissivityFrontIR, TransmittanceIR, emissivityBackIR, TransmittanceIR );

    solidLayerThickness = 0.003962399904;
    emissivityBackIR = 0.84;

    shared_ptr< CBaseIGULayer > aSolidLayer2 = 
      make_shared< CIGUSolidLayer > ( solidLayerThickness, solidLayerConductance,
        emissivityFrontIR, TransmittanceIR, emissivityBackIR, TransmittanceIR );

    double gapThickness = 0.0001;
    double gapPressure = 0.1333;
    CIGUGapLayer aGapLayer = CIGUGapLayer( gapThickness, gapPressure );

    // Add support pillars
    double pillarConductivity = 999;
    double pillarSpacing = 0.03;
    double pillarRadius = 0.0002;
    shared_ptr< CBaseIGULayer > m_GapLayer = make_shared< CCircularPillar >( aGapLayer, 
      pillarConductivity, pillarSpacing, pillarRadius );

    ASSERT_TRUE( m_GapLayer != nullptr );

    double windowWidth = 1; //[m]
    double windowHeight = 1;
    shared_ptr< CIGU > aIGU = make_shared< CIGU >( windowWidth, windowHeight );
    ASSERT_TRUE( aIGU != nullptr );
    aIGU->addLayer( aSolidLayer1 );
    aIGU->addLayer( m_GapLayer );
    aIGU->addLayer( aSolidLayer2 );

    /////////////////////////////////////////////////////////
    // System
    /////////////////////////////////////////////////////////
    m_TarcogSystem = make_shared< CSingleSystem >( aIGU, Indoor, Outdoor );
    ASSERT_TRUE( m_TarcogSystem != nullptr );

    m_TarcogSystem->solve();
  }

public:
  shared_ptr< CSingleSystem > GetSystem() { return m_TarcogSystem; };

};

TEST_F( DoubleLowEVacuumCircularPillar, Test1 ) {
  SCOPED_TRACE( "Begin Test: Double Low-E - vacuum with circular pillar support" );
  
  shared_ptr< CSingleSystem > aSystem = GetSystem();
  
  ASSERT_TRUE( aSystem != nullptr );

  vector< double > Temperature = *aSystem->getTemperatures();
  vector< double > correctTemperature = { 255.997063, 256.095933, 290.398479, 290.496419 };
  ASSERT_EQ( correctTemperature.size(), Temperature.size() );

  for( auto i = 0u; i < correctTemperature.size(); ++i ) {
    EXPECT_NEAR( correctTemperature[ i ], Temperature[ i ], 1e-5 );
  }

  vector< double > Radiosity = *aSystem->getRadiosities();
  vector< double > correctRadiosity = { 242.987484, 396.293176, 402.108090, 407.071738 };
  ASSERT_EQ( correctRadiosity.size(), Radiosity.size() );

  for( auto i = 0u; i < correctRadiosity.size(); ++i ) {
    EXPECT_NEAR( correctRadiosity[ i ], Radiosity[ i ], 1e-5 );
  }

  size_t numOfIter = aSystem->getNumberOfIterations();
  EXPECT_EQ( 21u, numOfIter );
}
