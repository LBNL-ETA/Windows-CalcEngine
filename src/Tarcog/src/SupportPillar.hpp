#pragma once

#include "IGUGapLayer.hpp"


namespace Tarcog::ISO15099
{
    class UniversalSupportPillar : public CIGUGapLayer
    {
    public:
        UniversalSupportPillar(const CIGUGapLayer & layer,
                               double materialConductivity,
                               double cellArea);

    protected:
        [[nodiscard]] virtual double areaOfContact() = 0;

    private:
        void calculateConvectionOrConductionFlow() override;

        [[nodiscard]] double conductivityOfPillarArray();
        [[nodiscard]] virtual double singlePillarThermalResistance();

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
        double areaOfContact() override;

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
        double areaOfContact() override;

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
        double areaOfContact() override;

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
        double areaOfContact() override;
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
        double areaOfContact() override;

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
        double areaOfContact() override;

        double m_PillarLength;
    };

}   // namespace Tarcog::ISO15099
