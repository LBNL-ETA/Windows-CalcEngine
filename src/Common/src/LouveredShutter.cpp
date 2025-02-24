#include <cmath>

#include "LouveredShutter.hpp"

#include "MathFunctions.hpp"

namespace FenestrationCommon::LouveredShutter
{
    double thickness(const Geometry & t_Geometry)
    {
        return t_Geometry.SlatWidth
                 * std::cos(FenestrationCommon::radians(t_Geometry.SlatAngle))
               + t_Geometry.SlatThickness
                   * std::abs(std::sin(FenestrationCommon::radians(t_Geometry.SlatAngle)));
    }
}