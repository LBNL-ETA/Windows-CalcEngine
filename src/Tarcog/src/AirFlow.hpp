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
        AirflowProperties(double mAirSpeed,
                          AirVerticalDirection mAirVerticalDirection,
                          AirHorizontalDirection mAirHorizontalDirection,
                          bool mIsVentilationForced);

        double m_AirSpeed;
        AirVerticalDirection m_AirVerticalDirection;
        AirHorizontalDirection m_AirHorizontalDirection;
        bool m_IsVentilationForced;
    };
}