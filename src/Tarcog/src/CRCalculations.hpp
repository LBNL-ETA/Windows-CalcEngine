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

    inline const DewPointTable DefaultDewPoints = {
      {Humidity::H30(), 2.9}, {Humidity::H50(), 10.3}, {Humidity::H70(), 15.4}};

    //! Frame-only condensation resistance
    CRResult crf(const ISO15099::WindowVision & vision,
                 const DewPointTable & dewPoints = DefaultDewPoints);
}   // namespace Tarcog::CR
