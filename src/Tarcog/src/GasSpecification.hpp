#pragma once

#include "AirFlow.hpp"

#include <WCEGases.hpp>

namespace Tarcog::ISO15099
{
    struct GasSpecification
    {
        GasSpecification() = default;

        void setTemperature(double temperature);

        double pressure{Gases::DefaultPressure};
        AirflowProperties airflowProperties{};

        Gases::CGas gas{Gases::CGas()};
    };
}   // namespace Tarcog::ISO15099