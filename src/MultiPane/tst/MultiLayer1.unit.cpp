#include <memory>
#include <gtest/gtest.h>

#include "MultiLayer.hpp"
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
class TestMultiLayer1 : public testing::Test {

private:
  shared_ptr< CMultiLayer > m_Layer;

protected:
  virtual void SetUp() {
    shared_ptr< CScatteringSurface > f1 = make_shared< CScatteringSurface >( 0.08, 0.05, 0.46, 0.23, 0.46, 0.52 );
    shared_ptr< CScatteringSurface > b1 = make_shared< CScatteringSurface >( 0.13, 0.25, 0.38, 0.19, 0.64, 0.22 );
    shared_ptr< CLayer > aLayer1 = make_shared< CLayer >( f1, b1 );
    
    shared_ptr< CScatteringSurface > f2 = make_shared< CScatteringSurface >( 0.1, 0.05, 0.48, 0.26, 0.56, 0.34 );
    shared_ptr< CScatteringSurface > b2 = make_shared< CScatteringSurface >( 0.15, 0.0, 0.38, 0.19, 0.49, 0.39 );
    shared_ptr< CLayer > aLayer2 = make_shared< CLayer >( f2, b2 );

    shared_ptr< CScatteringSurface > f3 = make_shared< CScatteringSurface >( 0.08, 0.05, 0.46, 0.23, 0.46, 0.52 );
    shared_ptr< CScatteringSurface > b3 = make_shared< CScatteringSurface >( 0.13, 0.25, 0.38, 0.19, 0.64, 0.22 );
    shared_ptr< CLayer > aLayer3 = make_shared< CLayer >( f3, b3 );
    
    m_Layer = make_shared< CMultiLayer >( aLayer1 );
    m_Layer->addLayer( aLayer2 );
    m_Layer->addLayer( aLayer3 );
  
  }

public:
  shared_ptr< CMultiLayer > getLayer() { return m_Layer; };

};

TEST_F( TestMultiLayer1, TestTripleLayer ) {
  SCOPED_TRACE( "Begin Test: Test triple layer with scattering properties." );
  
  shared_ptr< CMultiLayer > aLayer = getLayer();

  ///////////////////////////////////////////////
  // Direct-Direct
  ///////////////////////////////////////////////
  double Tf = 0;
  
}