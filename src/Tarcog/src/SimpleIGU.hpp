#pragma once

#include "IGUConfigurations.hpp"


namespace Tarcog
{
    namespace ISO15099
    {
        class SimpleIGU : public IIGUSystem
        {
        public:
            virtual ~SimpleIGU() = default;
            SimpleIGU(double uValue,
                      double shgc,
                      double h,
                      double thickness = 0,
                      std::vector<double> gapLayerThicknesses = {});

            double getUValue() override;
            double getSHGC(double t_TotSol) override;
            [[nodiscard]] double getH(System system, Environment environment) const override;

            void setWidth(double width) override;
            void setHeight(double height) override;
            void setTilt(double tilt) override;
            void setWidthAndHeight(double width, double height) override;
            void setInteriorAndExteriorSurfacesHeight(double height) override;

            [[nodiscard]] double thickness() const override;
            [[nodiscard]] std::vector<double> gapLayerThicknesses() const override;

        private:
            double m_UValue;
            double m_SHGC;
            double m_H;
            double m_Thickness{0};
            std::vector<double> m_gapLayerThicknesses;
        };
    }   // namespace ISO15099
}   // namespace Tarcog
