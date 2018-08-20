#include "Environments.hpp"
#include "OutdoorEnvironment.hpp"
#include "IndoorEnvironment.hpp"

namespace Tarcog
{
    std::shared_ptr<CIndoorEnvironment>
      Tarcog::Environments::indoor( double roomAirTemperature, double roomPressure )
    {
        return std::make_shared<CIndoorEnvironment>(roomAirTemperature, roomPressure);
    }

    std::shared_ptr<COutdoorEnvironment>
      Environments::outdoor( double airTemperature,
							 double pressure,
							 double airSpeed,
							 double solarRadiation,
							 double skyTemperature,
							 SkyModel skyModel,
							 AirHorizontalDirection airDirection,
							 double fractionOfClearSky )
    {
        return std::make_shared<COutdoorEnvironment>(airTemperature,
                                                    pressure,
                                                    airSpeed,
                                                    solarRadiation,
                                                    airDirection,
                                                    skyTemperature,
                                                    skyModel,
                                                    fractionOfClearSky);
    }
}   // namespace Tarcog