#include <memory>
#include <gtest/gtest.h>

#include "EquivalentLayer.hpp"
#include "OpticalSurface.hpp"
#include "OpticalLayer.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace LayerOptics;
using namespace MultiPane;
using namespace FenestrationCommon;

// Test equivalent properties of double layer with direct-direct, direct-diffuse and diffuse-diffuse components
// Tests include adding layer on back and front sides
class TestEquivalentLayerWithScattering1 : public testing::Test {

private:
  // Additional layer added to the back side
  shared_ptr< CEquivalentLayer > m_DoubleBack;

  // Additional layer added to the front side
  shared_ptr< CEquivalentLayer > m_DoubleFront;

protected:
  virtual void SetUp() {
    shared_ptr< CScatteringSurface > f1 = make_shared< CScatteringSurface >( 0.08, 0.05, 0.46, 0.23, 0.46, 0.52 );
    shared_ptr< CScatteringSurface > b1 = make_shared< CScatteringSurface >( 0.13, 0.25, 0.38, 0.19, 0.64, 0.22 );
    shared_ptr< CLayer > aLayer1 = make_shared< CLayer >( f1, b1 );
    
    shared_ptr< CScatteringSurface > f2 = make_shared< CScatteringSurface >( 0.1, 0.05, 0.48, 0.26, 0.56, 0.34 );
    shared_ptr< CScatteringSurface > b2 = make_shared< CScatteringSurface >( 0.15, 0.0, 0.38, 0.19, 0.49, 0.39 );
    shared_ptr< CLayer > aLayer2 = make_shared< CLayer >( f2, b2 );
    
    m_DoubleBack = make_shared< CEquivalentLayer >( aLayer1 );
    m_DoubleBack->addLayer( aLayer2, Side::Back );

    m_DoubleFront = make_shared< CEquivalentLayer >( aLayer1 );
    m_DoubleFront->addLayer( aLayer2, Side::Front );
  
  }

public:
  shared_ptr< CEquivalentLayer > getDoubleBack() { return m_DoubleBack; };
  shared_ptr< CEquivalentLayer > getDoubleFront() { return m_DoubleFront; };

};

TEST_F( TestEquivalentLayerWithScattering1, TestSimpleComponents ) {
  SCOPED_TRACE( "Begin Test: Equivalent layer transmittance and reflectances (direct-direct, direct-diffuse and diffuse-diffuse" );
  
  shared_ptr< CEquivalentLayer > doubleLayer = getDoubleBack();

  double Tf = doubleLayer->getPropertySimple( PropertySimple::T, Side::Front, Scattering::DirectDirect );
  EXPECT_NEAR( 0.278426286, Tf, 1e-6 );

}