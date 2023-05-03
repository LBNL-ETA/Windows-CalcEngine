#pragma once

#include "GasData.hpp"

namespace Gases
{
    enum class GasDef
    {
        Air,
        Argon,
        Krypton,
        Xenon
    };

    CGasData getGasData(GasDef gasDef);

}   // namespace Gases
