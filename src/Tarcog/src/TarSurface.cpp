#include <math.h>
#include <stdexcept>
#include <assert.h>
#include <vector>
#include <algorithm>

#include "BaseTarcogLayer.hpp"
#include "TarSurface.hpp"
#include "Constants.hpp"

using namespace std;

namespace Tarcog {

  CTarSurface::CTarSurface(double t_Emissivity, double t_Transmittance):
    m_Emissivity( t_Emissivity ), m_Transmittance( t_Transmittance ), m_Temperature( 273.15 ), 
    m_J( 0 ), m_MeanDeflection( 0 ), m_MaxDeflection( 0 ) {
    m_Temperature = 273.15;
    m_J = 0;
    initializeOptical();
  }

  CTarSurface::CTarSurface( const CTarSurface& t_Surface ) {
    m_Emissivity = t_Surface.m_Emissivity;
    m_Transmittance = t_Surface.m_Transmittance;
    m_Temperature = t_Surface.m_Temperature;
    m_J = t_Surface.m_J;
    m_MaxDeflection = t_Surface.m_MaxDeflection;
    m_MeanDeflection = t_Surface.m_MeanDeflection;
    initializeOptical();
  }

  CTarSurface::CTarSurface() : m_Emissivity( 0.84 ), m_Transmittance( 0 ), m_Temperature( 273.15 ), 
    m_J( 0 ), m_MeanDeflection( 0 ), m_MaxDeflection( 0 ) {
    initializeOptical();
  }

  double CTarSurface::getTemperature() const {
    return m_Temperature;
  }

  void CTarSurface::setTemperature( double const t_Temperature ) {
    m_Temperature = t_Temperature;
  }

  double CTarSurface::getEmissivity() const {
    return m_Emissivity;
  }

  double CTarSurface::getReflectance() const {
    return m_Reflectance;
  }

  double CTarSurface::getTransmittance() const {
    return m_Transmittance;
  }

  void CTarSurface::initializeOptical() {
    if ( m_Emissivity + m_Transmittance > 1 ) {
      throw runtime_error( "Sum of emittance and transmittance cannot be greater than one." );
    } else {
      m_Reflectance = 1 - m_Emissivity - m_Transmittance;
    }
  }

  void CTarSurface::initializeStart( double const t_Temperature ) {
    using ConstantsData::STEFANBOLTZMANN;

    m_Temperature = t_Temperature;
    m_J = STEFANBOLTZMANN * pow( m_Temperature, 4 );
  }

  void CTarSurface::initializeStart( const double t_Temperature, const double t_Radiation ) {
    m_Temperature = t_Temperature;
    m_J = t_Radiation;
  }

  void CTarSurface::applyDeflection( const double t_MeanDeflection, const double t_MaxDeflection ) {
    m_MeanDeflection = t_MeanDeflection;
    m_MaxDeflection = t_MaxDeflection;
  }

  double CTarSurface::getMeanDeflection() const {
    return m_MeanDeflection;
  }

  double CTarSurface::getMaxDeflection() const {
    return m_MaxDeflection;
  }

  double CTarSurface::J() const {
    return m_J;
  }

  void CTarSurface::setJ( double const t_J ) {
    m_J = t_J;
  }

  double CTarSurface::emissivePowerTerm() const {
    using ConstantsData::STEFANBOLTZMANN;

    return STEFANBOLTZMANN * m_Emissivity * pow( m_Temperature, 3 );
  }
  
}