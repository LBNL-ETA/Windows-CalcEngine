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

// Test equivalent properties of triple layer with direct-direct, direct-diffuse and diffuse-diffuse components
// Tests include adding layer on back and front sides. EquivalentLayer contains only transmittance and reflectance
class TestEquivalentLayerWithScattering2 : public testing::Test {

private:
  // Additional layer added to the back side
  shared_ptr< CEquivalentLayer > m_EqLayerFront;
  shared_ptr< CEquivalentLayer > m_EqLayerBack;

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
    
    m_EqLayerFront = make_shared< CEquivalentLayer >( aLayer1 );
    m_EqLayerFront->addLayer( aLayer2, Side::Back );
    m_EqLayerFront->addLayer( aLayer3, Side::Back );

    m_EqLayerBack = make_shared< CEquivalentLayer >( aLayer3 );
    m_EqLayerBack->addLayer( aLayer2, Side::Front );
    m_EqLayerBack->addLayer( aLayer1, Side::Front );
  
  }

public:
  shared_ptr< CEquivalentLayer > getBack() { return m_EqLayerBack; };
  shared_ptr< CEquivalentLayer > getFront() { return m_EqLayerFront; };

};

TEST_F( TestEquivalentLayerWithScattering2, TestTripleLayerBack ) {
  SCOPED_TRACE( "Begin Test: Equivalent layer transmittance and reflectances (direct-direct, direct-diffuse and diffuse-diffuse" );
  
  shared_ptr< CEquivalentLayer > doubleLayer = getBack();

  ///////////////////////////////////////////////
  // Direct-Direct
  ///////////////////////////////////////////////
  double Tf = doubleLayer->getPropertySimple( PropertySimple::T, Side::Front, Scattering::DirectDirect );
  EXPECT_NEAR( 0.000648224, Tf, 1e-6 );

  double Rf = doubleLayer->getPropertySimple( PropertySimple::R, Side::Front, Scattering::DirectDirect );
  EXPECT_NEAR( 0.050534583, Rf, 1e-6 );

  double Tb = doubleLayer->getPropertySimple( PropertySimple::T, Side::Back, Scattering::DirectDirect );
  EXPECT_NEAR( 0.002567576, Tb, 1e-6 );

  double Rb = doubleLayer->getPropertySimple( PropertySimple::R, Side::Back, Scattering::DirectDirect );
  EXPECT_NEAR( 0.250039501, Rb, 1e-6 );

  ///////////////////////////////////////////////
  // Diffuse-Diffuse
  ///////////////////////////////////////////////
  Tf = doubleLayer->getPropertySimple( PropertySimple::T, Side::Front, Scattering::DiffuseDiffuse );
  EXPECT_NEAR( 0.167799034, Tf, 1e-6 );

  Rf = doubleLayer->getPropertySimple( PropertySimple::R, Side::Front, Scattering::DiffuseDiffuse );
  EXPECT_NEAR( 0.692483233, Rf, 1e-6 );

  Tb = doubleLayer->getPropertySimple( PropertySimple::T, Side::Back, Scattering::DiffuseDiffuse );
  EXPECT_NEAR( 0.284211597, Tb, 1e-6 );

  Rb = doubleLayer->getPropertySimple( PropertySimple::R, Side::Back, Scattering::DiffuseDiffuse );
  EXPECT_NEAR( 0.395593248, Rb, 1e-6 );

  ///////////////////////////////////////////////
  // Direct-Diffuse
  ///////////////////////////////////////////////
  Tf = doubleLayer->getPropertySimple( PropertySimple::T, Side::Front, Scattering::DirectDiffuse );
  EXPECT_NEAR( 0.197511986, Tf, 1e-6 );

  Rf = doubleLayer->getPropertySimple( PropertySimple::R, Side::Front, Scattering::DirectDiffuse );
  EXPECT_NEAR( 0.429497739, Rf, 1e-6 );

  Tb = doubleLayer->getPropertySimple( PropertySimple::T, Side::Back, Scattering::DirectDiffuse );
  EXPECT_NEAR( 0.220590948, Tb, 1e-6 );

  Rb = doubleLayer->getPropertySimple( PropertySimple::R, Side::Back, Scattering::DirectDiffuse );
  EXPECT_NEAR( 0.316271007, Rb, 1e-6 );
}

TEST_F( TestEquivalentLayerWithScattering2, TestTripleLayerFront ) {
  SCOPED_TRACE( "Begin Test: Equivalent layer transmittance and reflectances (direct-direct, direct-diffuse and diffuse-diffuse" );

  shared_ptr< CEquivalentLayer > doubleLayer = getFront();

  ///////////////////////////////////////////////
  // Direct-Direct
  ///////////////////////////////////////////////
  double Tf = doubleLayer->getPropertySimple( PropertySimple::T, Side::Front, Scattering::DirectDirect );
  EXPECT_NEAR( 0.000648224, Tf, 1e-6 );

  double Rf = doubleLayer->getPropertySimple( PropertySimple::R, Side::Front, Scattering::DirectDirect );
  EXPECT_NEAR( 0.050534583, Rf, 1e-6 );

  double Tb = doubleLayer->getPropertySimple( PropertySimple::T, Side::Back, Scattering::DirectDirect );
  EXPECT_NEAR( 0.002567576, Tb, 1e-6 );

  double Rb = doubleLayer->getPropertySimple( PropertySimple::R, Side::Back, Scattering::DirectDirect );
  EXPECT_NEAR( 0.250039501, Rb, 1e-6 );

  ///////////////////////////////////////////////
  // Diffuse-Diffuse
  ///////////////////////////////////////////////
  Tf = doubleLayer->getPropertySimple( PropertySimple::T, Side::Front, Scattering::DiffuseDiffuse );
  EXPECT_NEAR( 0.167799034, Tf, 1e-6 );

  Rf = doubleLayer->getPropertySimple( PropertySimple::R, Side::Front, Scattering::DiffuseDiffuse );
  EXPECT_NEAR( 0.692483233, Rf, 1e-6 );

  Tb = doubleLayer->getPropertySimple( PropertySimple::T, Side::Back, Scattering::DiffuseDiffuse );
  EXPECT_NEAR( 0.284211597, Tb, 1e-6 );

  Rb = doubleLayer->getPropertySimple( PropertySimple::R, Side::Back, Scattering::DiffuseDiffuse );
  EXPECT_NEAR( 0.395593248, Rb, 1e-6 );

  ///////////////////////////////////////////////
  // Direct-Diffuse
  ///////////////////////////////////////////////
  Tf = doubleLayer->getPropertySimple( PropertySimple::T, Side::Front, Scattering::DirectDiffuse );
  EXPECT_NEAR( 0.197511986, Tf, 1e-6 );

  Rf = doubleLayer->getPropertySimple( PropertySimple::R, Side::Front, Scattering::DirectDiffuse );
  EXPECT_NEAR( 0.429497739, Rf, 1e-6 );

  Tb = doubleLayer->getPropertySimple( PropertySimple::T, Side::Back, Scattering::DirectDiffuse );
  EXPECT_NEAR( 0.220590948, Tb, 1e-6 );

  Rb = doubleLayer->getPropertySimple( PropertySimple::R, Side::Back, Scattering::DirectDiffuse );
  EXPECT_NEAR( 0.316271007, Rb, 1e-6 );
}