#pragma once

#include <limits>


namespace Tarcog
{
    namespace TarcogConstants
    {
        constexpr double DEFAULT_WINDOW_WIDTH = 1;
        constexpr double DEFAULT_WINDOW_HEIGHT = 1;
        constexpr double DEFAULT_LAYER_THICKNESS = (std::numeric_limits<double>::max)();
        constexpr double DEFAULT_TILT = 90;
        constexpr double DEFAULT_ENV_EMISSIVITY = 1;
        constexpr double DEFAULT_FRACTION_OF_CLEAR_SKY = 1;
    }

    namespace IterationConstants
    {
        constexpr double RELAXATION_PARAMETER_MAX = 0.65;
        constexpr double RELAXATION_PARAMETER_MIN = 0.05;
        constexpr double RELAXATION_PARAMETER_STEP = 0.05;
        constexpr double CONVERGENCE_TOLERANCE = 1e-8;
        constexpr size_t NUMBER_OF_STEPS = 200;
        constexpr double RELAXATION_PARAMETER_AIRFLOW = 0.9;
        constexpr double RELAXATION_PARAMETER_AIRFLOW_MIN = 0.1;
        constexpr double RELAXATION_PARAMETER_AIRFLOW_STEP = 0.1;
        constexpr double CONVERGENCE_TOLERANCE_AIRFLOW = 1e-2;
    }

    namespace MaterialConstants
    {
        constexpr double GLASSDENSITY = 2500;
    }

    namespace DeflectionConstants
    {
        constexpr double YOUNGSMODULUS = 7.2e10;
        constexpr double POISONRATIO = 0.22;
    }
}
