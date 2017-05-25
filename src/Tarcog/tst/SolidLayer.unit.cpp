#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

using namespace std;
using namespace Tarcog;

class TestSolidLayer : public testing::Test {

private:
  shared_ptr< CIGUSolidLayer > m_SolidLayer;

protected:
  virtual void SetUp() {
    try {
      shared_ptr< CSurface > surface1 = std::make_shared< CSurface > ();
      ASSERT_TRUE( surface1 != nullptr );
      surface1->setTemperature( 280 );
      shared_ptr< CSurface > surface2 = std::make_shared< CSurface > ();
      ASSERT_TRUE( surface2 != nullptr );
      surface2->setTemperature( 300 );

      m_SolidLayer = make_shared< CIGUSolidLayer > ( 0.01, 2.5, surface1, surface2 );
      ASSERT_TRUE( m_SolidLayer != nullptr );
    } catch( exception &e ) {
      cout << e.what() << endl;
      throw &e;
    }
  }

public:
  shared_ptr< CIGUSolidLayer > GetLayer() { return m_SolidLayer; };

};

TEST_F( TestSolidLayer, Test1 ) {
  try {
    SCOPED_TRACE( "Begin Test: Test Solid Layer - Conduction heat flow" );
    
    shared_ptr< CIGUSolidLayer > aLayer = nullptr;
    
    aLayer = GetLayer();
    ASSERT_TRUE( aLayer != nullptr );
    
    double conductionHeatFlow = aLayer->getConvectionConductionFlow();

    EXPECT_NEAR( 5000, conductionHeatFlow, 1e-6 );
  } catch( exception &e ) {
    cout << e.what() << endl;
  }

}