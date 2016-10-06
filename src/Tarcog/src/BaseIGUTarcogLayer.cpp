#include <memory>
#include "BaseIGUTarcogLayer.hpp"
#include "TarSurface.hpp"
#include "FenestrationCommon.hpp"

using namespace FenestrationCommon;

namespace Tarcog {

  CBaseIGUTarcogLayer::CBaseIGUTarcogLayer( double const t_Thickness ) : 
    m_Thickness( t_Thickness ) {
    
  }

  CBaseIGUTarcogLayer::CBaseIGUTarcogLayer( const CBaseIGUTarcogLayer& t_Layer ) : 
    CBaseTarcogLayer( t_Layer ) {
    m_Thickness = t_Layer.m_Thickness;
  }

  double CBaseIGUTarcogLayer::layerTemperature() {
    return ( m_Surface.at( Side::Front )->getTemperature() + m_Surface.at( Side::Back )->getTemperature() ) / 2;
  }

  double CBaseIGUTarcogLayer::getThickness() const {
    return m_Thickness;
  }

  double CBaseIGUTarcogLayer::getTemperature( const Side t_Position ) const {
    return getSurface( t_Position )->getTemperature();
  }

  double CBaseIGUTarcogLayer::J( const Side t_Position ) const {
    return getSurface( t_Position )->J();
  }

}