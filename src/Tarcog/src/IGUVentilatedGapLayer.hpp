#ifndef TARIGUVENTILATEDGAPLAYER_H
#define TARIGUVENTILATEDGAPLAYER_H

#include <memory>

#include "WCEGases.hpp"
#include "IGUGapLayer.hpp"


namespace Tarcog
{
    namespace ISO15099
    {
        class CIGUVentilatedGapLayer : public CIGUGapLayer
        {
        public:
            explicit CIGUVentilatedGapLayer(const std::shared_ptr<CIGUGapLayer> & t_Layer);

            double layerTemperature() override;

            void setFlowGeometry(double const t_Atop, double const t_Abot);

            void setFlowDirection(const AirVerticalDirection t_Direction);

            void setFlowTemperatures(double t_topTemp,
                                     double t_botTemp,
                                     const AirVerticalDirection & t_Direction);
            void setFlowTemperatures(double t_inTemperature, double t_outTemperature);
            void setFlowSpeed(double t_speed);

            double getAirflowReferencePoint(double t_GapTemperature);

            double bernoullyPressureTerm();
            double hagenPressureTerm();
            double pressureLossTerm();
            double betaCoeff();

            void smoothEnergyGain(double qv1, double qv2);

        private:
            void calculateConvectionOrConductionFlow() override;
            double characteristicHeight();
            double calcImpedance(double t_A) const;
            void ventilatedFlow();

            std::shared_ptr<CIGUGapLayer> m_Layer;
            Gases::CGas m_ReferenceGas;

            double m_inTemperature;
            double m_outTemperature;
            double m_Zin;
            double m_Ain{0};
            double m_Zout;
            double m_Aout{0};
        };

    }   // namespace ISO15099

}   // namespace Tarcog

#endif
