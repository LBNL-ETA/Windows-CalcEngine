#include <memory>
#include "BaseIGUTarcogLayer.hpp"
#include "TarSurface.hpp"

namespace Tarcog {

  CBaseIGUTarcogLayer::CBaseIGUTarcogLayer( double const t_Thickness ) : m_Thickness( t_Thickness ) {
    
  }

  double CBaseIGUTarcogLayer::layerTemperature() {
    return ( m_FrontSurface->getTemperature() + m_BackSurface->getTemperature() ) / 2;
  }

  double CBaseIGUTarcogLayer::getThickness() const {
    return m_Thickness;
  }

  double CBaseIGUTarcogLayer::getTemperature( const SurfacePosition t_Position ) const {
    return getSurface( t_Position )->getTemperature();
  }

  double CBaseIGUTarcogLayer::J( const SurfacePosition t_Position ) const {
    return getSurface( t_Position )->J();
  }

}