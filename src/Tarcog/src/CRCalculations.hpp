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

    // All this public interface is produced because of detailed tabular report. For general use
    // only one number is provided as the output (CR)

    std::vector<CRFrameContribution> frameAreaContributions(const ISO15099::WindowVision & vision);
    std::vector<CRFrameContribution> edgeAreasContributions(const ISO15099::WindowVision & vision);

    std::vector<CRFrameContribution> computeAverages(const std::vector<CRFrameContribution> & items);

    //! Frame-only condensation resistance
    CRResult crf(const ISO15099::WindowVision & vision);
    CRResult cre(const ISO15099::WindowVision & vision);
    CRResult crg(const ISO15099::WindowVision & vision,
                 const DewPointSettings & dewPointSettings,
                 double outsideTemperature);

    CRResult cr(const ISO15099::WindowVision & vision,
                const DewPointSettings & dewPointSettings,
                double outsideTemperature);

    CRResult crb(const ISO15099::WindowVision & vision,
                 const DewPointSettings & dewPointSettings,
                 double outsideTemperature);

}   // namespace Tarcog::CR
