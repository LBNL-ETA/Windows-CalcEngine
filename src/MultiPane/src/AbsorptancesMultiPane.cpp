#include "AbsorptancesMultiPane.hpp"
#include "SpectralProperties.hpp"

using namespace std;
using namespace SpectralAveraging;

namespace MultiPane {

  CAbsorptancesMultiPane::CAbsorptancesMultiPane( shared_ptr< const CSpectralProperties > t_T, 
      shared_ptr< const CSpectralProperties > t_Rf, 
      shared_ptr< const CSpectralProperties > t_Rb ) : m_StateCalculated( false ) {
    m_T.push_back( t_T );
    m_Rf.push_back( t_Rf );
    m_Rb.push_back( t_Rb );
  };

  void CAbsorptancesMultiPane::addLayer( shared_ptr< const CSpectralProperties > t_T, 
      shared_ptr< const CSpectralProperties > t_Rf, 
      shared_ptr< const CSpectralProperties > t_Rb ) {
    m_T.push_back( t_T );
    m_Rf.push_back( t_Rf );
    m_Rb.push_back( t_Rb );
    m_StateCalculated = false;
  };

  shared_ptr< CSpectralProperties > CAbsorptancesMultiPane::Abs( size_t const Index ) {
    calculateState();
    shared_ptr< CSpectralProperties > aAbs = nullptr;
    if( Index < m_Abs.size() ) {
      aAbs = m_Abs[ Index ];
    }
    return aAbs;
  };

  size_t CAbsorptancesMultiPane::numOfLayers() {
    calculateState();
    return m_Abs.size();    
  };

  void CAbsorptancesMultiPane::calculateState() {
    if( !m_StateCalculated ) {
      size_t size = m_T.size();

      // Calculate r and t coefficients
      shared_ptr< CSpectralProperties > r = make_shared< CSpectralProperties >();
      shared_ptr< CSpectralProperties > t = make_shared< CSpectralProperties >();
      shared_ptr< vector< double > > wv = m_T[size-1]->getWavelengths();
      r->setConstantValues( wv, 0 );
      t->setConstantValues( wv, 0 );
      m_rCoeffs.clear();
      m_tCoeffs.clear();
      
      // layers loop
      for( int i = int( size ) - 1; i >= 0; --i ) {
        t = tCoeffs( m_T[i], m_Rb[i], r );
        r = rCoeffs( m_T[i], m_Rf[i], m_Rb[i], r );

        m_rCoeffs.insert( m_rCoeffs.begin(), r );
        m_tCoeffs.insert( m_tCoeffs.begin(), t );
      }

      // Calculate normalized radiances
      size = m_rCoeffs.size();
      vector< shared_ptr< CSpectralProperties > > Iplus;
      vector< shared_ptr< CSpectralProperties > > Iminus;

      shared_ptr< CSpectralProperties > Im = make_shared< CSpectralProperties >();
      shared_ptr< CSpectralProperties > Ip = nullptr;
      Im->setConstantValues( wv, 1 );
      Iminus.push_back( Im );

      for( size_t i = 0; i < size; ++i ) {
        Ip = m_rCoeffs[ i ]->mMult( Im );
        Im = m_tCoeffs[ i ]->mMult( Im );
        Iplus.push_back( Ip );
        Iminus.push_back( Im );
      }
      Ip = make_shared< CSpectralProperties >();
      Ip->setConstantValues( wv, 0 );
      Iplus.push_back( Ip );

      // Calculate absorptances
      m_Abs.clear();
      size = Iminus.size();
      for( size_t i = 0; i < size - 1; ++i ) {
        shared_ptr< CSpectralProperties > Iincoming = Iminus[ i ]->mSub( Iplus[ i ] );
        shared_ptr< CSpectralProperties > Ioutgoing = Iminus[ i+1 ]->mSub( Iplus[ i+1 ] );
        shared_ptr< CSpectralProperties > layerAbs = Iincoming->mSub( Ioutgoing );
        m_Abs.push_back( layerAbs );
      }
    }
  }

  shared_ptr< CSpectralProperties > CAbsorptancesMultiPane::rCoeffs( 
      shared_ptr< const CSpectralProperties > t_T, 
      shared_ptr< const CSpectralProperties > t_Rf,
      shared_ptr< const CSpectralProperties > t_Rb,
      shared_ptr< const CSpectralProperties > t_RCoeffs ) {
    
    shared_ptr< CSpectralProperties > rCoeffs = make_shared< CSpectralProperties >();
    size_t size = t_T->size();
     
    for( size_t i = 0; i < size; ++i ) {
      double wl = (*t_T)[i]->wavelength();
      double rValue = ( *t_Rf )[ i ]->value() + ( *t_T )[ i ]->value() * ( *t_T )[ i ]->value() * ( *t_RCoeffs )[i]->value() / 
        ( 1 - (*t_Rb)[i]->value() * ( *t_RCoeffs )[i]->value() );
      rCoeffs->addProperty( wl, rValue );
    }

    return rCoeffs;
  };

  shared_ptr< CSpectralProperties > CAbsorptancesMultiPane::tCoeffs( 
      shared_ptr< const CSpectralProperties > t_T,
      shared_ptr< const CSpectralProperties > t_Rb,
      shared_ptr< const CSpectralProperties > t_RCoeffs ) {
    
    shared_ptr< CSpectralProperties > tCoeffs = make_shared< CSpectralProperties >();
    size_t size = t_T->size();
     
    for( size_t i = 0; i < size; ++i ) {
      double wl = (*t_T)[i]->wavelength();
      double tValue = ( *t_T )[ i ]->value() / ( 1 - (*t_Rb)[i]->value() * ( *t_RCoeffs )[i]->value() );
      tCoeffs->addProperty( wl, tValue );
    }

    return tCoeffs;
  };

}