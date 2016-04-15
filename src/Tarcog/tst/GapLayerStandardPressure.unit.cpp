#include <memory>
#include <gtest/gtest.h>

#include "TarSurface.hpp"
#include "TarIGUSolidLayer.hpp"
#include "TarIGUGapLayer.hpp"
#include "TarIGU.hpp"

using namespace std;
using namespace Tarcog;

class TestGapLayerStandardPressure : public testing::Test {

private:
  std::shared_ptr< CTarIGUGapLayer > m_GapLayer;
  std::shared_ptr< CTarIGU > m_IGU;

protected:
  virtual void SetUp() {
    try {
      // Gap layer construct is made in a way that it is not possible to create gap alone. In order to test gap, entire
      // IGU has to be created. Example is taken as part of double clear air run from WINDOW 7 version of TARCOG
      shared_ptr< CTarSurface > surface1 = make_shared< CTarSurface > ();
      ASSERT_TRUE( surface1 != nullptr );
      shared_ptr< CTarSurface > surface2 = make_shared< CTarSurface > ();
      ASSERT_TRUE( surface2 != nullptr );
      shared_ptr< CTarSurface > surface3 = make_shared< CTarSurface > ();
      ASSERT_TRUE( surface3 != nullptr );
      shared_ptr< CTarSurface > surface4 = make_shared< CTarSurface > ();
      ASSERT_TRUE( surface4 != nullptr );

      double solidLayerThickness = 0.005715; // [m]
      double solidLayerConductance = 1;
      double gapSurface1Temperature = 262.756296539528;
      double gapSurface2Temperature = 276.349093518906;
      // surfaces 2 and 3 are actually surfaces of the gap
      surface2->setTemperature( gapSurface1Temperature );
      surface3->setTemperature( gapSurface2Temperature );
      std::shared_ptr< CTarIGUSolidLayer > solidLayer1 = 
        std::make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance, surface1, surface2 );
      ASSERT_TRUE( solidLayer1 != nullptr );
      std::shared_ptr< CTarIGUSolidLayer > solidLayer2 = 
        std::make_shared< CTarIGUSolidLayer > ( solidLayerThickness, solidLayerConductance, surface3, surface4 );
      ASSERT_TRUE( solidLayer2 != nullptr );

      double gapThickness = 0.012;
      double gapPressure = 101325;
      m_GapLayer = std::make_shared< CTarIGUGapLayer >( gapThickness, gapPressure );
      ASSERT_TRUE( m_GapLayer != nullptr );

      double windowWidth = 1;
      double windowHeight = 1;
      m_IGU = std::make_shared< CTarIGU >( windowWidth, windowHeight );
      ASSERT_TRUE( m_IGU != nullptr );
      m_IGU->addLayer( solidLayer1 );
      m_IGU->addLayer( m_GapLayer );
      m_IGU->addLayer( solidLayer2 );
    } catch( exception &e ) {
      cout << e.what() << endl;
      throw &e;
    }
  }

public:
  std::shared_ptr< CTarIGUGapLayer > GetLayer() { return m_GapLayer; };

};

TEST_F( TestGapLayerStandardPressure, ConvectionHeatFlow )
{
  try {
    SCOPED_TRACE( "Begin Test: Test Gap Layer - Convection heat flow [Pa = 101325 Pa]" );
    
    std::shared_ptr< CTarIGUGapLayer > aLayer = nullptr;
    
    aLayer = GetLayer();

    ASSERT_TRUE( aLayer != nullptr );

    double convectionHeatFlow = aLayer->getConvectionConductionFlow();

    EXPECT_NEAR( 27.673789062350764, convectionHeatFlow, 1e-6 );
  } catch( exception &e ) {
    cout << e.what() << endl;
  }

}