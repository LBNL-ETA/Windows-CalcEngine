#pragma once

namespace Tarcog::ISO15099
{
    enum class AirVerticalDirection
    {
        None,
        Up,
        Down
    };

    enum class AirHorizontalDirection
    {
        None,
        Leeward,
        Windward
    };

    struct AirflowProperties
    {
        double airSpeed{0};
        AirVerticalDirection airVerticalDirection{AirVerticalDirection::None};
        AirHorizontalDirection airHorizontalDirection{AirHorizontalDirection::None};
        bool isVentilationForced{false};
    };
}