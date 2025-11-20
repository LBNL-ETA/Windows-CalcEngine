#pragma once

#include "FrameData.hpp"

namespace Tarcog::ISO15099
{
    [[nodiscard]] FrameData splitFrameWidth(const FrameData & frame);
    [[nodiscard]] Frame mergeFrameWidths(const Frame & a, const Frame & b, size_t edgeMultiplier = 1U);

    [[nodiscard]] double frameSHGC(double absorptance,
                                   double uValue,
                                   double projectedFrameDimension,
                                   double wettedLength,
                                   double hc);

    [[nodiscard]] double frameArea(const Frame & frame);
    [[nodiscard]] double wettedArea(const Frame & frame);
    [[nodiscard]] double edgeOfGlassArea(const Frame & frame);
}   // namespace Tarcog::ISO15099
