#include <ranges>
#include <stdexcept>

#include <WCECommon.hpp>

#include "CR.hpp"

namespace Tarcog
{
    Humidity Humidity::fromFixed(const double humidity_value)
    {
        if(!isAllowed(humidity_value))
        {
            throw std::invalid_argument("Invalid humidity value");
        }

        return Humidity(humidity_value);
    }

    double Humidity::asDouble() const
    {
        return m_value;
    }

    Humidity::Humidity(const double humidity_value) : m_value(humidity_value)
    {}

    bool Humidity::isAllowed(const double AllowedHumidities)
    {
        static constexpr std::array allowedValues = {
          10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0};

        return std::ranges::any_of(allowedValues, [&](const double allowedValue) {
            return FenestrationCommon::isEqual(AllowedHumidities, allowedValue);
        });
    }
}   // namespace Tarcog
