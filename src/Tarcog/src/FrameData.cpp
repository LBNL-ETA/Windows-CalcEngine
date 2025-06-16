#include "FrameData.hpp"

namespace Tarcog::ISO15099
{
    double dividerUValue(const DividerBodyPoly & poly, double uCenter, double gap)
    {
        return poly[0] + poly[1] * gap * 1000.0 + poly[2] * (gap * 1000.0) * (gap * 1000.0)
               + poly[3] * uCenter + poly[4] * uCenter * uCenter;
    }

    double frameEdgeUValue(const GenericFrame & c, double uCenter, double gap)
    {
        if(gap == 0)
        {
            return uCenter;
        }

        return c[0] + c[1] * uCenter + c[2] * uCenter * uCenter + c[3] * gap + c[4] * gap * gap;
    }

    double dividerEdgeUValue(const DividerEdgePoly & poly, double uCenter, double gap)
    {
        return poly[0] + poly[1] * gap * 1000.0 + poly[2] * uCenter + poly[3] * uCenter * uCenter;
    }
}   // namespace Tarcog::ISO15099