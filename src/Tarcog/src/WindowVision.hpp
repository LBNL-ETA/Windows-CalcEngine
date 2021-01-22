#pragma once

#include <memory>
#include "IGUConfigurations.hpp"

namespace Tarcog
{
    namespace ISO15099
    {
        class VisionThermal
        {
        public:
            VisionThermal() = delete;
            VisionThermal(double area, double tvis, double tsol, const IIGUSystem & iguSystem);
            VisionThermal(double area, double uvalue, double shgc, double vt, double hcExterior);
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

        class VisionGeometry
        {
        private:
            static const double m_VisionLength;

            //! Number of exterior frames is important for edge area calculations
            size_t m_NumOfExteriorFrames{4u};
            size_t m_NumOfVerticalDividers{0u};
            size_t m_NumOfHorizontalDividers{0u};
        };
    }   // namespace ISO15099
}   // namespace Tarcog
