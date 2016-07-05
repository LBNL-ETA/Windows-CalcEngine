#include <memory>
#include "BaseIGUTarcogLayer.hpp"
#include "TarSurface.hpp"
#include "FenestrationCommon.hpp"

using namespace FenestrationCommon;

namespace Tarcog {

  CBaseIGUTarcogLayer::CBaseIGUTarcogLayer( double const t_Thickness ) : m_Thickness( t_Thickness ) {
    
  }

  double CBaseIGUTarcogLayer::layerTemperature() {
    return ( m_FrontSurface->getTemperature() + m_BackSurface->getTemperature() ) / 2;
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