#pragma once

#include <memory>

#include <WCEGases.hpp>

#include "IGUGapLayer.hpp"
#include "LayerTypes.hpp"


namespace Tarcog::ISO15099
{
    struct VentilatedGapTemperatures
    {
        VentilatedGapTemperatures() = default;
        VentilatedGapTemperatures(double inletTemperature, double outletTemperature);
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
        explicit CIGUVentilatedGapLayer(const GapLayer & t_Layer);
        CIGUVentilatedGapLayer(const GapLayer & t_Layer,
                               double forcedVentilationInletTemperature,
                               double forcedVentilationInletSpeed);

        double averageLayerTemperature() override;

        void setFlowGeometry(double t_Ain, double t_Aout);

        void calculateThermallyDrivenAirflowWithAdjacentGap(CIGUVentilatedGapLayer & adjacentGap);

        // Calculates airflow properties of the gap given inletTemperature temperature. In case
        // inletTemperature temperature is not given, the function will use temperature provided in the
        // gap constructor.
        void calculateVentilatedAirflow(double inletTemperature);

        std::shared_ptr<CBaseLayer> clone() const override;

    private:
        void setInletTemperature(double inletTemperature);

        void setFlowTemperatures(double t_inTemperature, double t_outTemperature);
        void setFlowSpeed(double t_speed);

        void smoothEnergyGain(double qv1, double qv2);

        void precalculateState() override;
        void calculateOutletTemperatureFromAirFlow();

        VentilatedGapTemperatures calculateInletAndOutletTemperaturesWithTheAdjacentGap(
          CIGUVentilatedGapLayer & adjacentGap,
          VentilatedGapTemperatures current,
          VentilatedGapTemperatures previous,
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

        double performIterationStep(double relaxationParameter, double & TgapOut);

        GapLayer m_Layer;

        VentilatedGapTemperatures m_State;
        const Gases::GasProperties m_ReferenceGasProperties;

        double m_Zin{0};
        double m_Zout{0};

        //! Forced ventilation is stored as optional so that it can be decided if airflow
        //! calculations will be either thermally driven or forced. Forced ventilation will be
        //! used automatically if this value has been populated, otherwise, thermally driven is
        //! assumed.
        std::optional<ForcedVentilation> m_ForcedVentilation;
        bool isConverged(const VentilatedGapTemperatures & current,
                         const VentilatedGapTemperatures & previous);
        void adjustTemperatures(CIGUVentilatedGapLayer & adjacentGap);
        void performIterationStep(CIGUVentilatedGapLayer & adjacentGap,
                                  VentilatedGapTemperatures & current,
                                  double RelaxationParameter);
    };

}   // namespace Tarcog::ISO15099
