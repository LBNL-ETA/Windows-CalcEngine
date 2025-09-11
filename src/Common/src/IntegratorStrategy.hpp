#pragma once

#include "Series.hpp"

namespace FenestrationCommon
{

    enum class IntegrationType
    {
        Rectangular,
        RectangularCentroid,
        Trapezoidal,
        TrapezoidalA,
        TrapezoidalB,
        PreWeighted
    };

    // Single non-virtual entry point that does NOT expose m_Series
    CSeries
      integrate(IntegrationType type, const CSeries & series, double normalizationCoeff = 1.0);

}   // namespace FenestrationCommon
