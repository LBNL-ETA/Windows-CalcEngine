
#include <algorithm>
#include <cmath>

#include "Venetian.hpp"
#include "MathFunctions.hpp"

namespace FenestrationCommon::Venetian
{
    Geometry adjustSlatTiltAngle(const Geometry & t_Geometry)
    {
        Geometry result = t_Geometry;
        result.SlatTiltAngle = std::clamp(result.SlatTiltAngle, -89.99999, 89.99999);
        return result;
    }

    double thickness(const Geometry & t_Geometry)
    {
        return t_Geometry.SlatWidth * std::cos(radians(t_Geometry.SlatTiltAngle));
    }
}   // namespace FenestrationCommon::Venetian