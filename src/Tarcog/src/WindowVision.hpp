#pragma once

#include <memory>
#include "IGUConfigurations.hpp"

namespace Tarcog
{
    namespace ISO15099
    {
        class Vision
        {
        public:
            Vision() = delete;
            Vision(double area, double tvis, double tsol, const IIGUSystem & iguSystem);
            Vision(double area, double uvalue, double shgc, double vt, double hcExterior);
            double area() const;
            double uValue() const;
            double shgc() const;
            double vt() const;
            double hc() const;

        private:
            double m_Area{0};
            double m_Uvalue{0};
            double m_SHGC{0};
            double m_VT{1};
            double m_HcExterior{0};
        };
    }   // namespace ISO15099
}   // namespace Tarcog
