#pragma once

#include <memory>
#include "LayerInterfaces.hpp"
#include "TarcogConstants.hpp"

namespace Tarcog
{
    namespace ISO15099
    {
        class CIndoorEnvironment;

        class COutdoorEnvironment;

        enum class SkyModel;

        // Singleton class to create indoor and outdoor environments
        class Environments
        {
        public:
            static std::shared_ptr<CIndoorEnvironment> indoor(double roomAirTemperature,
                                                              double roomPressure);

            static std::shared_ptr<COutdoorEnvironment>
              outdoor(double airTemperature,
                      double pressure,
                      double airSpeed,
                      double solarRadiation,
                      double skyTemperature,
                      SkyModel skyModel,
                      AirHorizontalDirection airDirection = AirHorizontalDirection::Windward,
                      double fractionOfClearSky = TarcogConstants::DEFAULT_FRACTION_OF_CLEAR_SKY);
        };

    }   // namespace ISO15099

}   // namespace Tarcog
