#pragma once

#include "FrameData.hpp"


namespace Tarcog::ISO15099
{
    [[nodiscard]] FrameData splitFrameWidth(const FrameData& frame);
    [[nodiscard]] double shgc(const FrameData& frame, double hc);

    [[nodiscard]] double projectedArea(const Frame& frame);
    [[nodiscard]] double wettedArea(const Frame& frame);
    [[nodiscard]] double edgeOfGlassArea(const Frame& frame);
}   // namespace Tarcog::ISO15099
