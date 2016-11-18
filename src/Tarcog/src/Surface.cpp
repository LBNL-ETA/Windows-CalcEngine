#include <math.h>
#include <stdexcept>
#include <assert.h>
#include <vector>
#include <algorithm>

#include "BaseLayer.hpp"
#include "Surface.hpp"
#include "Constants.hpp"

using namespace std;

namespace Tarcog {

  //////////////////////////////////////////////////////////////////////////////
  // ISurface
  //////////////////////////////////////////////////////////////////////////////
  ISurface::ISurface() : m_Temperature( 273.15 ), m_J( 0 ), m_Emissivity( 0.84 ),
    m_Transmittance( 0 ), m_MeanDeflection( 0 ), m_MaxDeflection( 0 ) {
    calculateReflectance();
  }

  ISurface::ISurface( const double t_Emissivity, const double t_Transmittance ) :
    m_Temperature( 273.15 ), m_J( 0 ), m_Emissivity( t_Emissivity ),
    m_Transmittance( t_Transmittance ), m_MeanDeflection( 0 ), m_MaxDeflection( 0 ) {
    calculateReflectance();
  }

  ISurface::ISurface( const ISurface & t_Surface ) {
    m_Emissivity = t_Surface.m_Emissivity;
    m_Transmittance = t_Surface.m_Transmittance;
    m_Temperature = t_Surface.m_Temperature;
    m_J = t_Surface.m_J;
    m_MaxDeflection = t_Surface.m_MaxDeflection;
    m_MeanDeflection = t_Surface.m_MeanDeflection;
    calculateReflectance();
  }

  void ISurface::setTemperature( double const t_Temperature ) {
    m_Temperature = t_Temperature;
  }

  void ISurface::setJ( double const t_J ) {
    m_J = t_J;
  }

  void ISurface::applyDeflection( const double t_MeanDeflection, const double t_MaxDeflection ) {
    m_MeanDeflection = t_MeanDeflection;
    m_MaxDeflection = t_MaxDeflection;
  }

  double ISurface::getTemperature() const {
    return m_Temperature;
  }

  double ISurface::getEmissivity() const {
    return m_Emissivity;
  }

  double ISurface::getReflectance() const {
    return m_Reflectance;
  }

  double ISurface::getTransmittance() const {
    return m_Transmittance;
  }

  double ISurface::J() const {
    return m_J;
  }

  double ISurface::getMeanDeflection() const {
    return m_MeanDeflection;
  }

  double ISurface::getMaxDeflection() const {
    return m_MaxDeflection;
  }

  double ISurface::emissivePowerTerm() const {
    using ConstantsData::STEFANBOLTZMANN;

    return STEFANBOLTZMANN * m_Emissivity * pow( m_Temperature, 3 );
  }

  void ISurface::calculateReflectance() {
    if( m_Emissivity + m_Transmittance > 1 ) {
      throw runtime_error( "Sum of emittance and transmittance cannot be greater than one." );
    } else {
      m_Reflectance = 1 - m_Emissivity - m_Transmittance;
    }
  }

  void ISurface::initializeStart( double const t_Temperature ) {
    using ConstantsData::STEFANBOLTZMANN;

    m_Temperature = t_Temperature;
    m_J = STEFANBOLTZMANN * pow( m_Temperature, 4 );
  }

  void ISurface::initializeStart( const double t_Temperature, const double t_Radiation ) {
    m_Temperature = t_Temperature;
    m_J = t_Radiation;
  }


  //////////////////////////////////////////////////////////////////////////////
  // CSurface
  //////////////////////////////////////////////////////////////////////////////
  CSurface::CSurface(double t_Emissivity, double t_Transmittance): 
    ISurface( t_Emissivity, t_Transmittance ) {
    
  }

  CSurface::CSurface( const CSurface& t_Surface ) : ISurface( t_Surface ) {
    
  }

  shared_ptr< ISurface > CSurface::clone() const {
    return make_shared< CSurface >( *this );
  }

  CSurface::CSurface() : ISurface() {
    
  }

}
