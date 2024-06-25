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
            CIGUGapLayer(double t_Thickness, double t_Pressure);
            CIGUGapLayer(double t_Thickness, double t_Pressure, const Gases::CGas & t_Gas);
            CIGUGapLayer(double t_Thickness,
                         double t_Pressure,
                         const Gases::CGas & t_Gas,
                         double t_AccommodationCoefficient1,
                         double t_AccommodationCoefficient2);

            void connectToBackSide(const std::shared_ptr<CBaseLayer> & t_Layer) override;

            double getPressure();

            std::shared_ptr<CBaseLayer> clone() const override;

            double getMaxDeflection() const override;
            double getMeanDeflection() const override;

            void setSealedGapProperties(double t_Temperature, double t_Pressure);

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
        };

    }   // namespace ISO15099

}   // namespace Tarcog