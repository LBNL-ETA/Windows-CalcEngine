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
    m_Emissivity( t_Emissivity ), m_Transmittance( t_Transmittance )
  {
    initialize();
  }

  CTarSurface::CTarSurface() : m_Emissivity( 0.84 ), m_Transmittance( 0 ) {
    initialize();
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

  void CTarSurface::initialize() {
    if ( m_Emissivity + m_Transmittance > 1 ) {
      throw runtime_error( "Sum of emittance and transmittance cannot be greater than one." );
    } else {
      m_Reflectance = 1 - m_Emissivity - m_Transmittance;
    }
    m_Temperature = 273.15;
    m_J = 0;
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