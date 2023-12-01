#include "GasSpecification.hpp"

namespace Tarcog::ISO15099
{
    void GasSpecification::setTemperature(double temperature)
    {
        gas.setTemperatureAndPressure(temperature, pressure);
    }
}   // namespace Tarcog::ISO15099
