#pragma once

#include "IGUGapLayer.hpp"

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

    class SupportPillarMeasured : public CIGUGapLayer
    {
    public:
        explicit SupportPillarMeasured(const PillarMeasurement & pillar);

    private:
        void calculateConvectionOrConductionFlow() override;

        double m_MeasuredConductance;
    };
}   // namespace Tarcog::ISO15099