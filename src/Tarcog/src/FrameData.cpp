#include "FrameData.hpp"

namespace Tarcog::ISO15099
{
    double frameEdgeUValue(const GenericFrame & c, double uCenter, double gap)
    {
        if(gap == 0)
        {
            return uCenter;
        }

        return c[0] + c[1] * uCenter + c[2] * uCenter * uCenter + c[3] * gap + c[4] * gap * gap;
    }

    double dividerEdgeUValue(const GenericDivider & divider, double uCenter, double gap)
    {
        const auto& c = divider.Divider;
        return c[0] + c[1] * gap * 1000.0 + c[2] * uCenter + c[3] * uCenter * uCenter;
    }
}   // namespace Tarcog::ISO15099