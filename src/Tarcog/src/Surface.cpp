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

  CSurface::CSurface(double t_Emissivity, double t_Transmittance):
    m_Emissivity( t_Emissivity ), m_Transmittance( t_Transmittance ), m_Temperature( 273.15 ), 
    m_J( 0 ), m_MeanDeflection( 0 ), m_MaxDeflection( 0 ) {
    m_Temperature = 273.15;
    m_J = 0;
    initializeOptical();
  }

  CSurface::CSurface( const CSurface& t_Surface ) {
    m_Emissivity = t_Surface.m_Emissivity;
    m_Transmittance = t_Surface.m_Transmittance;
    m_Temperature = t_Surface.m_Temperature;
    m_J = t_Surface.m_J;
    m_MaxDeflection = t_Surface.m_MaxDeflection;
    m_MeanDeflection = t_Surface.m_MeanDeflection;
    initializeOptical();
  }

  CSurface::CSurface() : m_Emissivity( 0.84 ), m_Transmittance( 0 ), m_Temperature( 273.15 ), 
    m_J( 0 ), m_MeanDeflection( 0 ), m_MaxDeflection( 0 ) {
    initializeOptical();
  }

  double CSurface::getTemperature() const {
    return m_Temperature;
  }

  void CSurface::setTemperature( double const t_Temperature ) {
    m_Temperature = t_Temperature;
  }

  double CSurface::getEmissivity() const {
    return m_Emissivity;
  }

  double CSurface::getReflectance() const {
    return m_Reflectance;
  }

  double CSurface::getTransmittance() const {
    return m_Transmittance;
  }

  void CSurface::initializeOptical() {
    if ( m_Emissivity + m_Transmittance > 1 ) {
      throw runtime_error( "Sum of emittance and transmittance cannot be greater than one." );
    } else {
      m_Reflectance = 1 - m_Emissivity - m_Transmittance;
    }
  }

  void CSurface::initializeStart( double const t_Temperature ) {
    using ConstantsData::STEFANBOLTZMANN;

    m_Temperature = t_Temperature;
    m_J = STEFANBOLTZMANN * pow( m_Temperature, 4 );
  }

  void CSurface::initializeStart( const double t_Temperature, const double t_Radiation ) {
    m_Temperature = t_Temperature;
    m_J = t_Radiation;
  }

  void CSurface::applyDeflection( const double t_MeanDeflection, const double t_MaxDeflection ) {
    m_MeanDeflection = t_MeanDeflection;
    m_MaxDeflection = t_MaxDeflection;
  }

  double CSurface::getMeanDeflection() const {
    return m_MeanDeflection;
  }

  double CSurface::getMaxDeflection() const {
    return m_MaxDeflection;
  }

  double CSurface::J() const {
    return m_J;
  }

  void CSurface::setJ( double const t_J ) {
    m_J = t_J;
  }

  double CSurface::emissivePowerTerm() const {
    using ConstantsData::STEFANBOLTZMANN;

    return STEFANBOLTZMANN * m_Emissivity * pow( m_Temperature, 3 );
  }
  
}