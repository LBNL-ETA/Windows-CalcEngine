#include <ranges>
#include <algorithm>
#include <stdexcept>

#include <WCECommon.hpp>

#include "CRCalculations.hpp"
#include "CR.hpp"
#include "thermal/commonThermal.hpp"

namespace Tarcog::CR
{
    // Hidden defaults - internal linkage
    namespace
    {
        // Defaults in the documentation are given in Celsius. However, WindowsCalc-Engine will
        // always expect temperatures in Kelvins
        constexpr auto celsiusToKelvin = [](const double celsius) {
            return celsius + ConstantsData::KELVINCONV;
        };

        const double kDefaultDewPointTemperature{celsiusToKelvin(0.3)};
        const DewPointTable kDefaultDewPoints = {{Humidity::H30(), celsiusToKelvin(2.9)},
                                                 {Humidity::H50(), celsiusToKelvin(10.3)},
                                                 {Humidity::H70(), celsiusToKelvin(15.4)}};
    }   // namespace

    // =============================================================
    //   Helpers
    // =============================================================

    template<typename Getter>
    std::map<Humidity, double>
      accumulateCRValues(const std::map<FramePosition, CRFrameContribution> & items,
                         Getter getValue)
    {
        std::map<Humidity, double> acc;

        for(const auto & item : items | std::views::values)
        {
            for(const auto & cd : item.data)
            {
                acc[cd.humidity] += item.area * getValue(cd);
            }
        }

        return acc;
    }


    template<typename Getter, typename AreaGetter>
    std::map<Humidity, double> weightedDeltasGeneric(const ISO15099::WindowVision & vision,
                                                     Getter getValue,
                                                     AreaGetter getArea)
    {
        std::map<Humidity, double> acc;

        for(const auto & [pos, frame] : vision.frames())
        {
            if(!frame.frameData.condensationData.has_value())
            {
                continue;
            }

            const double area = getArea(pos, frame);
            const auto & cds = frame.frameData.condensationData.value();

            for(const auto & val : cds)
            {
                acc[val.humidity] += area * getValue(val);
            }
        }

        return acc;
    }

    CRResult combineMin2(const CRResult & a, const CRResult & b)
    {
        CRResult out;

        for(const auto & [h, v] : a.values)
        {
            auto it = b.values.find(h);
            if(it != b.values.end())
            {
                out.values[h] = std::min(v, it->second);
            }
        }

        out.average = std::min(a.average, b.average);

        return out;
    }

    template<typename First, typename... Rest>
    CRResult combineMin(const First & first, const Rest &... rest)
    {
        CRResult out = first;
        ((out = combineMin2(out, rest)), ...);
        return out;
    }

    std::map<Humidity, double> sum(const std::map<Humidity, double> & a,
                                   const std::map<Humidity, double> & b)
    {
        std::map<Humidity, double> out = a;
        for(const auto & [h, v] : b)
        {
            out[h] += v;
        }
        return out;
    }

    inline double crFormula(const double ratio)
    {
        static constexpr double CR_EXPONENT = 1.0 / 3.0;
        return 100.0 * (1.0 - std::pow(ratio, CR_EXPONENT));
    }

    // -------------------------------------------------------------
    // Lookup dew point by humidity (vector scan)
    // -------------------------------------------------------------
    double lookupDewPoint(const Humidity & hum, const DewPointTable & table)
    {
        const auto itr =
          std::ranges::find_if(table, [&](const auto & entry) { return entry.humidity == hum; });

        if(itr != table.end())
        {
            return itr->temperature;
        }

        throw std::runtime_error("Missing dew point for humidity: "
                                 + std::to_string(hum.asDouble()));
    }

    // -------------------------------------------------------------
    // Total frame area
    // -------------------------------------------------------------
    double totalFrameArea(const ISO15099::WindowVision & vision)
    {
        auto keysView = vision.frames() | std::views::keys;
        return std::accumulate(
          std::ranges::begin(keysView),
          std::ranges::end(keysView),
          0.0,
          [&vision](double sum, const auto & pos) { return sum + vision.frameArea(pos); });
    }

    std::map<Humidity, double> cogContribution(const double outsideTemperature,
                                               const double insideGlassTemp,
                                               const DewPointSettings & dps)
    {
        const auto kelvinToCelsius = [](const double temperature) {
            return temperature - ConstantsData::KELVINCONV;
        };
        std::map<Humidity, double> out;

        for(const auto & [humidity, temperature] : dps.dewPoints)
        {
            const double delta = std::max(
              0.0,
              kelvinToCelsius(temperature - insideGlassTemp + dps.dewPointTemperature)
                / kelvinToCelsius(temperature + dps.dewPointTemperature - outsideTemperature));
            out.emplace(humidity, delta);
        }

        return out;
    }

    std::map<Humidity, double> rawDeltasGlassAndEdge(const ISO15099::WindowVision & vision,
                                                     const DewPointSettings & dps,
                                                     const double outsideTemperature)
    {
        const auto glass = cogContribution(
          outsideTemperature, vision.getTemperatures(ISO15099::System::SHGC).back(), dps);

        const auto rawEdges = rawDeltasEdge(vision.frames());

        return sum(glass, rawEdges);
    }

    inline double normalizeCR(const double rawDelta, const double totalArea)
    {
        if(totalArea <= 0.0)
        {
            throw std::invalid_argument("normalizeCR: totalArea must be positive and nonzero");
        }

        return crFormula(rawDelta / totalArea);
    }

    std::map<Humidity, double> applyDewPointNormalization(const std::map<Humidity, double> & deltas,
                                                          const double totalArea)
    {
        std::map<Humidity, double> out;

        std::ranges::transform(deltas, std::inserter(out, out.end()), [totalArea](const auto & kv) {
            return std::make_pair(kv.first, normalizeCR(kv.second, totalArea));
        });

        return out;
    }

    double crAverage(const std::map<Humidity, double> & rawDeltas)
    {
        if(rawDeltas.empty())
        {
            throw std::runtime_error("No raw deltas available to compute CR average");
        }

        const double sum = std::accumulate(std::ranges::begin(rawDeltas | std::views::values),
                                           std::ranges::end(rawDeltas | std::views::values),
                                           0.0);

        return sum / static_cast<double>(rawDeltas.size());
    }

    double crAverageNormalized(const std::map<Humidity, double> & rawDeltas, const double totalArea)
    {
        return normalizeCR(crAverage(rawDeltas), totalArea);
    }

    DewPointSettings defaultDewPointSettings() noexcept
    {
        return {kDefaultDewPointTemperature, kDefaultDewPoints};
    }

    CRResult crge(const ISO15099::WindowVision & vision,
                  const DewPointSettings & dewPointSettings,
                  const double outsideTemperature)
    {
        const double area = vision.visionPercentage() * vision.area();
        const auto totals = rawDeltasGlassAndEdge(vision, dewPointSettings, outsideTemperature);

        return {applyDewPointNormalization(totals, area), crAverageNormalized(totals, area)};
    }

    // =============================================================
    //   PUBLIC API
    // =============================================================

    CRResult crdiv(const IWindow & window)
    {
        const double area = window.getDividerArea();
        if(area < 0.0)
        {
            throw std::runtime_error("Total divider area is less than zero");
        }

        const auto rawDeltas = rawDeltasDivider(window);

        return {applyDewPointNormalization(rawDeltas, area), crAverageNormalized(rawDeltas, area)};
    }

    CRResult crdive(const IWindow & window)
    {
        const double area = window.getDividerEdgeOfGlassArea();
        if(area < 0.0)
        {
            throw std::runtime_error("Total divider area is less than zero");
        }

        const auto rawDeltas = rawDeltasDividerEdge(window);

        return {applyDewPointNormalization(rawDeltas, area), crAverageNormalized(rawDeltas, area)};
    }

    CRResult crg(const ISO15099::WindowVision & vision,
                 const DewPointSettings & dewPointSettings,
                 const double outsideTemperature)
    {
        const double tInside = vision.getTemperatures(ISO15099::System::SHGC).back();

        const double area = vision.visionPercentage() * vision.area() - vision.edgeOfGlassArea();
        const auto raw = cogContribution(outsideTemperature, tInside, dewPointSettings);

        return {applyDewPointNormalization(raw, area), crAverageNormalized(raw, area)};
    }

    ////////////////////////////////////
    /////   CR
    ////////////////////////////////////

    template<typename Window, typename GetV1, typename GetV2, typename GetFrames>
    CRResult cr_common(const Window & window,
                       const DewPointSettings & dewPointSettings,
                       const double outsideTemperature,
                       GetV1 getVision1,
                       GetV2 getVision2,
                       GetFrames getFrames)
    {
        const auto crGlassEdge1 = crge(getVision1(window), dewPointSettings, outsideTemperature);
        const auto crGlassEdge2 = crge(getVision2(window), dewPointSettings, outsideTemperature);
        const auto crFrame = crf(getFrames(window));

        return combineMin(crFrame, crGlassEdge1, crGlassEdge2);
    }

    CRResult cr(const ISO15099::WindowSingleVision & window,
                const DewPointSettings & dewPointSettings,
                const double outsideTemperature)
    {
        return cr_common(
          window,
          dewPointSettings,
          outsideTemperature,
          [](auto const & w) -> decltype(auto) { return w.vision(); },
          [](auto const & w) -> decltype(auto) { return w.vision(); },
          [](auto const & w) -> decltype(auto) { return w.frames(); });
    }

    CRResult cr(const ISO15099::DualVisionHorizontal & window,
                const DewPointSettings & dewPointSettings,
                const double outsideTemperature)
    {
        return cr_common(
          window,
          dewPointSettings,
          outsideTemperature,
          [](auto const & w) -> decltype(auto) { return w.vision1(); },
          [](auto const & w) -> decltype(auto) { return w.vision2(); },
          [](auto const & w) -> decltype(auto) { return w.frames(); });
    }

    CRResult cr(const ISO15099::DualVisionVertical & window,
                const DewPointSettings & dewPointSettings,
                const double outsideTemperature)
    {
        return cr_common(
          window,
          dewPointSettings,
          outsideTemperature,
          [](auto const & w) -> decltype(auto) { return w.vision1(); },
          [](auto const & w) -> decltype(auto) { return w.vision2(); },
          [](auto const & w) -> decltype(auto) { return w.frames(); });
    }

    ////////////////////////////////////
    /////   CRB
    ////////////////////////////////////

    template<typename Window, typename GetV1, typename GetV2, typename GetFrames>
    CRResult crb_common(const Window & window,
                        const DewPointSettings & dewPointSettings,
                        const double outsideTemperature,
                        GetV1 getVision1,
                        GetV2 getVision2,
                        GetFrames getFrames)
    {
        const auto frames = getFrames(window);
        const CRResult frame = crf(frames);
        const CRResult edge = cre(frames);

        const auto glass1 = crg(getVision1(window), dewPointSettings, outsideTemperature);
        const auto glass2 = crg(getVision2(window), dewPointSettings, outsideTemperature);

        return combineMin(frame, edge, glass1, glass2);
    }

    CRResult crb(const ISO15099::WindowSingleVision & window,
                 const DewPointSettings & dewPointSettings,
                 const double outsideTemperature)
    {
        return crb_common(
          window,
          dewPointSettings,
          outsideTemperature,
          [](auto const & w) -> decltype(auto) { return w.vision(); },
          [](auto const & w) -> decltype(auto) { return w.vision(); },
          [](auto const & w) -> decltype(auto) { return w.frames(); });
    }

    CRResult crb(const ISO15099::DualVisionHorizontal & window,
                 const DewPointSettings & dewPointSettings,
                 const double outsideTemperature)
    {
        return crb_common(
          window,
          dewPointSettings,
          outsideTemperature,
          [](auto const & w) -> decltype(auto) { return w.vision1(); },
          [](auto const & w) -> decltype(auto) { return w.vision2(); },
          [](auto const & w) -> decltype(auto) { return w.frames(); });
    }

    CRResult crb(const ISO15099::DualVisionVertical & window,
                 const DewPointSettings & dewPointSettings,
                 const double outsideTemperature)
    {
        return crb_common(
          window,
          dewPointSettings,
          outsideTemperature,
          [](auto const & w) -> decltype(auto) { return w.vision1(); },
          [](auto const & w) -> decltype(auto) { return w.vision2(); },
          [](auto const & w) -> decltype(auto) { return w.frames(); });
    }
}   // namespace Tarcog::CR
