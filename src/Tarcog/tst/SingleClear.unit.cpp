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

class TestSingleClear : public testing::Test {

private:
  shared_ptr< CBaseIGUTarcogLayer > m_SolidLayer;

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

    shared_ptr< CTarEnvironment > Outdoor = 
      make_shared< CTarOutdoorEnvironment >( airTemperature, pressure, airSpeed, solarRadiation, 
        airDirection, tSky, SkyModel::AllSpecified );
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
    double solidLayerConductance = 1;

    m_SolidLayer = make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance );
    ASSERT_TRUE( m_SolidLayer != nullptr );

    double windowWidth = 1;
    double windowHeight = 1;
    shared_ptr< CTarIGU > aIGU = make_shared< CTarIGU >( windowWidth, windowHeight );
    ASSERT_TRUE( aIGU != nullptr );
    aIGU->addLayer( m_SolidLayer );

    /////////////////////////////////////////////////////////
    // System
    /////////////////////////////////////////////////////////
    shared_ptr< CTarcogSystem >  aTarcogSystem = make_shared< CTarcogSystem >( aIGU, Indoor, Outdoor );
    ASSERT_TRUE( aTarcogSystem != nullptr );

    aTarcogSystem->solve();
  }

public:
  shared_ptr< CBaseIGUTarcogLayer > GetSolidLayer() { return m_SolidLayer; };

};

TEST_F( TestSingleClear, Test1 ) {
  SCOPED_TRACE( "Begin Test: Single Clear - U-value" );

  shared_ptr< CBaseIGUTarcogLayer > aLayer = nullptr;

  aLayer = GetSolidLayer();

  ASSERT_TRUE( aLayer != nullptr );

  shared_ptr< CTarSurface > aSurface = aLayer->getSurface( Side::Front );
  ASSERT_TRUE( aSurface != nullptr );

  double Temperature = aSurface->getTemperature();
  double Radiosity = aSurface->J();

  EXPECT_NEAR( 297.207035, Temperature, 1e-5 );
  EXPECT_NEAR( 432.444546, Radiosity, 1e-5 );

  aSurface = aLayer->getSurface( Side::Back );
  ASSERT_TRUE( aSurface != nullptr );

  Temperature = aSurface->getTemperature();
  Radiosity = aSurface->J();

  EXPECT_NEAR( 297.14470, Temperature, 1e-5 );
  EXPECT_NEAR( 439.201749, Radiosity, 1e-5 );
}