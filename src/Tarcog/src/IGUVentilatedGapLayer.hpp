#ifndef TARIGUVENTILATEDGAPLAYER_H
#define TARIGUVENTILATEDGAPLAYER_H

#include <memory>

#include "WCEGases.hpp"
#include "IGUGapLayer.hpp"


namespace Tarcog
{
    namespace ISO15099
    {
        struct VentilatedGapState
        {
            VentilatedGapState() = default;
            VentilatedGapState(double inletTemperature, double outletTemperature);
            double inletTemperature{0};
            double outletTemperature{0};
        };

        class CIGUVentilatedGapLayer : public CIGUGapLayer
        {
        public:
            explicit CIGUVentilatedGapLayer(const std::shared_ptr<CIGUGapLayer> & t_Layer);

            double layerTemperature() override;

            void setFlowGeometry(double t_Ain, double t_Aout);

            void setInletTemperature(double inletTemperature);

            void setFlowTemperatures(double t_inTemperature, double t_outTemperature);
            void setFlowSpeed(double t_speed);

            void smoothEnergyGain(double qv1, double qv2);

            // Calculates airflow properties of the gap given inletTemperature temperature. In case inletTemperature
            // temperature is not give, class will use temperature provided in the gap constructor.
            void calculateVentilatedAirflow(std::optional<double> inletTemperature);

            void calculateThermallyDrivenAirflowWithAdjacentGap(CIGUVentilatedGapLayer & adjacentGap);

        private:
            void calculateOutletTemperatureFromAirFlow();

            VentilatedGapState calculateInletAndOutletTemperaturesWithTheAdjecentGap(
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
            void ventilatedFlow();
            double calculateThermallyDrivenSpeed();

            std::shared_ptr<CIGUGapLayer> m_Layer;
            Gases::CGas m_ReferenceGas;

            VentilatedGapState m_State;
            double m_Zin{0};
            double m_Zout{0};
        };

    }   // namespace ISO15099

}   // namespace Tarcog

#endif
