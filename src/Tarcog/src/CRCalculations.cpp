#include <ranges>
#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "CRCalculations.hpp"
#include "CR.hpp"

namespace Tarcog::CR
{

    // =============================================================
    //   Helpers
    // =============================================================

    // -------------------------------------------------------------
    // Lookup dew point by humidity (vector scan)
    // -------------------------------------------------------------
    double lookupDewPoint(const Humidity & h, const DewPointTable & table)
    {
        auto it =
          std::ranges::find_if(table, [&](const auto & entry) { return entry.humidity == h; });

        if(it != table.end())
        {
            return it->temperature;
        }

        throw std::runtime_error("Missing dew point for humidity");
    }

    // -------------------------------------------------------------
    // Total frame area
    // -------------------------------------------------------------
    double totalFrameArea(const ISO15099::WindowVision & vision)
    {
        double Area = 0.0;
        for(auto pos : vision.frames() | std::views::keys)
        {
            Area += vision.area(pos);
        }
        return Area;
    }

    // -------------------------------------------------------------
    // Weighted deltas:   Σ ( area_i * CR_i(h) )
    //
    // Produces:
    //    map<Humidity, double>    raw weighted totals
    //
    // Python: "_weighted_cr_total"
    // -------------------------------------------------------------
    std::map<Humidity, double> weightedDeltas(const ISO15099::WindowVision & vision)
    {
        std::map<Humidity, double> acc;

        for(const auto & [pos, frame] : vision.frames())
        {
            if(!frame.frameData.condensationData.has_value())
            {
                continue;
            }

            const double Area = vision.area(pos);
            const auto & cds = frame.frameData.condensationData.value();

            std::ranges::for_each(cds, [&acc, Area](const auto & c) {
                acc[c.humidity] += Area * c.frame;   // CRf uses c.frame contribution
            });
        }

        return acc;
    }

    // -------------------------------------------------------------
    // CR exponent (Python CR_EXPONENT = 1/3)
    // -------------------------------------------------------------
    static constexpr double CR_EXPONENT = 1.0 / 3.0;

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
    inline double normalizeCR(double rawDelta, double totalArea)
    {
        const double ratio = rawDelta / totalArea;
        return 100.0 * (1.0 - std::pow(ratio, CR_EXPONENT));
    }

    // -------------------------------------------------------------
    // Normalize each humidity key (pure functional)
    // -------------------------------------------------------------
    std::map<Humidity, double> applyDewPointNormalization(const std::map<Humidity, double> & deltas,
                                                          double totalArea)
    {
        std::map<Humidity, double> out;

        for(const auto & [h, raw] : deltas)
        {
            out[h] = normalizeCR(raw, totalArea);
        }

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
    //   Python applies CR formula to the average *raw delta*.
    // -------------------------------------------------------------
    double computeCRfAverage(const std::map<Humidity, double> & rawDeltas, double totalArea)
    {
        double sum = 0.0;
        for(const auto & [h, raw] : rawDeltas)
            sum += raw;

        const double avgRaw = sum / rawDeltas.size();
        return normalizeCR(avgRaw, totalArea);
    }

    // =============================================================
    //   PUBLIC API: Frame condensation resistance (CRf)
    // =============================================================
    CRResult crf(const ISO15099::WindowVision & vision, const DewPointTable & /*dewPoints*/)
    {
        CRResult out;

        // ---------------------------------------------------------
        // 1. Total area
        // ---------------------------------------------------------
        const double totalArea = totalFrameArea(vision);
        if(totalArea <= 0.0)
            throw std::runtime_error("Total frame area is zero");

        // ---------------------------------------------------------
        // 2. Raw weighted sums:   Σ(A_i * CR_i(h))
        // ---------------------------------------------------------
        auto rawDeltas = weightedDeltas(vision);

        // ---------------------------------------------------------
        // 3. Normalize each humidity using CR formula
        // ---------------------------------------------------------
        out.values = applyDewPointNormalization(rawDeltas, totalArea);

        // ---------------------------------------------------------
        // 4. Python-style average:
        //      CR_avg = normalizeCR(avg_raw_delta, totalArea)
        // ---------------------------------------------------------
        out.average = computeCRfAverage(rawDeltas, totalArea);

        return out;
    }

}   // namespace Tarcog::CR
