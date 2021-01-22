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

        private:
            double m_UValue;
            double m_SHGC;
            double m_Hc;
        };
    }
}


