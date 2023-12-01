#pragma once

#include "IGUGapLayer.hpp"


namespace Tarcog::ISO15099
{
    //! Spacing between pillars
    struct PillarCellSpacing
    {
        PillarCellSpacing() = default;
        PillarCellSpacing(double sx, double sy);
        double Sx{0};
        double Sy{0};
    };

    class SupportPillar : public CIGUGapLayer
    {
    public:
        SupportPillar(const CIGUGapLayer & layer,
                      double materialConductivity,
                      const PillarCellSpacing & cell);

    protected:
        [[nodiscard]] virtual double areaOfContact() = 0;

    private:
        void calculateConvectionOrConductionFlow() override;

        [[nodiscard]] double conductivityOfPillarArray();
        [[nodiscard]] virtual double singlePillarThermalResistance();

        double m_MaterialConductivity;
        double m_CellArea;
    };

    class CylindricalPillar : public SupportPillar
    {
    public:
        CylindricalPillar(const CIGUGapLayer & layer,
                          double radius,
                          double materialConductivity,
                          const PillarCellSpacing & cell);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        double areaOfContact() override;

        double m_Radius;
    };

    class SphericalPillar : public SupportPillar
    {
    public:
        SphericalPillar(const CIGUGapLayer & layer,
                        double radiusOfContact,
                        double materialConductivity,
                        const PillarCellSpacing & cell);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        double areaOfContact() override;

        double m_RadiusOfContact;
    };

    class RectangularPillar : public SupportPillar
    {
    public:
        RectangularPillar(const CIGUGapLayer & layer,
                          double length,
                          double width,
                          double materialConductivity,
                          const PillarCellSpacing & cell);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        double areaOfContact() override;

        double m_PillarLength;
        double m_PillarWidth;
    };

    class TriangularPillar : public SupportPillar
    {
    public:
        TriangularPillar(const CIGUGapLayer & layer,
                         double length,
                         double materialConductivity,
                         const PillarCellSpacing & cell);
        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        double areaOfContact() override;
        double m_PillarLength;
    };

    class PentagonPillar : public SupportPillar
    {
    public:
        PentagonPillar(const CIGUGapLayer & layer,
                       double length,
                       double materialConductivity,
                       const PillarCellSpacing & cell);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        double areaOfContact() override;

        double m_PillarLength;
    };

}   // namespace Tarcog::ISO15099
