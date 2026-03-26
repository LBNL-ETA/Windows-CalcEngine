#pragma once

#include "PillarData.hpp"


namespace Tarcog::ISO15099
{
    enum class CellSpacingType
    {
        None,
        Square,
        ShiftedSquare,
        ShiftedRotatedSquare,
        NumberOfPillarsPerArea
    };

    [[nodiscard]] double pillarCellArea(CellSpacingType type, double sp);

}   // namespace Tarcog::ISO15099
