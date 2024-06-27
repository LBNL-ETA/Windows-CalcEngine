
#include <algorithm>
#include "Venetian.hpp"

namespace FenestrationCommon
{
    VenetianGeometry adjustSlatTiltAngle(const VenetianGeometry & t_Geometry)
    {
        VenetianGeometry result = t_Geometry;
        result.SlatTiltAngle = std::clamp(result.SlatTiltAngle, -89.99999, 89.99999);
        return result;
    }
}   // namespace FenestrationCommon