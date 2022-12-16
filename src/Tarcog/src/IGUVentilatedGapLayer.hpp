#ifndef TARIGUVENTILATEDGAPLAYER_H
#define TARIGUVENTILATEDGAPLAYER_H

#include <memory>

#include "WCEGases.hpp"
#include "IGUGapLayer.hpp"


namespace Tarcog
{
    namespace ISO15099
    {
        struct VentilatedTemperature
        {
            double inlet{0};
            double outlet{0};
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

            // Calculates airflow properties of the gap given inlet temperature. In case inlet
            // temperature is not give, class will use temperature provided in the gap constructor.
            void calculateVentilatedAirflow(std::optional<double> inletTemperature);

            void calculateOutletTemperatureFromAirFlow();
            VentilatedTemperature calculateInletAndOutletTemperaturesWithTheAdjecentGap(
              CIGUVentilatedGapLayer & adjacentGap,
              VentilatedTemperature current,
              VentilatedTemperature previous,
              double relaxationParameter);

        private:
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

            double m_inTemperature;
            double m_outTemperature;
            double m_InletTemperature{0};
            double m_Zin;
            double m_Ain{0};
            double m_Zout;
            double m_Aout{0};
            void calculateImpedances(double inletTemperature);
        };

    }   // namespace ISO15099

}   // namespace Tarcog

#endif
