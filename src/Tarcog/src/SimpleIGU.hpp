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
            SimpleIGU(double uValue, double shgc, double h, double thickness = 0);

            double getUValue() override;
            double getSHGC(double t_TotSol) override;
            double getH(System system, Environment environment) const override;

            void setWidth(double width) override;
            void setHeight(double height) override;
            void setTilt(double tilt) override;
            void setWidthAndHeight(double width, double height) override;
            void setInteriorAndExteriorSurfacesHeight(double height) override;

            double thickness() const override;

        private:
            double m_UValue;
            double m_SHGC;
            double m_H;
            double m_Thickness{0};
        };
    }   // namespace ISO15099
}   // namespace Tarcog
