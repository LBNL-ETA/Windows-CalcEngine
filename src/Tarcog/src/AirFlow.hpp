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
        AirflowProperties() = default;
        AirflowProperties(double mAirSpeed,
                          AirVerticalDirection mAirVerticalDirection,
                          AirHorizontalDirection mAirHorizontalDirection,
                          bool mIsVentilationForced);

        double m_AirSpeed{0};
        AirVerticalDirection m_AirVerticalDirection{AirVerticalDirection::None};
        AirHorizontalDirection m_AirHorizontalDirection{AirHorizontalDirection::None};
        bool m_IsVentilationForced{false};
    };
}