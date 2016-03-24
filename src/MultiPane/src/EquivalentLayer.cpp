#include <assert.h>

#include "EquivalentLayer.hpp"
#include "SpectralProperties.hpp"

using namespace SpectralAveraging;
using namespace std;

namespace MultiPane {

  CEquivalentLayer::CEquivalentLayer( std::shared_ptr< SpectralAveraging::CSpectralProperties > t_T, 
      std::shared_ptr< SpectralAveraging::CSpectralProperties > t_Rf, 
      std::shared_ptr< SpectralAveraging::CSpectralProperties > t_Rb  ) : m_T( t_T ), m_Rf( t_Rf ), m_Rb( t_Rb ) {
     
  };

  void CEquivalentLayer::addLayer( std::shared_ptr< SpectralAveraging::CSpectralProperties > t_T, 
      std::shared_ptr< SpectralAveraging::CSpectralProperties > t_Rf, 
      std::shared_ptr< SpectralAveraging::CSpectralProperties > t_Rb ) {
    
    shared_ptr< CSpectralProperties > tTot = transmittanceTot( *m_T, *t_T, *m_Rb, *t_Rf );
    shared_ptr< CSpectralProperties > tRfTot = ReflectanceFrontTot( *m_T, *m_Rf, *m_Rb, *t_Rf );
    shared_ptr< CSpectralProperties > tRbTot = ReflectanceBackTot( *t_T, *t_Rb, *m_Rb, *t_Rf );

    m_T = tTot;
    m_Rf = tRfTot;
    m_Rb = tRbTot;

  };

  shared_ptr< CSpectralProperties > CEquivalentLayer::T() const {
    return m_T; 
  };

  shared_ptr< CSpectralProperties > CEquivalentLayer::Rf() const {
    return m_Rf;
  };

  shared_ptr< CSpectralProperties > CEquivalentLayer::Rb() const {
    return m_Rb; 
  };

  shared_ptr< CSpectralProperties > CEquivalentLayer::AbsF() {
    size_t size = m_T->size();
    m_AbsF = make_shared< CSpectralProperties >();
    for( size_t i = 0; i < size; ++i ) {
      double wl = ( *m_T )[ i ]->wavelength();
      double value = 1 - ( *m_T )[ i ]->value() - ( *m_Rf )[ i ]->value();
      m_AbsF->addProperty( wl, value );
    }
    return m_AbsF;
  };

  shared_ptr< CSpectralProperties > CEquivalentLayer::AbsB() {
    size_t size = m_T->size();
    m_AbsB = make_shared< CSpectralProperties >();
    for( size_t i = 0; i < size; ++i ) {
      double wl = ( *m_T )[ i ]->wavelength();
      double value = 1 - ( *m_T )[ i ]->value() - ( *m_Rb )[ i ]->value();
      m_AbsB->addProperty( wl, value );
    }
    return m_AbsB;
  };


  // Calculates total transmittance of equivalent layer over the entire spectrum. It expects that all properties are the same size.
  shared_ptr< CSpectralProperties > CEquivalentLayer::transmittanceTot( CSpectralProperties& t_T1, 
    CSpectralProperties& t_T2, CSpectralProperties& t_Rb1, CSpectralProperties& t_Rf2 ) {
    assert( t_T1.size() == t_T2.size() );
    assert( t_T1.size() == t_Rb1.size() );
    assert( t_T1.size() == t_Rf2.size() );

    shared_ptr< CSpectralProperties > aSpectralProperties = make_shared< CSpectralProperties >();

    for ( size_t i = 0; i < t_T1.size(); ++i ) {
      double tTot = t_T1[i]->value() * t_T2[i]->value() / ( 1 - t_Rb1[i]->value() * t_Rf2[i]->value() );
      double wv = t_T1[i]->wavelength();
      aSpectralProperties->addProperty( wv, tTot );
    }

    return aSpectralProperties;
  };

  // Calculates total front reflectance of equvalent layer over the entire spectrum. Properties must be same size
  shared_ptr< CSpectralProperties > CEquivalentLayer::ReflectanceFrontTot( CSpectralProperties& t_T1, CSpectralProperties& t_Rf1, 
    CSpectralProperties& t_Rb1, CSpectralProperties& t_Rf2 ) {
    assert( t_T1.size() == t_Rf1.size() );
    assert( t_T1.size() == t_Rb1.size() );
    assert( t_T1.size() == t_Rf2.size() );

    shared_ptr< CSpectralProperties > aSpectralProperties = make_shared< CSpectralProperties >();

    for ( size_t i = 0; i < t_T1.size(); ++i ) {
      double tRfTot = t_Rf1[i]->value() + t_T1[i]->value() * t_T1[i]->value() * t_Rf2[i]->value()
        / ( 1 - t_Rb1[i]->value() * t_Rf2[i]->value() );
      double wv = t_T1[i]->wavelength();
      aSpectralProperties->addProperty( wv, tRfTot );
    }

    return aSpectralProperties;
  };

  // Calculates total back reflectance of equvalent layer over the entire spectrum. Properties must be same size
  shared_ptr< CSpectralProperties > CEquivalentLayer::ReflectanceBackTot( CSpectralProperties& t_T2, CSpectralProperties& t_Rb2, 
    CSpectralProperties& t_Rb1, CSpectralProperties& t_Rf2 ) {
    assert( t_T2.size() == t_Rb2.size() );
    assert( t_T2.size() == t_Rb1.size() );
    assert( t_T2.size() == t_Rf2.size() );

    shared_ptr< CSpectralProperties > aSpectralProperties = make_shared< CSpectralProperties >();

    for ( size_t i = 0; i < t_T2.size(); ++i ) {
      double tRbTot = t_Rb2[i]->value() + t_T2[i]->value() * t_T2[i]->value() * t_Rb1[i]->value()
        / ( 1 - t_Rb1[i]->value() * t_Rf2[i]->value() );
      double wv = t_T2[i]->wavelength();
      aSpectralProperties->addProperty( wv, tRbTot );
    }

    return aSpectralProperties;
  };

}