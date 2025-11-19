#include <ranges>
#include <algorithm>
#include <numeric>
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


    // Core CR formula (shared by CRf, CRe and CRg)
    // Equivalent to Python _cr_formula(value, area)
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

    inline std::map<Humidity, double> weightedDeltasFrame(const ISO15099::WindowVision & vision)
    {
        auto frameAreaGetter = [&](auto pos, const auto &) { return vision.frameArea(pos); };

        return weightedDeltasGeneric(
          vision, [](const auto & val) { return val.frame; }, frameAreaGetter);
    }

    inline std::map<Humidity, double> weightedDeltasEdge(const ISO15099::WindowVision & vision)
    {
        auto edgeAreaGetter = [&](auto pos, const auto &) { return vision.edgeOfGlassArea(pos); };

        return weightedDeltasGeneric(
          vision, [](const auto & val) { return val.edge; }, edgeAreaGetter);
    }

    std::map<Humidity, double> rawGlassDeltas(const double outsideTemperature,
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
                / kelvinToCelsius(temperature + dps.dewPointTemperature
                   - outsideTemperature));
            out.emplace(humidity, delta);
        }

        return out;
    }

    std::map<Humidity, double> weightedGlassAndEdge(const ISO15099::WindowVision & vision,
                                                    const DewPointSettings & dps,
                                                    const double outsideTemperature)
    {
        // glass-only raw
        const auto glass = rawGlassDeltas(
          outsideTemperature, vision.getTemperatures(ISO15099::System::SHGC).back(), dps);
        
        const auto rawEdges = weightedDeltasEdge(vision);

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

    double computeCRAverage(const std::map<Humidity, double> & rawDeltas, const double totalArea)
    {
        if(rawDeltas.empty())
        {
            throw std::runtime_error("No raw deltas available to compute CRf average");
        }

        const double sum = std::accumulate(std::ranges::begin(rawDeltas | std::views::values),
                                           std::ranges::end(rawDeltas | std::views::values),
                                           0.0);

        const double avgRaw = sum / static_cast<double>(rawDeltas.size());
        return normalizeCR(avgRaw, totalArea);
    }

    DewPointSettings defaultDewPointSettings() noexcept
    {
        return {kDefaultDewPointTemperature, kDefaultDewPoints};
    }

    // TODO: Consider making this public
    CRResult crge(const ISO15099::WindowVision & vision,
                  const DewPointSettings & dewPointSettings,
                  double outsideTemperature)
    {
        const double area = vision.visionPercentage() * vision.area();
        const auto totals = weightedGlassAndEdge(vision, dewPointSettings, outsideTemperature);

        return {applyDewPointNormalization(totals, area), computeCRAverage(totals, area)};
    }

    // =============================================================
    //   PUBLIC API
    // =============================================================
    CRResult crf(const ISO15099::WindowVision & vision)
    {
        const double totalArea = totalFrameArea(vision);
        if(totalArea <= 0.0)
        {
            throw std::runtime_error("Total frame area is zero");
        }

        const auto rawDeltas = weightedDeltasFrame(vision);

        return {applyDewPointNormalization(rawDeltas, totalArea),
                computeCRAverage(rawDeltas, totalArea)};
    }

    CRResult cre(const ISO15099::WindowVision & vision)
    {
        const double totalArea = vision.edgeOfGlassArea();
        if(totalArea <= 0.0)
        {
            throw std::runtime_error("Total frame area is zero");
        }

        const auto rawDeltas = weightedDeltasEdge(vision);

        return {applyDewPointNormalization(rawDeltas, totalArea),
                computeCRAverage(rawDeltas, totalArea)};
    }

    CRResult crg(const ISO15099::WindowVision & vision,
                 const DewPointSettings & dewPointSettings,
                 const double outsideTemperature)
    {
        const double tInside = vision.getTemperatures(ISO15099::System::SHGC).back();

        const double area = vision.visionPercentage() * vision.area() - vision.edgeOfGlassArea();
        const auto raw = rawGlassDeltas(outsideTemperature, tInside, dewPointSettings);

        return {applyDewPointNormalization(raw, area), computeCRAverage(raw, area)};
    }

    CRResult cr(const ISO15099::WindowVision & vision,
                const DewPointSettings & dewPointSettings,
                const double outsideTemperature)
    {
        const auto crGlassEdge = crge(vision, dewPointSettings, outsideTemperature);
        const auto crFrame = crf(vision);


        return combineMin(crFrame, crGlassEdge);
    }
    CRResult crb(const ISO15099::WindowVision & vision,
                 const DewPointSettings & dewPointSettings,
                 const double outsideTemperature)
    {
        const CRResult frame = crf(vision);
        const CRResult edge = cre(vision);
        const CRResult glass = crg(vision, dewPointSettings, outsideTemperature);

        return combineMin(frame, edge, glass);
    }


}   // namespace Tarcog::CR
