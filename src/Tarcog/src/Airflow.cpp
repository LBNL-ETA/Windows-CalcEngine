#include "AirFlow.hpp"

namespace Tarcog::ISO15099
{
    AirflowProperties::AirflowProperties(double mAirSpeed,
                                         AirVerticalDirection mAirVerticalDirection,
                                         AirHorizontalDirection mAirHorizontalDirection,
                                         bool mIsVentilationForced) :
        m_AirSpeed(mAirSpeed),
        m_AirVerticalDirection(mAirVerticalDirection),
        m_AirHorizontalDirection(mAirHorizontalDirection),
        m_IsVentilationForced(mIsVentilationForced)
    {}
}
