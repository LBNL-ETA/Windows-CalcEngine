#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "TarOutdoorEnvironment.hpp"
#include "TarIndoorEnvironment.hpp"
#include "TarSurface.hpp"
#include "TarIGUSolidLayer.hpp"
#include "TarIGU.hpp"
#include "TarcogSystem.hpp"

using namespace Tarcog;
using namespace std;

class TestOutdoorEnvironmentHCalcSwingbank : public testing::Test {

private:
  shared_ptr< CTarEnvironment > Outdoor;
  shared_ptr< CTarcogSystem > m_TarcogSystem;

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
    double solarRadiation = 0;

    Outdoor = make_shared< CTarOutdoorEnvironment >( airTemperature, pressure, airSpeed, solarRadiation, 
      airDirection, tSky, SkyModel::Swinbank );
    ASSERT_TRUE( Outdoor != nullptr );
    Outdoor->setHCoeffModel( BoundaryConditionsCoeffModel::CalculateH );

    /////////////////////////////////////////////////////////
    // Indoor
    /////////////////////////////////////////////////////////

    double roomTemperature = 294.15;

    shared_ptr< CTarEnvironment > Indoor = make_shared< CTarIndoorEnvironment > ( roomTemperature, pressure );
    ASSERT_TRUE( Indoor != nullptr );

    /////////////////////////////////////////////////////////
    // IGU
    /////////////////////////////////////////////////////////
    double solidLayerThickness = 0.003048; // [m]
    double solidLayerConductance = 100;

    shared_ptr< CTarIGUSolidLayer > aSolidLayer = make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
    ASSERT_TRUE( aSolidLayer != nullptr );

    double windowWidth = 1;
    double windowHeight = 1;
    shared_ptr< CTarIGU > aIGU = make_shared< CTarIGU >( windowWidth, windowHeight );
    ASSERT_TRUE( aIGU != nullptr );
    aIGU->addLayer( aSolidLayer );

    /////////////////////////////////////////////////////////
    // System
    /////////////////////////////////////////////////////////
    m_TarcogSystem = make_shared< CTarcogSystem >( aIGU, Indoor, Outdoor );
    m_TarcogSystem->solve();
    ASSERT_TRUE( m_TarcogSystem != nullptr );
  }

public:
  std::shared_ptr< CTarEnvironment > GetOutdoors() { return Outdoor; };

};

TEST_F( TestOutdoorEnvironmentHCalcSwingbank, CalculateH_Swinbank ) {
  SCOPED_TRACE( "Begin Test: Outdoors -> H model = Calculate; Sky Model = Swinbank" );
  
  shared_ptr< CTarEnvironment > aOutdoor = nullptr;
  
  aOutdoor = GetOutdoors();
  ASSERT_TRUE( aOutdoor != nullptr );

  double radiosity = aOutdoor->getEnvironmentIR();
  EXPECT_NEAR( 423.17235, radiosity, 1e-6 );

  double hc = aOutdoor->getHc();
  EXPECT_NEAR( 26, hc, 1e-6 );

  double outIR = aOutdoor->getRadiationFlow();
  EXPECT_NEAR( 20.7751423, outIR, 1e-6 );

  double outConvection = aOutdoor->getConvectionConductionFlow();
  EXPECT_NEAR( -48.607583, outConvection, 1e-6 );

  double totalHeatFlow = aOutdoor->getHeatFlow();
  EXPECT_NEAR( -27.83244071, totalHeatFlow, 1e-6 );
}