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

    constexpr double DefaultDewPointTemperature = 0.3;

    //! Frame-only condensation resistance
    CRResult crf(const ISO15099::WindowVision & vision);
    CRResult cre(const ISO15099::WindowVision & vision);
    CRResult crg(const ISO15099::WindowVision & vision,
                 double DewPointTemperature,
                 const DewPointTable & dpt);

}   // namespace Tarcog::CR
