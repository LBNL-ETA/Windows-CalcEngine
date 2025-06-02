#include "FrameData.hpp"

namespace Tarcog::ISO15099
{
    double frameEdgeUValue(const FrameClass & c, double uCenter, double gap)
    {
        if(gap == 0)
        {
            return uCenter;
        }

        return c[0] + c[1] * uCenter + c[2] * uCenter * uCenter
               + c[3] * gap + c[4] * gap * gap;
    }
}   // namespace Tarcog::ISO15099