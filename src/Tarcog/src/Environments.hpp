#pragma once

#include <memory>

#include "TarcogConstants.hpp"
#include "AirFlow.hpp"


namespace Tarcog::ISO15099
{
    class CIndoorEnvironment;
    class COutdoorEnvironment;

    enum class SkyModel;

    // Singleton class to create indoor and outdoor environments
    class Environments
    {
    public:
        static std::shared_ptr<CIndoorEnvironment> indoor(double roomAirTemperature,
                                                          double roomPressure = 101325);

        static std::shared_ptr<COutdoorEnvironment>
          outdoor(double airTemperature,
                  double airSpeed,
                  double solarRadiation,
                  double skyTemperature,
                  SkyModel skyModel,
                  double pressure = 101325,
                  AirHorizontalDirection airDirection = AirHorizontalDirection::Windward,
                  double fractionOfClearSky = TarcogConstants::DEFAULT_FRACTION_OF_CLEAR_SKY);
    };

}   // namespace Tarcog::ISO15099
