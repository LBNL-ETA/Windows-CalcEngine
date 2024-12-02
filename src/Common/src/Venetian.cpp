
#include <algorithm>
#include "Venetian.hpp"

namespace FenestrationCommon::Venetian
{
    Geometry adjustSlatTiltAngle(const Geometry & t_Geometry)
    {
        Geometry result = t_Geometry;
        result.SlatTiltAngle = std::clamp(result.SlatTiltAngle, -89.99999, 89.99999);
        return result;
    }
}   // namespace FenestrationCommon