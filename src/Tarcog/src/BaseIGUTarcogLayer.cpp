#include <memory>
#include <assert.h>

#include "BaseIGUTarcogLayer.hpp"
#include "TarSurface.hpp"
#include "FenestrationCommon.hpp"

using namespace FenestrationCommon;
using namespace std;

namespace Tarcog {

  CBaseIGUTarcogLayer::CBaseIGUTarcogLayer( double const t_Thickness ) : 
    m_Thickness( t_Thickness ) {
    
  }

  CBaseIGUTarcogLayer::CBaseIGUTarcogLayer( const CBaseIGUTarcogLayer& t_Layer ) : 
    CState( t_Layer ), CBaseTarcogLayer( t_Layer ) {
    m_Thickness = t_Layer.m_Thickness;
  }

  double CBaseIGUTarcogLayer::layerTemperature() {
    return ( m_Surface.at( Side::Front )->getTemperature() + 
      m_Surface.at( Side::Back )->getTemperature() ) / 2;
  }

  double CBaseIGUTarcogLayer::getThickness() const {
    return m_Thickness + getSurface( Side::Front )->getMeanDeflection() - 
      getSurface( Side::Back )->getMeanDeflection();
  }

  double CBaseIGUTarcogLayer::getTemperature( const Side t_Position ) const {
    return getSurface( t_Position )->getTemperature();
  }

  double CBaseIGUTarcogLayer::J( const Side t_Position ) const {
    return getSurface( t_Position )->J();
  }

  double CBaseIGUTarcogLayer::getMaxDeflection() const {
    assert( getSurface( Side::Front )->getMaxDeflection() == 
      getSurface( Side::Back )->getMaxDeflection() );
    return getSurface( Side::Front )->getMaxDeflection();
  }

  double CBaseIGUTarcogLayer::getMeanDeflection() const {
    assert( getSurface( Side::Front )->getMeanDeflection() ==
      getSurface( Side::Back )->getMeanDeflection() );
    return getSurface( Side::Front )->getMeanDeflection();
  }

  double CBaseIGUTarcogLayer::getConductivity() {
    return getConductionConvectionCoefficient() * m_Thickness;
  }

}
