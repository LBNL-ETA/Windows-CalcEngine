#pragma once

namespace Tarcog::ISO15099
{
    struct Glass
    {
        double thickness{0.0};
        double conductivity{0.0};
        double emissivity{0.0};
    };

    struct PillarMeasurement
    {
        double totalThickness{0.0};
        double conductivity{0.0};
        double temperatureSurface1{0.0};
        double temperatureSurface4{0.0};
        Glass glass1;
        Glass glass2;
    };

    [[nodiscard]] double cStar(const PillarMeasurement & pillar);
}   // namespace Tarcog::ISO15099