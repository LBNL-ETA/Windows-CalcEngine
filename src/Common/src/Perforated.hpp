#pragma once

namespace FenestrationCommon::Perforated
{
    enum class Type
    {
        Circular,
        Square,
        Rectangular
    };

    struct Geometry
    {
        Type type;
        double SpacingX;
        double SpacingY;
        double DimensionX;
        double DimensionY;
    };
}   // namespace FenestrationCommon::Perforated
