#include <cmath>

#include "LouveredShutter.hpp"

#include "MathFunctions.hpp"

namespace FenestrationCommon::LouveredShutter
{
    double thickness(const Geometry & t_Geometry)
    {
        return t_Geometry.SlatWidth * std::cos(radians(t_Geometry.SlatAngle))
               + t_Geometry.SlatThickness * std::abs(std::sin(radians(t_Geometry.SlatAngle)));
    }
}   // namespace FenestrationCommon::LouveredShutter