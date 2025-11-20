#pragma once

#include <vector>
#include <numeric>

#include "CR.hpp"
#include "WindowVision.hpp"

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

//    template<typename Pos>
//    std::map<Pos, CRFrameContribution>
//      computeAverages(const std::map<Pos, CRFrameContribution> & items);

    template<typename Pos, typename Getter>
    std::map<Humidity, double> accumulateCRValues(const std::map<Pos, CRFrameContribution> & items,
                                                  Getter getValue)
    {
        std::map<Humidity, double> acc;

        for(const auto & [pos, item] : items)
        {
            for(const auto & cd : item.data)
            {
                acc[cd.humidity] += item.area * getValue(cd);
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
            sum += ISO15099::frameArea(frame);
        return sum;
    }

    template<typename Pos>
    double totalEdgeOfGlassArea(const std::map<Pos, ISO15099::Frame> & frames)
    {
        double sum = 0.0;
        for(const auto & [pos, frame] : frames)
            sum += ISO15099::edgeOfGlassArea(frame);
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
                continue;

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

    // -----------------------------------------------------------
    // 3) Templated crf/cre
    // -----------------------------------------------------------

    template<typename Pos>
    CRResult crf(const std::map<Pos, ISO15099::Frame> & frames)
    {
        const double totalArea = totalFrameArea(frames);
        if(totalArea <= 0.0)
            throw std::runtime_error("Total frame area is zero");

        const auto rawDeltas = rawDeltasFrame(frames);

        return {applyDewPointNormalization(rawDeltas, totalArea),
                crAverageNormalized(rawDeltas, totalArea)};
    }

    template<typename Pos>
    CRResult cre(const std::map<Pos, ISO15099::Frame> & frames)
    {
        const double totalArea = totalEdgeOfGlassArea(frames);
        if(totalArea <= 0.0)
            throw std::runtime_error("Total edge-of-glass area is zero");

        const auto rawDeltas = rawDeltasEdge(frames);

        return {applyDewPointNormalization(rawDeltas, totalArea),
                crAverageNormalized(rawDeltas, totalArea)};
    }

    // -----------------------------------------------------------
    // 4) WindowVision overloads (non-templates)
    // -----------------------------------------------------------

    // Existing CRG/CR/CRB remain unchanged
    CRResult crg(const ISO15099::WindowVision &, const DewPointSettings &, double);
    CRResult cr(const ISO15099::WindowVision &, const DewPointSettings &, double);
    CRResult crb(const ISO15099::WindowVision &, const DewPointSettings &, double);

}   // namespace Tarcog::CR
