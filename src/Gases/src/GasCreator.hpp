#pragma once

#include "GasData.hpp"

namespace Gases
{
    enum class GasDef
    {
        Air,
        Argon,
        Krypton,
        Xenon,
        VacuumMixture
    };

    CGasData getGasData(GasDef gasDef);

}   // namespace Gases
