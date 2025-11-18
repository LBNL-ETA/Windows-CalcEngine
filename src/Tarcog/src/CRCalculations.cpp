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
        constexpr double kDefaultDewPointTemperature = 0.3;
        const DewPointTable kDefaultDewPoints = {
          {Humidity::H30(), 2.9}, {Humidity::H50(), 10.3}, {Humidity::H70(), 15.4}};
    }   // namespace

    // =============================================================
    //   Helpers
    // =============================================================

    template<typename Getter>
    std::map<Humidity, double> weightedDeltasGeneric(const ISO15099::WindowVision & vision,
                                                     Getter getValue)
    {
        std::map<Humidity, double> acc;

        for(const auto & [pos, frame] : vision.frames())
        {
            if(!frame.frameData.condensationData.has_value())
            {
                continue;
            }

            const double area = vision.area(pos);
            const auto & cds = frame.frameData.condensationData.value();

            for(const auto & val : cds)
            {
                acc[val.humidity] += area * getValue(val);
            }
        }

        return acc;
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
          [&vision](double sum, const auto & pos) { return sum + vision.area(pos); });
    }

    // -------------------------------------------------------------
    // Weighted deltas:   Σ ( area_i * CR_i(h) )
    //
    // Produces:
    //    map<Humidity, double>    raw weighted totals
    //
    // Python: "_weighted_cr_total"
    // -------------------------------------------------------------
    inline std::map<Humidity, double> weightedDeltas(const ISO15099::WindowVision & vision)
    {
        return weightedDeltasGeneric(vision, [](const auto & val) { return val.frame; });
    }

    inline std::map<Humidity, double> weightedDeltasEdge(const ISO15099::WindowVision & vision)
    {
        return weightedDeltasGeneric(vision, [](const auto & val) { return val.edge; });
    }

    std::map<Humidity, double> rawGlassDeltas(const double outsideTemperature,
                                              const double insideGlassTemp,
                                              const DewPointSettings & dps)
    {
        std::map<Humidity, double> out;

        for(const auto & entry : dps.dewPoints)
        {
            const double delta =
              std::max(0.0,
                       (entry.temperature - insideGlassTemp + dps.dewPointTemperature)
                         / (entry.temperature + dps.dewPointTemperature - outsideTemperature
                            + ConstantsData::KELVINCONV));
            out.emplace(entry.humidity, delta);
        }

        return out;
    }

    // -------------------------------------------------------------
    // Function to apply the CR_f formula (pure)
    //
    //    CR = 100 * (1 - (delta / totalArea)^(1/3))
    //
    // where:
    //    delta = Σ(A_i * CR_i(h))          -- raw weighted total
    //
    // Python:   _cr_formula(raw_delta, total_area)
    // -------------------------------------------------------------
    inline double normalizeCR(const double rawDelta, const double totalArea)
    {
        if(totalArea <= 0.0)
        {
            throw std::invalid_argument("normalizeCR: totalArea must be positive and nonzero");
        }

        return crFormula(rawDelta / totalArea);
    }

    // -------------------------------------------------------------
    // Normalize each humidity key (pure functional)
    // -------------------------------------------------------------
    std::map<Humidity, double> applyDewPointNormalization(const std::map<Humidity, double> & deltas,
                                                          const double totalArea)
    {
        std::map<Humidity, double> out;

        std::ranges::transform(deltas, std::inserter(out, out.end()), [totalArea](const auto & kv) {
            return std::make_pair(kv.first, normalizeCR(kv.second, totalArea));
        });

        return out;
    }

    // -------------------------------------------------------------
    // Compute Python-style CR_f average:
    //
    //   1. Compute average of raw deltas:
    //        avg_raw = mean( raw_deltas[h] )
    //
    //   2. Apply CR formula to this average:
    //        CR_avg = normalizeCR(avg_raw, totalArea)
    //
    // NOTE:
    //   This is NOT mean of per-humidity CR values!
    // -------------------------------------------------------------
    double computeCRfAverage(const std::map<Humidity, double> & rawDeltas, const double totalArea)
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

    // =============================================================
    //   PUBLIC API: Frame condensation resistance (CRf)
    // =============================================================
    CRResult crf(const ISO15099::WindowVision & vision)
    {
        const double totalArea = totalFrameArea(vision);
        if(totalArea <= 0.0)
        {
            throw std::runtime_error("Total frame area is zero");
        }

        const auto rawDeltas = weightedDeltas(vision);

        return {applyDewPointNormalization(rawDeltas, totalArea),
                computeCRfAverage(rawDeltas, totalArea)};
    }

    CRResult cre(const ISO15099::WindowVision & vision)
    {
        const double totalArea = totalFrameArea(vision);
        if(totalArea <= 0.0)
        {
            throw std::runtime_error("Total frame area is zero");
        }

        const auto rawDeltas = weightedDeltasEdge(vision);

        return {applyDewPointNormalization(rawDeltas, totalArea),
                computeCRfAverage(rawDeltas, totalArea)};
    }

    CRResult crg(const ISO15099::WindowVision & vision,
                 const DewPointSettings & dewPointSettings,
                 const double outsideTemperature)
    {
        // 1. Extract inside glass temperature from IGU system
        const double tInside =
          vision.getTemperatures(ISO15099::System::SHGC).back() - ConstantsData::KELVINCONV;

        // 2. Compute center-of-glass area from WindowVision
        // Note that this is strictly COG area and that edge of glass area should NOT be included
        const double area = vision.visionPercentage() * vision.area() - vision.edgeOfGlassArea();

        // 3. Compute deltas
        const auto raw = rawGlassDeltas(outsideTemperature, tInside, dewPointSettings);

        // 4. Normalize and average
        CRResult res;
        res.values = applyDewPointNormalization(raw, area);
        res.average = computeCRfAverage(raw, area);

        return res;
    }

}   // namespace Tarcog::CR
