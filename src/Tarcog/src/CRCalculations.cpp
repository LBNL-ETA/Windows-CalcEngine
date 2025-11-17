#include "CRCalculations.hpp"
#include <stdexcept>
#include <numeric>
#include <ranges>

namespace Tarcog::CR
{
    // Lookup dew-point temperature for a given humidity
    static double lookupDewPoint(Humidity h, const DewPointTable & table)
    {
        for(const auto & dpt : table)
        {
            if(dpt.humidity == h)
            {
                return dpt.temperature;
            }
        }
        throw std::runtime_error("Missing dew point for humidity value");
    }

    // Compute frame CR (CRf), matching Python exactly
    CRResult crf(const ISO15099::WindowVision & vision, const DewPointTable & dewPoints)
    {
        CRResult result;

        // We iterate over all frames (head, sill, jambs)
        const auto & frames = vision.frames();

        for(const auto & [position, frame] : frames)
        {
            if(!frame.frameData.condensationData.has_value())
            {
                continue;
            }

            const auto & crData = frame.frameData.condensationData.value();

            // area of the frame (projected frame dimension * length)
            const double area = vision.area(position);

            for(const auto & c : crData)
            {
                const Humidity h = c.humidity;

                // weighted sum is: sum( CR_frame * area ) but per frame it's just multiplication
                result.values[h] += c.frame * area;
            }
        }

        // Now divide by total frame area
        double totalArea = 0.0;
        for(const auto & position : vision.frames() | std::views::keys)
        {
            totalArea += vision.area(position);
        }

        if(totalArea <= 0.0)
        {
            throw std::runtime_error("Total frame area is zero");
        }

        // Python formula:
        // CR_f = 100 * (1 - (weighted_delta / dew_point))
        for(auto & [humidity, weighted_sum] : result.values)
        {
            const double dewPoint = lookupDewPoint(humidity, dewPoints);

            const double delta = weighted_sum / totalArea;

            const double crValue = 100.0 * (1.0 - (delta / dewPoint));

            weighted_sum = crValue;
        }

        // Compute average
        double sum = 0.0;
        for(const auto & val : result.values | std::views::values)
        {
            sum += val;
        }

        result.average = sum / result.values.size();

        return result;
    }
}   // namespace Tarcog::CR
