#include "TarIGUGapDeflection.hpp"
#include "FenestrationCommon.hpp"
#include "TarSurface.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace Tarcog {

  CTarIGUGapLayerDeflection::CTarIGUGapLayerDeflection( shared_ptr< CTarIGUGapLayer > t_GapLayer,
    const double t_Tini, const double t_Pini ) : CTarIGUGapLayer( *t_GapLayer ), m_GapLayer( t_GapLayer ), 
    m_Tini( t_Tini ), m_Pini( t_Pini ) {

  }

  double CTarIGUGapLayerDeflection::getPressure() {
    double Vini = m_Width * m_Height * m_Thickness;
    double modThickness = getThickness();
    double Vgap = m_Width * m_Height * modThickness;
    return m_Pini * Vini * layerTemperature() / ( m_Tini * Vgap );
  }

}
