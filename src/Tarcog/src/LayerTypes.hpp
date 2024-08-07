#pragma once

#include <memory>

namespace Tarcog::ISO15099
{
    class CIGUSolidLayer;
    using SolidLayer = std::shared_ptr<CIGUSolidLayer>;

    class CIGUShadeLayer;
    using ShadeLayer = std::shared_ptr<CIGUShadeLayer>;

    class CIGUGapLayer;
    using GapLayer = std::shared_ptr<CIGUGapLayer>;

    class CIGUVentilatedGapLayer;
    using VentilatedGapLayer = std::shared_ptr<CIGUVentilatedGapLayer>;
}   // namespace Tarcog::ISO15099