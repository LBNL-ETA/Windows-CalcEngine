#pragma once

#include <vector>
#include <numeric>

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

    std::map<FramePosition, CRFrameContribution>
      frameAreaContributions(const ISO15099::WindowVision & vision);
    std::map<FramePosition, CRFrameContribution>
      edgeAreasContributions(const ISO15099::WindowVision & vision);

    template<typename Pos>
    std::map<Pos, CRFrameContribution>
      frameAreaContributions(const std::map<Pos, ISO15099::Frame> & frames)
    {
        {
            std::map<Pos, CRFrameContribution> out;

            for(const auto & [pos, frame] : frames)
            {
                if(!frame.frameData.condensationData)
                    continue;

                CRFrameContribution c;
                c.area = ISO15099::frameArea(frame);
                c.data = *frame.frameData.condensationData;

                out[pos] = c;
            }

            return out;
        }
    }

    template<typename Pos>
    std::map<Pos, CRFrameContribution>
      edgeAreasContributions(const std::map<Pos, ISO15099::Frame> & frames)
    {
        std::map<Pos, CRFrameContribution> out;

        for(const auto & [pos, frame] : frames)
        {
            if(!frame.frameData.condensationData)
                continue;

            CRFrameContribution c;
            c.area = ISO15099::edgeOfGlassArea(frame);
            c.data = *frame.frameData.condensationData;

            out[pos] = c;
        }

        return out;
    }

    template<typename Pos>
    std::map<Pos, CRFrameContribution>
      computeAverages(const std::map<Pos, CRFrameContribution> & items)
    {
        std::map<Pos, CRFrameContribution> out;

        for(const auto & [pos, item] : items)
        {
            auto copy = item;
            copy.average.reset();

            if(!item.data.empty())
            {
                const auto [sumFrame, sumEdge] = std::accumulate(item.data.begin(),
                                                                 item.data.end(),
                                                                 std::pair{0.0, 0.0},
                                                                 [](auto acc, const auto & cd) {
                                                                     acc.first += cd.frame;
                                                                     acc.second += cd.edge;
                                                                     return acc;
                                                                 });

                const double n = static_cast<double>(item.data.size());
                copy.average = CRFrameContributionAverage{sumFrame / n, sumEdge / n};
            }

            out[pos] = copy;
        }

        return out;
    }

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
