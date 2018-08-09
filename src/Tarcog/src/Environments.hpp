#pragma once

#include <memory>
#include "LayerInterfaces.hpp"
#include "TarcogConstants.hpp"

namespace Tarcog
{
    class CIndoorEnvironment;
    class COutdoorEnvironment;
    enum class SkyModel;

    // Singleton class to create indoor and outdoor environments
    class Environments
    {
    public:
        static std::shared_ptr<CIndoorEnvironment> indoor( double roomAirTemperature,
														   double roomPressure );

        static std::shared_ptr<COutdoorEnvironment> outdoor(
			double airTemperature,
			double pressure,
			double airSpeed,
			double solarRadiation,
			AirHorizontalDirection airDirection,
			double skyTemperature,
			SkyModel skyModel,
			double fractionOfClearSky = TarcogConstants::DEFAULT_FRACTION_OF_CLEAR_SKY );
    };

}   // namespace Tarcog
