#pragma once

#include <vector>
#include <numeric>

#include "CR.hpp"
#include "WindowVision.hpp"
#include "WholeWindow.hpp"

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

    // -----------------------------------------------------------
    // 1) Forward declarations required by templates
    // -----------------------------------------------------------

    // Map-based contributions (templated versions will be defined later)
    template<typename Pos>
    std::map<Pos, CRFrameContribution>
      frameAreaContributions(const std::map<Pos, ISO15099::Frame> & frames);

    template<typename Pos>
    std::map<Pos, CRFrameContribution>
      edgeAreasContributions(const std::map<Pos, ISO15099::Frame> & frames);

    // Weighted deltas helpers (templated)
    template<typename Pos>
    std::map<Humidity, double> rawDeltasFrame(const std::map<Pos, ISO15099::Frame> & frames);

    template<typename Pos>
    std::map<Humidity, double> rawDeltasEdge(const std::map<Pos, ISO15099::Frame> & frames);

    // Core functions used inside crf/cre templates
    std::map<Humidity, double> applyDewPointNormalization(const std::map<Humidity, double> & deltas,
                                                          double totalArea);

    double crAverageNormalized(const std::map<Humidity, double> & deltas, double totalArea);

    template<typename Pos>
    double totalFrameArea(const std::map<Pos, ISO15099::Frame> & frames);

    template<typename Pos>
    double totalEdgeOfGlassArea(const std::map<Pos, ISO15099::Frame> & frames);

    template<typename Getter>
    std::map<Humidity, double> accumulateCRValue(const CRFrameContribution & item, Getter getValue)
    {
        std::map<Humidity, double> acc;
        for(const auto & cd : item.data)
        {
            acc[cd.humidity] += item.area * getValue(cd);
        }
        return acc;
    }

    template<typename Pos, typename Getter>
    std::map<Humidity, double> accumulateCRValues(const std::map<Pos, CRFrameContribution> & items,
                                                  Getter getValue)
    {
        std::map<Humidity, double> acc;
        for(const auto & [pos, item] : items)
        {
            const auto partial = accumulateCRValue(item, getValue);
            for(const auto & [h, v] : partial)
            {
                acc[h] += v;
            }
        }
        return acc;
    }

    // -----------------------------------------------------------
    // 2) Template definitions
    // -----------------------------------------------------------

    template<typename Pos>
    double totalFrameArea(const std::map<Pos, ISO15099::Frame> & frames)
    {
        double sum = 0.0;
        for(const auto & [pos, frame] : frames)
        {
            sum += ISO15099::frameArea(frame);
        }
        return sum;
    }

    template<typename Pos>
    double totalEdgeOfGlassArea(const std::map<Pos, ISO15099::Frame> & frames)
    {
        double sum = 0.0;
        for(const auto & [pos, frame] : frames)
        {
            sum += ISO15099::edgeOfGlassArea(frame);
        }
        return sum;
    }

    template<typename Pos>
    std::map<Humidity, double> rawDeltasFrame(const std::map<Pos, ISO15099::Frame> & frames)
    {
        const auto items = frameAreaContributions(frames);
        return accumulateCRValues(items, [](const auto & cd) { return cd.frame; });
    }

    template<typename Pos>
    std::map<Humidity, double> rawDeltasEdge(const std::map<Pos, ISO15099::Frame> & frames)
    {
        const auto items = edgeAreasContributions(frames);
        return accumulateCRValues(items, [](const auto & cd) { return cd.edge; });
    }

    template<typename Pos>
    std::map<Pos, CRFrameContribution>
      frameAreaContributions(const std::map<Pos, ISO15099::Frame> & frames)
    {
        std::map<Pos, CRFrameContribution> out;
        for(const auto & [pos, frame] : frames)
        {
            if(!frame.frameData.condensationData)
            {
                continue;
            }

            CRFrameContribution c;
            c.area = ISO15099::frameArea(frame);
            c.data = *frame.frameData.condensationData;
            out[pos] = c;
        }
        return out;
    }

    template<typename Pos>
    std::map<Pos, CRFrameContribution>
      edgeAreasContributions(const std::map<Pos, ISO15099::Frame> & frames)
    {
        std::map<Pos, CRFrameContribution> out;
        for(const auto & [pos, frame] : frames)
        {
            if(!frame.frameData.condensationData)
            {
                continue;
            }

            CRFrameContribution c;
            c.area = ISO15099::edgeOfGlassArea(frame);
            c.data = *frame.frameData.condensationData;
            out[pos] = c;
        }
        return out;
    }

    inline std::optional<CRFrameContribution> dividerAreaContribution(const IWindow & window)
    {
        if(!window.divider())
        {
            return std::nullopt;
        }

        CRFrameContribution c;
        c.area = window.getDividerArea();
        c.data = *window.divider()->frame.condensationData;
        return c;
    }

    inline std::optional<CRFrameContribution> dividerEdgeAreaContribution(const IWindow & window)
    {
        if(!window.divider())
        {
            return std::nullopt;
        }

        CRFrameContribution c;
        c.area = window.getDividerEdgeOfGlassArea();
        c.data = *window.divider()->frame.condensationData;
        return c;
    }

    inline std::map<Humidity, double> rawDeltasDivider(const IWindow & window)
    {
        const auto item = dividerAreaContribution(window);
        return accumulateCRValue(item.value(), [](const auto & cd) { return cd.frame; });
    }

    inline std::map<Humidity, double> rawDeltasDividerEdge(const IWindow & window)
    {
        const auto item = dividerEdgeAreaContribution(window);
        return accumulateCRValue(item.value(), [](const auto & cd) { return cd.edge; });
    }

    inline CRFrameContribution computeAverage(const CRFrameContribution & item)
    {
        CRFrameContribution copy = item;
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

            const auto n = static_cast<double>(item.data.size());
            copy.average = CRFrameContributionAverage{sumFrame / n, sumEdge / n};
        }

        return copy;
    }

    template<typename Pos>
    std::map<Pos, CRFrameContribution>
      computeAverages(const std::map<Pos, CRFrameContribution> & items)
    {
        std::map<Pos, CRFrameContribution> out;

        for(const auto & [pos, item] : items)
        {
            out[pos] = computeAverage(item);
        }

        return out;
    }

    std::optional<CRResult> crdiv(const IWindow & window);

    std::optional<CRResult> crdive(const IWindow & window);

    // -----------------------------------------------------------
    // 3) Templated crf/cre
    // -----------------------------------------------------------

    template<typename Pos>
    CRResult crf(const std::map<Pos, ISO15099::Frame> & frames)
    {
        const double totalArea = totalFrameArea(frames);
        if(totalArea <= 0.0)
        {
            throw std::runtime_error("Total frame area is zero");
        }

        const auto rawDeltas = rawDeltasFrame(frames);

        return {applyDewPointNormalization(rawDeltas, totalArea),
                crAverageNormalized(rawDeltas, totalArea)};
    }

    template<typename Pos>
    CRResult cre(const std::map<Pos, ISO15099::Frame> & frames)
    {
        const double totalArea = totalEdgeOfGlassArea(frames);
        if(totalArea <= 0.0)
        {
            throw std::runtime_error("Total edge-of-glass area is zero");
        }

        const auto rawDeltas = rawDeltasEdge(frames);

        return {applyDewPointNormalization(rawDeltas, totalArea),
                crAverageNormalized(rawDeltas, totalArea)};
    }

    // -----------------------------------------------------------
    // 4) WindowVision overloads (non-templates)
    // -----------------------------------------------------------

    // Existing CRG/CR/CRB remain unchanged
    CRResult crg(const ISO15099::WindowVision & window,
                 const DewPointSettings & dewPointSetting,
                 double outsideTemperature);

    CRResult cr(const ISO15099::WindowSingleVision & window,
                const DewPointSettings & dewPointSetting,
                double outsideTemperature);
    CRResult cr(const ISO15099::DualVisionHorizontal & window,
                const DewPointSettings & dewPointSetting,
                double outsideTemperature);
    CRResult cr(const ISO15099::DualVisionVertical & window,
                const DewPointSettings & dewPointSetting,
                double outsideTemperature);

    CRResult crb(const ISO15099::WindowSingleVision & window,
                 const DewPointSettings & dewPointSettings,
                 double outsideTemperature);
    CRResult crb(const ISO15099::DualVisionHorizontal & window,
                 const DewPointSettings & dewPointSettings,
                 double outsideTemperature);
    CRResult crb(const ISO15099::DualVisionVertical & window,
                 const DewPointSettings & dewPointSettings,
                 double outsideTemperature);
}   // namespace Tarcog::CR
