#pragma once

#include <memory>
#include "BaseIGULayer.hpp"
#include "WCEGases.hpp"

namespace Tarcog
{
    double const ReferenceTemperature = 273.15;

    namespace ISO15099
    {
        // Properties of the gap at time of production
        struct SealedGapProperties
        {
            SealedGapProperties(double t_Temperature, double t_Pressure) :
                temperature(t_Temperature), pressure(t_Pressure)
            {}

            double temperature;
            double pressure;
        };

        class CIGUGapLayer : public CBaseIGULayer
        {
        public:
            CIGUGapLayer(double t_Thickness, double t_Pressure);
            CIGUGapLayer(double t_Thickness, double t_Pressure, const Gases::CGas & t_Gas);

            void connectToBackSide(const std::shared_ptr<CBaseLayer> & t_Layer) override;

            double getPressure();

            std::shared_ptr<CBaseLayer> clone() const override;

            double getMaxDeflection() const override;
            double getMeanDeflection() const override;

            void setSealedGapProperties(double t_Temperature, double t_Pressure);

        protected:
            void calculateConvectionOrConductionFlow() override;

            double m_Pressure;
            AirflowProperties m_AirflowProperties;

            // Gap by default will not be considered to be sealed. If not sealed then
            // pressure will be considered to be m_Pressure;
            std::optional<SealedGapProperties> m_SealedGapProperties{std::nullopt};

            Gases::CGas m_Gas;

        private:
            double calculateRayleighNumber();
            double aspectRatio() const;
            double convectiveH();

            void checkNextLayer() const;
        };

    }   // namespace ISO15099

}   // namespace Tarcog