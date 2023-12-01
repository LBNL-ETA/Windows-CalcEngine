#pragma once

namespace Tarcog::ISO15099
{
    enum class BoundaryConditionsCoeffModel
    {
        CalculateH,
        HPrescribed,
        HcPrescribed
    };

    enum class Environment
    {
        Indoor,
        Outdoor
    };
}   // namespace Tarcog::ISO15099
