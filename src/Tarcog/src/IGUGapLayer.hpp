#pragma once

#include <memory>
#include <optional>
#include <functional>

#include <WCEGases.hpp>

#include "BaseLayer.hpp"
#include "GasSpecification.hpp"

namespace Tarcog
{
    double const ReferenceTemperature = 273.15;

    namespace ISO15099
    {
        // Properties of the gap at time of production
        struct ManufacturingConditions
        {
            ManufacturingConditions(double t_Temperature, double t_Pressure) :
                temperature(t_Temperature), pressure(t_Pressure)
            {}

            double temperature;
            double pressure;
        };

        struct VentilatedGapTemperatures
        {
            double inletTemperature{0};
            double outletTemperature{0};
        };

        struct ForcedVentilation
        {
            double speed{0};
            double temperature{0};
        };

        [[nodiscard]] bool isStillAir(double airSpeed);

        class CIGUGapLayer : public CBaseLayer
        {
        public:
            CIGUGapLayer(double t_Thickness, double t_Pressure, bool t_isDCenterMeasured = false);
            CIGUGapLayer(double t_Thickness,
                         double t_Pressure,
                         const Gases::CGas & t_Gas,
                         bool t_isDCenterMeasured = false);
            CIGUGapLayer(double t_Thickness,
                         double t_Pressure,
                         const Gases::CGas & t_Gas,
                         double t_AccommodationCoefficient1,
                         double t_AccommodationCoefficient2,
                         bool t_isDCenterMeasured = false);

            void connectToBackSide(const std::shared_ptr<CBaseLayer> & t_Layer) override;

            double getPressure();

            double getThickness() const override;
            [[nodiscard]] bool isGapLayer() const override { return true; }

            std::shared_ptr<CBaseLayer> clone() const override;

            double getMaxDeflection() const override;
            double getMeanDeflection() const override;

            void setManufacturingConditions(double t_Temperature, double t_Pressure);

            [[nodiscard]] GasSpecification getGasSpecification() const;

            // Pillar activation
            using PillarAreaFn = std::function<double()>;
            using PillarResistanceFn = std::function<double(double kHot,
                                                            double kCold,
                                                            double kPillar,
                                                            double height,
                                                            double area)>;

            void activatePillar(double materialConductivity,
                                double cellArea,
                                PillarAreaFn areaOfContactFn,
                                PillarResistanceFn thermalResistanceFn);
            void activatePillar(double measuredConductance);

            // Ventilation activation
            void activateVentilation() override;
            void activateVentilation(double forcedVentilationInletTemperature,
                                     double forcedVentilationInletSpeed);
            [[nodiscard]] bool isVentilated() const override;

            // Ventilation flow methods (called by shade layer)
            void setFlowGeometry(double t_Ain, double t_Aout);
            void calculateThermallyDrivenAirflowWithAdjacentGap(CIGUGapLayer & adjacentGap);
            void calculateVentilatedAirflow(double inletTemperature);

            double averageLayerTemperature() override;

        private:
            void calculateConvectionOrConductionFlow() override;

            std::optional<ManufacturingConditions> m_ManufacturingConditions{std::nullopt};

            GasSpecification gasSpecification;

            // Base gap calculations
            double calculateRayleighNumber();
            double aspectRatio() const;
            void updateGasSpecifications();
            [[nodiscard]] double calculateConvectiveConductiveCoefficient();
            [[nodiscard]] double addAirflowEffect(double convectiveCoefficient);
            double convectiveH();

            // Ventilation calculations
            void precalculateState() override;
            void setInletTemperature(double inletTemperature);
            void setFlowTemperatures(double t_inTemperature, double t_outTemperature);
            void setFlowSpeed(double t_speed);
            void smoothEnergyGain(double qv1, double qv2);
            void calculateHeatFlowNextLayer() const;
            double characteristicHeight();
            [[nodiscard]] double calcImpedance(double t_A) const;
            [[nodiscard]] double ventilatedHeatGain();
            double calculateThermallyDrivenSpeed();
            void calculateOutletTemperatureFromAirFlow();
            double calculateThermallyDrivenSpeedOfAdjacentGap(CIGUGapLayer & adjacentGap);
            VentilatedGapTemperatures calculateInletAndOutletTemperaturesWithTheAdjacentGap(
              CIGUGapLayer & adjacentGap,
              VentilatedGapTemperatures current,
              VentilatedGapTemperatures previous,
              double relaxationParameter);
            double performIterationStep(double relaxationParameter, double & TgapOut);
            [[nodiscard]] double getDrivingPressure();
            [[nodiscard]] double bernoullyPressureTerm();
            [[nodiscard]] double hagenPressureTerm();
            [[nodiscard]] double pressureLossTerm();
            [[nodiscard]] double betaCoeff();
            [[nodiscard]] bool isConverged(const VentilatedGapTemperatures & current,
                                           const VentilatedGapTemperatures & previous);
            void adjustTemperatures(CIGUGapLayer & adjacentGap);

            // Pillar calculations
            [[nodiscard]] double conductivityOfPillarArray();

            double m_AccommodationCoefficient1{
              ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT};
            double m_AccommodationCoefficient2{
              ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT};

            bool m_isDCenterMeasured{false};

            // Pillar state
            double m_PillarMaterialConductivity{0};
            double m_PillarCellArea{0};
            PillarAreaFn m_PillarAreaOfContactFn;
            PillarResistanceFn m_PillarResistanceFn;
            std::optional<double> m_PillarMeasuredConductance;

            // Ventilation state
            bool m_IsVentilated{false};
            VentilatedGapTemperatures m_VentState{Gases::DefaultTemperature,
                                                  Gases::DefaultTemperature};
            Gases::GasProperties m_ReferenceGasProperties{};
            double m_Zin{0};
            double m_Zout{0};
            std::optional<ForcedVentilation> m_ForcedVentilation;
        };

    }   // namespace ISO15099

}   // namespace Tarcog
