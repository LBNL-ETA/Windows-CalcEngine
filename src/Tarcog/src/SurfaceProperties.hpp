#pragma once

#include <WCECommon.hpp>

namespace Tarcog::ISO15099
{
    class ISurfaceProperties
        {
        public:
            virtual ~ISurfaceProperties() = default;

            [[nodiscard]] virtual double emissivity(double t_Temperature) const = 0;
            [[nodiscard]] virtual double transmittance(double t_Temperature) const = 0;

            [[nodiscard]] virtual std::unique_ptr<ISurfaceProperties> clone() const = 0;
        };

        class ConstantSurfaceProperties : public ISurfaceProperties
        {
        public:
            ConstantSurfaceProperties(double t_Emissivity, double t_Transmittance);

            [[nodiscard]] double emissivity(double t_Temperature) const override;
            [[nodiscard]] double transmittance(double t_Temperature) const override;

            [[nodiscard]] std::unique_ptr<ISurfaceProperties> clone() const override;

        private:
            double m_Emissivity;
            double m_Transmittance;
        };

        class ThermochromicSurfaceProperties : public ISurfaceProperties
        {
        public:
            ThermochromicSurfaceProperties(
              const std::vector<std::pair<double, double>> & t_Emissivity,
              const std::vector<std::pair<double, double>> & t_Transmittance);

            [[nodiscard]] std::unique_ptr<ISurfaceProperties> clone() const override;

            [[nodiscard]] double emissivity(double t_Temperature) const override;
            [[nodiscard]] double transmittance(double t_Temperature) const override;

        private:
            std::shared_ptr<FenestrationCommon::IInterpolation2D> m_EmissivityInterpolator;
            std::shared_ptr<FenestrationCommon::IInterpolation2D> m_TransmittanceInterpolator;
        };

}   // namespace Tarcog::ISO15099