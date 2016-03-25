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

class TestOutdoorEnvironment : public testing::Test
{

private:
  shared_ptr< CTarOutdoorEnvironment > Outdoor;
  shared_ptr< CTarcogSystem > m_TarcogSystem;

protected:
  virtual void SetUp()
  {
    try {
    /////////////////////////////////////////////////////////
    // Outdoor
    /////////////////////////////////////////////////////////
    double airTemperature = 300; // Kelvins
    double pressure = 101325; // Pascals
    double airSpeed = 5.5; // meters per second
    AirHorizontalDirection airDirection = AirHorizontalDirection::Windward;
    double tSky = 270; // Kelvins

    Outdoor = make_shared< CTarOutdoorEnvironment >( airTemperature, pressure, airSpeed, airDirection );
    ASSERT_TRUE( Outdoor != nullptr );
    Outdoor->setSkyTemperature( tSky );
    Outdoor->setHCoeffModel( BoundaryConditionsCoeffModel::CalculateH );
    Outdoor->setSkyModel( SkyModel::AllSpecified );

    /////////////////////////////////////////////////////////
    // Indoor
    /////////////////////////////////////////////////////////

    double roomTemperature = 294.15;
    double roomAirSpeed = 0;

    shared_ptr< CTarIndoorEnvironment > Indoor = 
      make_shared< CTarIndoorEnvironment > ( roomTemperature, pressure, roomAirSpeed, airDirection );
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
    ASSERT_TRUE( m_TarcogSystem != nullptr );
    } catch( exception &e ) {
      cout << e.what() << endl;
      throw e;
    }
  };

public:
  std::shared_ptr< CTarOutdoorEnvironment > GetOutdoors() { return Outdoor; };

};

TEST_F( TestOutdoorEnvironment, CalculateH_AllSpecified )
{
  try {
    SCOPED_TRACE( "Begin Test: Outdoors -> H model = Calculate; Sky Model = All Specified" );
    
    std::shared_ptr< CTarOutdoorEnvironment > aOutdoor = nullptr;
    
    aOutdoor = GetOutdoors();
    ASSERT_TRUE( aOutdoor != nullptr );

    aOutdoor->setHCoeffModel( BoundaryConditionsCoeffModel::CalculateH );
    aOutdoor->setSkyModel( SkyModel::AllSpecified );

    double radiosity = aOutdoor->getEnvironmentRadiosity();

    EXPECT_NEAR( 380.278401885, radiosity, 1e-6 );
  } catch( exception &e ) {
    cout << e.what() << endl;
  }

}

TEST_F( TestOutdoorEnvironment, CalculateH_TSkySpecified )
{
  try {
    SCOPED_TRACE( "Begin Test: Outdoors -> H model = Calculate; Sky Model = TSky Specified" );
    
    std::shared_ptr< CTarOutdoorEnvironment > aOutdoor = nullptr;
    
    aOutdoor = GetOutdoors();
    ASSERT_TRUE( aOutdoor != nullptr );

    aOutdoor->setHCoeffModel( BoundaryConditionsCoeffModel::CalculateH );
    aOutdoor->setSkyModel( SkyModel::TSkySpecified );

    double radiosity = aOutdoor->getEnvironmentRadiosity();

    EXPECT_NEAR( 380.278401885, radiosity, 1e-6 );
  } catch( exception &e ) {
    cout << e.what() << endl;
  }

}

TEST_F( TestOutdoorEnvironment, CalculateH_Swinbank )
{
  try {
    SCOPED_TRACE( "Begin Test: Outdoors -> H model = Calculate; Sky Model = Swinbank" );
    
    shared_ptr< CTarOutdoorEnvironment > aOutdoor = nullptr;
    
    aOutdoor = GetOutdoors();
    ASSERT_TRUE( aOutdoor != nullptr );

    aOutdoor->setHCoeffModel( BoundaryConditionsCoeffModel::CalculateH );
    aOutdoor->setSkyModel( SkyModel::Swinbank );

    double radiosity = aOutdoor->getEnvironmentRadiosity();

    EXPECT_NEAR( 423.17235, radiosity, 1e-6 );
  } catch( exception &e ) {
    cout << e.what() << endl;
  }

}

TEST_F( TestOutdoorEnvironment, HPrescribed_AllSpecified )
{
  try {
    SCOPED_TRACE( "Begin Test: Outdoors -> H model = HPrescribed; Sky Model = All Specified" );
    
    shared_ptr< CTarOutdoorEnvironment > aOutdoor = nullptr;
    
    aOutdoor = GetOutdoors();
    ASSERT_TRUE( aOutdoor != nullptr );

    aOutdoor->setHCoeffModel( BoundaryConditionsCoeffModel::HPrescribed );
    aOutdoor->setSkyModel( SkyModel::AllSpecified );

    double radiosity = aOutdoor->getEnvironmentRadiosity();

    EXPECT_NEAR( 459.2457, radiosity, 1e-6 );
  } catch( exception &e ) {
    cout << e.what() << endl;
  }

}

TEST_F( TestOutdoorEnvironment, HPrescribed_TSkySpecified )
{
  try {
    SCOPED_TRACE( "Begin Test: Outdoors -> H model = HPrescribed; Sky Model = TSky Specified" );
    
    shared_ptr< CTarOutdoorEnvironment > aOutdoor = nullptr;
    
    aOutdoor = GetOutdoors();
    ASSERT_TRUE( aOutdoor != nullptr );

    aOutdoor->setHCoeffModel( BoundaryConditionsCoeffModel::HPrescribed );
    aOutdoor->setSkyModel( SkyModel::TSkySpecified );

    double radiosity = aOutdoor->getEnvironmentRadiosity();

    EXPECT_NEAR( 459.2457, radiosity, 1e-6 );
  } catch( exception &e ) {
    cout << e.what() << endl;
  }

}

TEST_F( TestOutdoorEnvironment, HPrescribed_Swinbank )
{
  try {
    SCOPED_TRACE( "Begin Test: Outdoors -> H model = HPrescribed; Sky Model = Swinbank" );
    
    shared_ptr< CTarOutdoorEnvironment > aOutdoor = nullptr;
    
    aOutdoor = GetOutdoors();
    ASSERT_TRUE( aOutdoor != nullptr );

    aOutdoor->setHCoeffModel( BoundaryConditionsCoeffModel::HPrescribed );
    aOutdoor->setSkyModel( SkyModel::Swinbank );

    double radiosity = aOutdoor->getEnvironmentRadiosity();

    EXPECT_NEAR( 459.2457, radiosity, 1e-6 );
  } catch( exception &e ) {
    cout << e.what() << endl;
  }

}