#pragma once

#include "IGUGapLayer.hpp"
#include "PillarData.hpp"


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

    class CylindricalPillarLayer : public UniversalSupportPillar
    {
    public:
        CylindricalPillarLayer(const CIGUGapLayer & layer, const CylindricalPillar & data);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        [[nodiscard]] double areaOfContact() override;

        double m_Radius;
    };

    class SphericalPillarLayer : public UniversalSupportPillar
    {
    public:
        SphericalPillarLayer(const CIGUGapLayer & layer, const SphericalPillar & data);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        [[nodiscard]] double areaOfContact() override;

        double m_RadiusOfContact;
    };

    class RectangularPillarLayer : public UniversalSupportPillar
    {
    public:
        RectangularPillarLayer(const CIGUGapLayer & layer, const RectangularPillar & data);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        [[nodiscard]] double areaOfContact() override;
        [[nodiscard]] double singlePillarThermalResistance() override;

        double m_PillarLength;
        double m_PillarWidth;
    };

    class TriangularPillarLayer : public UniversalSupportPillar
    {
    public:
        TriangularPillarLayer(const CIGUGapLayer & layer, const PolygonalPillar & data);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        [[nodiscard]] double areaOfContact() override;
        double m_PillarLength;
    };

    class PentagonPillarLayer : public UniversalSupportPillar
    {
    public:
        PentagonPillarLayer(const CIGUGapLayer & layer, const PolygonalPillar & data);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        [[nodiscard]] double areaOfContact() override;

        double m_PillarLength;
    };

    class HexagonPillarLayer : public UniversalSupportPillar
    {
    public:
        HexagonPillarLayer(const CIGUGapLayer & layer, const PolygonalPillar & data);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        [[nodiscard]] double areaOfContact() override;

        double m_PillarLength;
    };

    class LinearBearingPillarLayer : public UniversalSupportPillar
    {
    public:
        LinearBearingPillarLayer(const CIGUGapLayer & layer, const LinearBearingPillar & data);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        [[nodiscard]] double areaOfContact() override;
        [[nodiscard]] double singlePillarThermalResistance() override;

        double m_PillarLength;
        double m_PillarWidth;
    };

    class TruncatedConePillarLayer : public UniversalSupportPillar
    {
    public:
        TruncatedConePillarLayer(const CIGUGapLayer & layer, const TruncatedConePillar & data);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        [[nodiscard]] double areaOfContact() override;
        [[nodiscard]] double singlePillarThermalResistance() override;

        double m_Radius1;   // Radius at surface 1 (left)
        double m_Radius2;   // Radius at surface 2 (right)
    };

    class AnnulusCylinderPillarLayer : public UniversalSupportPillar
    {
    public:
        AnnulusCylinderPillarLayer(const CIGUGapLayer & layer, const AnnulusCylinderPillar & data);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        [[nodiscard]] double areaOfContact() override;
        [[nodiscard]] double singlePillarThermalResistance() override;

        double m_InnerRadius;
        double m_OuterRadius;
    };

}   // namespace Tarcog::ISO15099
