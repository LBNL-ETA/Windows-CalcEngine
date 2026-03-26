#pragma once

#include <memory>

namespace Tarcog::ISO15099
{
    class CIGUSolidLayer;
    using SolidLayer = std::shared_ptr<CIGUSolidLayer>;

    using ShadeLayer = std::shared_ptr<CIGUSolidLayer>;

    class CIGUGapLayer;
    using GapLayer = std::shared_ptr<CIGUGapLayer>;

    using VentilatedGapLayer = std::shared_ptr<CIGUGapLayer>;
}   // namespace Tarcog::ISO15099