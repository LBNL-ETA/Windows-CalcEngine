#include <memory>
#include "BaseIGUTarcogLayer.hpp"
#include "TarSurface.hpp"

namespace Tarcog {

  CBaseIGUTarcogLayer::CBaseIGUTarcogLayer( double const t_Thickness ) : m_Thickness( t_Thickness ) {
    
  }

  double CBaseIGUTarcogLayer::layerTemperature() const {
    return ( m_FrontSurface->getTemperature() + m_BackSurface->getTemperature() ) / 2;
  };

  double CBaseIGUTarcogLayer::getThickness () const {
    return m_Thickness;
  };

}