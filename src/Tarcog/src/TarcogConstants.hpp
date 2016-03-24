#ifndef TARCOGCONSTANTS_H
#define TARCOGCONSTANTS_H

namespace Tarcog {

  namespace TarcogConstants {
    const double DEFAULT_WINDOW_WIDTH = 1;
    const double DEFAULT_WINDOW_HEIGHT = 1;
    const double DEFAULT_TILT = 90;
    const double DEFAULT_ENV_EMISSIVITY = 1;
    const double DEFAULT_FRACTION_OF_CLEAR_SKY = 1;
  }

  namespace IterationConstants {
    const double RELAXATION_PARAMETER_MAX = 0.8;
    const double RELAXATION_PARAMETER_MIN = 0.2;
    const double RELAXATION_PARAMETER_STEP = 0.1;
    const double CONVERGENCE_TOLERANCE = 1e-6;
    const int NUMBER_OF_STEPS = 100;
    const double RELAXATION_PARAMETER_AIRFLOW = 0.9;
    const double CONVERGENCE_TOLERANCE_AIRFLOW = 1e-6;
  }

}



#endif