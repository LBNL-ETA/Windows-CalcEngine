#include <WCECommon.hpp>
#include "WavelengthSpectrum.hpp"

namespace FenestrationCommon
{

    std::vector<double> generateSpectrum(size_t numOfVisibleBands, size_t numOfIRBands) {
        std::vector<double> result;
        CWavelengthRange solarRange{WavelengthRange::Solar};
        CWavelengthRange visRange{WavelengthRange::Visible};
        const auto deltaVis = (visRange.maxLambda() - visRange.minLambda()) / numOfVisibleBands;
        const auto deltaSolar = (solarRange.maxLambda() - visRange.maxLambda()) / numOfIRBands;
        result.emplace_back(solarRange.minLambda());
        for(auto i = 0u; i < numOfVisibleBands; ++i)
        {
            result.emplace_back(visRange.minLambda() + i * deltaVis);
        }
        for(auto i = 0u; i < numOfIRBands; ++i)
        {
            result.emplace_back(visRange.maxLambda() + i * deltaSolar);
        }

        result.emplace_back(solarRange.maxLambda());

        return result;
    }
}