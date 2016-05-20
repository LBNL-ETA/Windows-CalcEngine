#include <assert.h>

#include "EquivalentLayerSingleComponent.hpp"
#include "Series.hpp"

using namespace FenestrationCommon;
using namespace std;

namespace MultiPane {

  CEquivalentLayerSingleComponent::CEquivalentLayerSingleComponent( std::shared_ptr< FenestrationCommon::CSeries > t_T, 
      std::shared_ptr< FenestrationCommon::CSeries > t_Rf, 
      std::shared_ptr< FenestrationCommon::CSeries > t_Rb  ) : m_T( t_T ), m_Rf( t_Rf ), m_Rb( t_Rb ) {
     
  }

  void CEquivalentLayerSingleComponent::addLayer( std::shared_ptr< FenestrationCommon::CSeries > t_T, 
      std::shared_ptr< FenestrationCommon::CSeries > t_Rf, 
      std::shared_ptr< FenestrationCommon::CSeries > t_Rb ) {
    
    shared_ptr< CSeries > tTot = transmittanceTot( *m_T, *t_T, *m_Rb, *t_Rf );
    shared_ptr< CSeries > tRfTot = ReflectanceFrontTot( *m_T, *m_Rf, *m_Rb, *t_Rf );
    shared_ptr< CSeries > tRbTot = ReflectanceBackTot( *t_T, *t_Rb, *m_Rb, *t_Rf );

    m_T = tTot;
    m_Rf = tRfTot;
    m_Rb = tRbTot;

  }

  shared_ptr< CSeries > CEquivalentLayerSingleComponent::T() const {
    return m_T; 
  }

  shared_ptr< CSeries > CEquivalentLayerSingleComponent::Rf() const {
    return m_Rf;
  }

  shared_ptr< CSeries > CEquivalentLayerSingleComponent::Rb() const {
    return m_Rb; 
  }

  shared_ptr< CSeries > CEquivalentLayerSingleComponent::AbsF() {
    size_t size = m_T->size();
    m_AbsF = make_shared< CSeries >();
    for( size_t i = 0; i < size; ++i ) {
      double wl = ( *m_T )[ i ]->x();
      double value = 1 - ( *m_T )[ i ]->value() - ( *m_Rf )[ i ]->value();
      m_AbsF->addProperty( wl, value );
    }
    return m_AbsF;
  }

  shared_ptr< CSeries > CEquivalentLayerSingleComponent::AbsB() {
    size_t size = m_T->size();
    m_AbsB = make_shared< CSeries >();
    for( size_t i = 0; i < size; ++i ) {
      double wl = ( *m_T )[ i ]->x();
      double value = 1 - ( *m_T )[ i ]->value() - ( *m_Rb )[ i ]->value();
      m_AbsB->addProperty( wl, value );
    }
    return m_AbsB;
  }

  // Calculates total transmittance of equivalent layer over the entire spectrum. It expects that all properties are the same size.
  shared_ptr< CSeries > CEquivalentLayerSingleComponent::transmittanceTot( CSeries& t_T1, 
    CSeries& t_T2, CSeries& t_Rb1, CSeries& t_Rf2 ) {
    assert( t_T1.size() == t_T2.size() );
    assert( t_T1.size() == t_Rb1.size() );
    assert( t_T1.size() == t_Rf2.size() );

    shared_ptr< CSeries > aSpectralProperties = make_shared< CSeries >();

    for ( size_t i = 0; i < t_T1.size(); ++i ) {
      double tTot = t_T1[i]->value() * t_T2[i]->value() / ( 1 - t_Rb1[i]->value() * t_Rf2[i]->value() );
      double wv = t_T1[i]->x();
      aSpectralProperties->addProperty( wv, tTot );
    }

    return aSpectralProperties;
  }

  // Calculates total front reflectance of equvalent layer over the entire spectrum. Properties must be same size
  shared_ptr< CSeries > CEquivalentLayerSingleComponent::ReflectanceFrontTot( CSeries& t_T1, CSeries& t_Rf1, 
    CSeries& t_Rb1, CSeries& t_Rf2 ) {
    assert( t_T1.size() == t_Rf1.size() );
    assert( t_T1.size() == t_Rb1.size() );
    assert( t_T1.size() == t_Rf2.size() );

    shared_ptr< CSeries > aSpectralProperties = make_shared< CSeries >();

    for ( size_t i = 0; i < t_T1.size(); ++i ) {
      double tRfTot = t_Rf1[i]->value() + t_T1[i]->value() * t_T1[i]->value() * t_Rf2[i]->value()
        / ( 1 - t_Rb1[i]->value() * t_Rf2[i]->value() );
      double wv = t_T1[i]->x();
      aSpectralProperties->addProperty( wv, tRfTot );
    }

    return aSpectralProperties;
  }

  // Calculates total back reflectance of equvalent layer over the entire spectrum. Properties must be same size
  shared_ptr< CSeries > CEquivalentLayerSingleComponent::ReflectanceBackTot( CSeries& t_T2, CSeries& t_Rb2, 
    CSeries& t_Rb1, CSeries& t_Rf2 ) {
    assert( t_T2.size() == t_Rb2.size() );
    assert( t_T2.size() == t_Rb1.size() );
    assert( t_T2.size() == t_Rf2.size() );

    shared_ptr< CSeries > aSpectralProperties = make_shared< CSeries >();

    for ( size_t i = 0; i < t_T2.size(); ++i ) {
      double tRbTot = t_Rb2[i]->value() + t_T2[i]->value() * t_T2[i]->value() * t_Rb1[i]->value()
        / ( 1 - t_Rb1[i]->value() * t_Rf2[i]->value() );
      double wv = t_T2[i]->x();
      aSpectralProperties->addProperty( wv, tRbTot );
    }

    return aSpectralProperties;
  }

}