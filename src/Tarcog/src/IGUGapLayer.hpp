#pragma once

#include <memory>

#include <WCEGases.hpp>

#include "BaseLayer.hpp"
#include "DeflectionInterface.hpp"
#include "GasSpecification.hpp"

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

        [[nodiscard]] bool isStillAir(double airSpeed);

        class CIGUGapLayer : public CBaseLayer, public Tarcog::Deflectable
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

            std::shared_ptr<CBaseLayer> clone() const override;

            double getMaxDeflection() const override;
            double getMeanDeflection() const override;

            void setSealedGapProperties(double t_Temperature, double t_Pressure);

            // Gas specification is needed for equivalent solid layer conductivity calculations
            // In case gas is not air, but some other gas or gas mixture, then gas specification
            // should be used to calculate the gas thermal conductivity.
            [[nodiscard]] GasSpecification getGasSpecification() const;

        protected:
            void calculateConvectionOrConductionFlow() override;

            // Gap by default will not be considered to be sealed. If not sealed then
            // pressure will be considered to be m_Pressure;
            std::optional<SealedGapProperties> m_SealedGapProperties{std::nullopt};

            GasSpecification gasSpecification;

        private:
            double calculateRayleighNumber();
            double aspectRatio() const;

            void updateGasSpecifications();
            [[nodiscard]] double calculateConvectiveConductiveCoefficient();
            [[nodiscard]] double addAirflowEffect(double convectiveCoefficient);
            double convectiveH();

            double m_AccommodationCoefficient1{
              ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT};
            double m_AccommodationCoefficient2{
              ConstantsData::DEFAULT_SURFACE_ACCOMMODATION_COEFFICIENT};

            bool m_isDCenterMeasured{false};
        };

    }   // namespace ISO15099

}   // namespace Tarcog