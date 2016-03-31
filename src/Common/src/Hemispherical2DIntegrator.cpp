#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#include "Hemispherical2DIntegrator.hpp"
#include "Series.hpp"
#include "IntegratorStrategy.hpp"
#include "MathFunctions.hpp"

using namespace std;

namespace FenestrationCommon {

  // Performs hemispherical 2D integration
  CHemispherical2DIntegrator::CHemispherical2DIntegrator( shared_ptr< CSeries > t_Series, const IntegrationType t_IntegrationType ) {
    // vector< double > xValues;
    vector< double > sinCosAngle;
    CSeries aResultValues = CSeries();
    for( size_t i = 0; i < t_Series->size(); ++i ) {
      double angle = radians( ( *t_Series )[ i ]->x() );
      double value = ( *t_Series )[ i ]->value();

      double sinCos = sin( angle ) * cos( angle );

      aResultValues.addProperty( angle, value * sinCos );
    }

    aResultValues.sort();

    shared_ptr< CSeries > integrated = aResultValues.integrate( t_IntegrationType );
    m_Value = 2 * integrated->sum();
  };

  double CHemispherical2DIntegrator::value() const {
    return m_Value;
  };

}