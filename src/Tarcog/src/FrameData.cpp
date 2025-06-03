#include "FrameData.hpp"

namespace Tarcog::ISO15099
{
    double dividerUValue(const GenericDivider & divider, double uCenter, double gap)
    {
        const auto& c = divider.Divider;
        const double x = gap * 1000.0;
        return c[4] + c[5] * x + c[6] * x * x + c[7] * uCenter + c[8] * uCenter * uCenter;
    }

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