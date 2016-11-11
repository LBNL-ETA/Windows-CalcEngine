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
#include "FenestrationCommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;

class DoubleClearDeflectionTPDefault : public testing::Test {

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
    double solidLayerThickness1 = 0.003048; // [m]
    double solidLayerThickness2 = 0.005715;
    double solidLayerConductance = 1;

    shared_ptr< CBaseIGULayer > aSolidLayer1 = 
      make_shared< CIGUSolidLayer > ( solidLayerThickness1, solidLayerConductance );

    shared_ptr< CBaseIGULayer > aSolidLayer2 = 
      make_shared< CIGUSolidLayer > ( solidLayerThickness2, solidLayerConductance );

    double gapThickness = 0.0127;
    double gapPressure = 101325;
    shared_ptr< CBaseIGULayer > m_GapLayer = make_shared< CIGUGapLayer >( gapThickness, gapPressure );
    ASSERT_TRUE( m_GapLayer != nullptr );

    double windowWidth = 1;
    double windowHeight = 1;
    shared_ptr< CIGU > aIGU = make_shared< CIGU >( windowWidth, windowHeight );
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
    m_TarcogSystem = make_shared< CSingleSystem >( aIGU, Indoor, Outdoor );
    ASSERT_TRUE( m_TarcogSystem != nullptr );

    m_TarcogSystem->solve();
  }

public:
  shared_ptr< CSingleSystem > GetSystem() { return m_TarcogSystem; };

};

TEST_F( DoubleClearDeflectionTPDefault, Test1 ) {
  SCOPED_TRACE( "Begin Test: Double Clear - Calculated Deflection" );
  
  shared_ptr< CSingleSystem > aSystem = GetSystem();
  ASSERT_TRUE( aSystem != nullptr );

  vector< double > Temperature = *aSystem->getTemperatures();
  vector< double > correctTemperature = { 258.811500, 259.137749, 278.961419, 279.573136 };
  ASSERT_EQ( correctTemperature.size(), Temperature.size() );

  for( auto i = 0; i < correctTemperature.size(); ++i ) {
    EXPECT_NEAR( correctTemperature[ i ], Temperature[ i ], 1e-5 );
  }

  vector< double > Radiosity = *aSystem->getRadiosities();
  vector< double > correctRadiosity = { 252.131797, 267.765290, 331.256183, 358.865247 };
  ASSERT_EQ( correctRadiosity.size(), Radiosity.size() );

  for( auto i = 0; i < correctRadiosity.size(); ++i ) {
    EXPECT_NEAR( correctRadiosity[ i ], Radiosity[ i ], 1e-5 );
  }

  vector< double > MaxDeflection = *aSystem->getMaxDeflections();
  vector< double > correctMaxDeflection = { -0.0031162, 0.00029386 };
  ASSERT_EQ( correctMaxDeflection.size(), MaxDeflection.size() );

  for( auto i = 0; i < correctMaxDeflection.size(); ++i ) {
    EXPECT_NEAR( correctMaxDeflection[ i ], MaxDeflection[ i ], 1e-5 );
  }

  vector< double > MeanDeflection = *aSystem->getMeanDeflections();
  vector< double > correctMeanDeflection = { -0.0013055, 0.00012311 };
  ASSERT_EQ( correctMeanDeflection.size(), MeanDeflection.size() );

  for( auto i = 0; i < correctMaxDeflection.size(); ++i ) {
    EXPECT_NEAR( correctMeanDeflection[ i ], MeanDeflection[ i ], 1e-5 );
  }

  size_t numOfIter = aSystem->getNumberOfIterations();
  EXPECT_EQ( 25, int( numOfIter ) );
}
