#pragma once

#include <functional>
#include <optional>

#include "IGUGapLayer.hpp"
#include "PillarData.hpp"


namespace Tarcog::ISO15099
{
    enum class CellSpacingType
    {
        None,
        Square,
        ShiftedSquare,
        ShiftedRotatedSquare,
        NumberOfPillarsPerArea
    };

    [[nodiscard]] double pillarCellArea(CellSpacingType type, double sp);

    class PillarGapLayer : public CIGUGapLayer
    {
    public:
        using AreaFn = std::function<double()>;
        using ResistanceFn = std::function<double(double kHot,
                                                  double kCold,
                                                  double kPillar,
                                                  double height,
                                                  double area)>;

        // Computed-conductance pillar
        PillarGapLayer(const CIGUGapLayer & gapLayer,
                       double materialConductivity,
                       double cellArea,
                       AreaFn areaOfContactFn,
                       ResistanceFn thermalResistanceFn);

        // Measured-conductance pillar
        PillarGapLayer(const CIGUGapLayer & gapLayer,
                       double measuredConductance);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        void calculateConvectionOrConductionFlow() override;
        [[nodiscard]] double conductivityOfPillarArray();

        double m_MaterialConductivity{0};
        double m_CellArea{0};
        AreaFn m_AreaOfContactFn;
        ResistanceFn m_ThermalResistanceFn;

        std::optional<double> m_MeasuredConductance;
    };

}   // namespace Tarcog::ISO15099
