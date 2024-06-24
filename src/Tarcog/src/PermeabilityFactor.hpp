#pragma once

namespace ThermalPermeability
{
    namespace Venetian
    {
        double maxAngle(double t_SlatSpacing, double t_MatThickness);

        double calculateRise(double t_Curvature, double t_SlatWidth);

        double calculateCurvature(double t_Rise, double t_SlatWidth);

        double frontOpenness(double t_TiltAngle,
                             double t_SlatSpacing,
                             double t_MatThickness,
                             double t_SlatCurvature,
                             double t_SlatWidth);
    }   // namespace Venetian

    namespace Perforated
    {
        enum class Type
        {
            Circular,
            Square,
            Rectangular
        };

        double frontOpenness(Type t_Type,
                             double t_SpacingX,
                             double t_SpacingY,
                             double t_DimensionX,
                             double t_DimensionY);
    }   // namespace Perforated

    namespace Woven
    {
        double frontOpenness(double t_Diameter, double t_Spacing);
    }
}   // namespace ThermalPermeability