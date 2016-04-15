#include "NIRRatio.hpp"
#include "Series.hpp"
#include "IntegratorStrategy.hpp"
#include "WavelengthRange.hpp"
#include "FenestrationCommon.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace SpectralAveraging {

  CNIRRatio::CNIRRatio( shared_ptr< const CSeries > t_SolarRadiation, const double lowLambda, const double highLambda ) {
    shared_ptr< CSeries > integratedSolar = t_SolarRadiation->integrate( IntegrationType::Trapezoidal );
    CWavelengthRangeFactory aWLFactory;
    shared_ptr< const CWavelengthRange > aSolarRange = aWLFactory.getWavelengthRange( WavelengthRange::Solar );
    
    double totSolar = integratedSolar->sum( aSolarRange->minLambda(), aSolarRange->maxLambda() );
    
    double totVisible = integratedSolar->sum( lowLambda, highLambda );
    m_Ratio = totVisible / totSolar;
  }

  double CNIRRatio::ratio() const {
    return m_Ratio;
  }

}