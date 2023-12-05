#pragma once

#include "IGUGapLayer.hpp"


namespace Tarcog::ISO15099
{
    enum class CellSpacingType
    {
        Square,
        ShiftedSquare,
        ShiftedRotatedSquare
    };

    [[nodiscard]] double cellArea(CellSpacingType type, double r);

    class UniversalSupportPillar : public CIGUGapLayer
    {
    public:
        ~UniversalSupportPillar() override = default;
        UniversalSupportPillar(const CIGUGapLayer & layer,
                               double materialConductivity,
                               double cellArea);

    protected:
        [[nodiscard]] virtual double areaOfContact() = 0;
        [[nodiscard]] virtual double singlePillarThermalResistance();

        [[nodiscard]] double materialConductivity() const;

    private:
        void calculateConvectionOrConductionFlow() override;

        [[nodiscard]] double conductivityOfPillarArray();

        double m_MaterialConductivity;
        double m_CellArea;
    };

    class CylindricalPillar : public UniversalSupportPillar
    {
    public:
        CylindricalPillar(const CIGUGapLayer & layer,
                          double radius,
                          double materialConductivity,
                          double cellArea);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        [[nodiscard]] double areaOfContact() override;

        double m_Radius;
    };

    class SphericalPillar : public UniversalSupportPillar
    {
    public:
        SphericalPillar(const CIGUGapLayer & layer,
                        double radiusOfContact,
                        double materialConductivity,
                        double cellArea);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        [[nodiscard]] double areaOfContact() override;

        double m_RadiusOfContact;
    };

    class RectangularPillar : public UniversalSupportPillar
    {
    public:
        RectangularPillar(const CIGUGapLayer & layer,
                          double length,
                          double width,
                          double materialConductivity,
                          double cellArea);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        [[nodiscard]] double areaOfContact() override;
        [[nodiscard]] double singlePillarThermalResistance() override;

        double m_PillarLength;
        double m_PillarWidth;
    };

    class TriangularPillar : public UniversalSupportPillar
    {
    public:
        TriangularPillar(const CIGUGapLayer & layer,
                         double length,
                         double materialConductivity,
                         double cellArea);
        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        [[nodiscard]] double areaOfContact() override;
        double m_PillarLength;
    };

    class PentagonPillar : public UniversalSupportPillar
    {
    public:
        PentagonPillar(const CIGUGapLayer & layer,
                       double length,
                       double materialConductivity,
                       double cellArea);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        [[nodiscard]] double areaOfContact() override;

        double m_PillarLength;
    };

    class HexagonPillar : public UniversalSupportPillar
    {
    public:
        HexagonPillar(const CIGUGapLayer & layer,
                      double length,
                      double materialConductivity,
                      double cellArea);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        [[nodiscard]] double areaOfContact() override;

        double m_PillarLength;
    };

    class LinearBearingPillar : public UniversalSupportPillar
    {
    public:
        LinearBearingPillar(const CIGUGapLayer & layer,
                            double length,
                            double width,
                            double materialConductivity,
                            double cellArea);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        [[nodiscard]] double areaOfContact() override;
        [[nodiscard]] double singlePillarThermalResistance() override;

        double m_PillarLength;
        double m_PillarWidth;
    };

    class TruncatedConePillar : public UniversalSupportPillar
    {
    public:
        TruncatedConePillar(const CIGUGapLayer & layer,
                            double radius1,
                            double radius2,
                            double materialConductivity,
                            double cellArea);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        [[nodiscard]] double areaOfContact() override;
        [[nodiscard]] double singlePillarThermalResistance() override;

        double m_Radius1; // Radius at surface 1 (left)
        double m_Radius2; // Radius at surface 2 (right)
    };

    class AnnulusCylinderPillar : public UniversalSupportPillar
    {
    public:
        AnnulusCylinderPillar(const CIGUGapLayer & layer,
                              double innerRadius,
                              double outerRadius,
                              double materialConductivity,
                              double cellArea);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        [[nodiscard]] double areaOfContact() override;
        [[nodiscard]] double singlePillarThermalResistance() override;

        double m_InnerRadius;
        double m_OuterRadius;
    };

}   // namespace Tarcog::ISO15099
