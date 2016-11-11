#include "IGUGapDeflection.hpp"
#include "FenestrationCommon.hpp"
#include "Surface.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace Tarcog {

  CIGUGapLayerDeflection::CIGUGapLayerDeflection( shared_ptr< CIGUGapLayer > t_GapLayer,
    const double t_Tini, const double t_Pini ) : CIGUGapLayer( *t_GapLayer ),
    m_Tini( t_Tini ), m_Pini( t_Pini ) {

  }

  double CIGUGapLayerDeflection::getPressure() {
    double Vini = m_Width * m_Height * m_Thickness;
    double modThickness = getThickness();
    double Vgap = m_Width * m_Height * modThickness;
    return m_Pini * Vini * layerTemperature() / ( m_Tini * Vgap );
  }

}
