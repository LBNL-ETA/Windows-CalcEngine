#pragma once

#include <vector>
#include "CR.hpp"             // Humidity, CRResult, CRResults
#include "WindowVision.hpp"   // For geometry and frame access

namespace Tarcog::CR
{
    struct DewPoint
    {
        Humidity humidity;
        double temperature;
    };

    using DewPointTable = std::vector<DewPoint>;

    struct DewPointSettings
    {
        double dewPointTemperature;
        DewPointTable dewPoints;
    };

    DewPointSettings defaultDewPointSettings() noexcept;

    //! Frame-only condensation resistance
    CRResult crf(const ISO15099::WindowVision & vision);
    CRResult cre(const ISO15099::WindowVision & vision);
    CRResult crg(const ISO15099::WindowVision & vision, const DewPointSettings & dewPointSettings);

}   // namespace Tarcog::CR
