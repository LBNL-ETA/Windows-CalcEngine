#pragma once

namespace Tarcog::ISO15099
{
    enum class PolygonType
    {
        Triangle,
        Pentagon,
        Hexagon
    };

    struct PillarData
    {
        double height{0.1};
        double materialConductivity{1.0};
        double cellArea{1.0};
    };

    struct CylindricalPillar : PillarData
    {
        double radius{0.5};
    };

    struct SphericalPillar : PillarData
    {
        double radiusOfContact{0.5};
    };

    struct RectangularPillar : PillarData
    {
        double length{0.5};
        double width{0.5};
    };

    struct PolygonalPillar : PillarData
    {
        double length{0.5};
        PolygonType type{PolygonType::Triangle};
    };

    struct LinearBearingPillar : PillarData
    {
        double length{0.5};
        double width{0.5};
    };

    struct TruncatedConePillar : PillarData
    {
        double radius1{0.5};
        double radius2{0.7};
    };

    struct AnnulusCylinderPillar : PillarData
    {
        double innerRadius{0.25};
        double outerRadius{0.5};
    };

    struct CShapedCylinderPillar : PillarData
    {
        double innerRadius{0.25};
        double outerRadius{0.5};
        double fractionCovered{0.35};
    };
}   // namespace Tarcog::ISO15099