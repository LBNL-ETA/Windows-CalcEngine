#pragma once

#include <memory>

#include <WCEGases.hpp>

#include "IGUGapLayer.hpp"


namespace Tarcog::ISO15099
{
    struct VentilatedGapState
    {
        VentilatedGapState() = default;
        VentilatedGapState(double inletTemperature, double outletTemperature);
        double inletTemperature{0};
        double outletTemperature{0};
    };

    //! @brief Storage for forced ventilation properties.
    //!
    //! @property speed - Speed at which air is flowing into the gap
    //! @property temperature - Temperature at which air will be coming into the gap
    struct ForcedVentilation
    {
        ForcedVentilation(double speed, double temperature);
        double speed{0};
        double temperature{0};
    };

    class CIGUVentilatedGapLayer : public CIGUGapLayer
    {
    public:
        explicit CIGUVentilatedGapLayer(const std::shared_ptr<CIGUGapLayer> & t_Layer);
        CIGUVentilatedGapLayer(const std::shared_ptr<CIGUGapLayer> & t_Layer,
                               double forcedVentilationInletTemperature,
                               double forcedVentilationInletSpeed);

        double averageLayerTemperature() override;

        void setFlowGeometry(double t_Ain, double t_Aout);

        void setInletTemperature(double inletTemperature);

        void setFlowTemperatures(double t_inTemperature, double t_outTemperature);
        void setFlowSpeed(double t_speed);

        void smoothEnergyGain(double qv1, double qv2);

        // Calculates airflow properties of the gap given inletTemperature temperature. In case
        // inletTemperature temperature is not give, class will use temperature provided in the
        // gap constructor.
        void calculateVentilatedAirflow(double inletTemperature);

        void calculateThermallyDrivenAirflowWithAdjacentGap(CIGUVentilatedGapLayer & adjacentGap);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        void precalculateState() override;
        void calculateOutletTemperatureFromAirFlow();

        VentilatedGapState calculateInletAndOutletTemperaturesWithTheAdjacentGap(
          CIGUVentilatedGapLayer & adjacentGap,
          VentilatedGapState current,
          VentilatedGapState previous,
          double relaxationParameter);

        double calculateThermallyDrivenSpeedOfAdjacentGap(CIGUVentilatedGapLayer & adjacentGap);

        double getDrivingPressure();

        double bernoullyPressureTerm();
        double hagenPressureTerm();
        double pressureLossTerm();
        double betaCoeff();
        void calculateConvectionOrConductionFlow() override;
        double characteristicHeight();
        double calcImpedance(double t_A) const;
        [[nodiscard]] double ventilatedHeatGain();
        double calculateThermallyDrivenSpeed();

        void calculateHeatFlowNextLayer() const;

        std::shared_ptr<CIGUGapLayer> m_Layer;

        VentilatedGapState m_State;
        const Gases::GasProperties m_ReferenceGasProperties;

        double m_Zin{0};
        double m_Zout{0};

        //! Forced ventilation is stored as optional so that it can be decided if airflow
        //! calculations will be either thermally driven or forced. Forced ventilation will be
        //! used automatically if this value has been populated, otherwise, thermally driven is
        //! assumed.
        std::optional<ForcedVentilation> m_ForcedVentilation;
    };

}   // namespace Tarcog::ISO15099
