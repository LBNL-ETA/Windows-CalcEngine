#pragma once

#include "IGUConfigurations.hpp"


namespace Tarcog {
    namespace ISO15099
    {
        class SimpleIGU : public IIGUSystem
        {
        public:
            SimpleIGU(double uValue, double shgc, double hc);

            double getUValue() const override;
            double getSHGC(double t_TotSol) const override;
            double getHc(System system, Environment environment) const override;

            void setWidth(double width) override;
            void setHeight(double height) override;
            void setWidthAndHeight(double width, double height) override;
            void setExteriorSurfaceHeight(double height) override;

        private:
            double m_UValue;
            double m_SHGC;
            double m_Hc;
        };
    }
}


