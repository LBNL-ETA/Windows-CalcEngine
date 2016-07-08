#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "TarOutdoorEnvironment.hpp"
#include "TarIndoorEnvironment.hpp"
#include "TarIGUSolidLayer.hpp"
#include "TarIGU.hpp"
#include "TarcogSystem.hpp"
#include "TarSurface.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace Tarcog;
using namespace FenestrationCommon;

class TestSingleClearSolarCond100 : public testing::Test {

private:
  shared_ptr< CTarIGUSolidLayer > m_SolidLayer;
  shared_ptr< CTarcogSystem > m_TarcogSystem;
  shared_ptr< CTarEnvironment > m_Indoor;

protected:
  virtual void SetUp() {
    /////////////////////////////////////////////////////////
    // Outdoor
    /////////////////////////////////////////////////////////
    double airTemperature = 300; // Kelvins
    double pressure = 101325; // Pascals
    double airSpeed = 5.5; // meters per second
    AirHorizontalDirection airDirection = AirHorizontalDirection::Windward;
    double tSky = 270; // Kelvins
    double solarRadiation = 1000;

    shared_ptr< CTarEnvironment > Outdoor = 
      make_shared< CTarOutdoorEnvironment >( airTemperature, pressure, airSpeed, solarRadiation, 
        airDirection, tSky, SkyModel::AllSpecified );
    ASSERT_TRUE( Outdoor != nullptr );
    Outdoor->setHCoeffModel( BoundaryConditionsCoeffModel::CalculateH );

    /////////////////////////////////////////////////////////
    // Indoor
    /////////////////////////////////////////////////////////
    double roomTemperature = 294.15;
    m_Indoor = make_shared< CTarIndoorEnvironment > ( roomTemperature, pressure );
    ASSERT_TRUE( m_Indoor != nullptr );

    /////////////////////////////////////////////////////////
    // IGU
    /////////////////////////////////////////////////////////
    double solidLayerThickness = 0.003048; // [m]
    double solidLayerConductance = 0.01;
    double solarAbsorptance = 0.094189159572;

    m_SolidLayer = make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
    m_SolidLayer->setSolarAbsorptance( solarAbsorptance );
    ASSERT_TRUE( m_SolidLayer != nullptr );

    double windowWidth = 1;
    double windowHeight = 1;
    shared_ptr< CTarIGU > aIGU = make_shared< CTarIGU >( windowWidth, windowHeight );
    ASSERT_TRUE( aIGU != nullptr );
    aIGU->addLayer( m_SolidLayer );

    /////////////////////////////////////////////////////////
    // System
    /////////////////////////////////////////////////////////
    m_TarcogSystem = make_shared< CTarcogSystem >( aIGU, m_Indoor, Outdoor );
    ASSERT_TRUE( m_TarcogSystem != nullptr );

    m_TarcogSystem->solve();
  }

public:
  shared_ptr< CTarcogSystem > GetSystem() { return m_TarcogSystem; };
  shared_ptr< CTarIGUSolidLayer > GetSolidLayer() { return m_SolidLayer; };
  shared_ptr< CTarEnvironment > GetIndoor() { return m_Indoor; };

};

TEST_F( TestSingleClearSolarCond100, TestTempAndRad ) {
  SCOPED_TRACE( "Begin Test: Single Clear (Solar Radiation) - Temperatures and Radiosity." );
  
  shared_ptr< CTarcogSystem > aSystem = nullptr;
  shared_ptr< CTarIGUSolidLayer > aLayer = nullptr;
  
  aSystem = GetSystem();
  ASSERT_TRUE( aSystem != nullptr );

  aLayer = GetSolidLayer();

  ASSERT_TRUE( aLayer != nullptr );

  Side aPosition = Side::Front;
  double Temperature = aLayer->getTemperature( aPosition );
  double Radiosity = aLayer->J( aPosition );

  EXPECT_NEAR( 299.465898, Temperature, 1e-5 );
  EXPECT_NEAR( 443.871080, Radiosity, 1e-5 );

  aPosition = Side::Back;
  Temperature = aLayer->getTemperature( aPosition );
  Radiosity = aLayer->J( aPosition );

  EXPECT_NEAR( 300.261869, Temperature, 1e-5 );
  EXPECT_NEAR( 455.028488, Radiosity, 1e-5 );
}

TEST_F( TestSingleClearSolarCond100, TestIndoor ) {
  SCOPED_TRACE( "Begin Test: Single Clear (Solar Radiation) - Indoor heat flow." );

  shared_ptr< CTarEnvironment > aIndoor = nullptr;

  aIndoor = GetIndoor();
  ASSERT_TRUE( aIndoor != nullptr );

  double convectiveHF = aIndoor->getConvectionConductionFlow();
  double radiativeHF = aIndoor->getRadiationFlow();
  double totalHF = aIndoor->getHeatFlow();

  EXPECT_NEAR( -13.913388, convectiveHF, 1e-5 );
  EXPECT_NEAR( -30.569739, radiativeHF, 1e-5 );
  EXPECT_NEAR( -44.483127, totalHF, 1e-5 );
}