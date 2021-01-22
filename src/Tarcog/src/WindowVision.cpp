#include "WindowVision.hpp"

#include "EnvironmentConfigurations.hpp"

namespace Tarcog
{
    namespace ISO15099
    {
        VisionThermal::VisionThermal(double area, double tvis, double tsol, const IIGUSystem & iguSystem) :
          m_Area(area),
          m_Uvalue(iguSystem.getUValue()),
          m_SHGC(iguSystem.getSHGC(tsol)),
          m_VT(tvis),
          m_HcExterior(iguSystem.getHc(System::SHGC, Environment::Outdoor))
        {}

        VisionThermal::VisionThermal(double area, double Uvalue, double shgc, double vt, double hcExterior) :
          m_Area(area), m_Uvalue(Uvalue), m_SHGC(shgc), m_VT(vt), m_HcExterior(hcExterior)
        {}

        double VisionThermal::uValue() const
        {
            return m_Uvalue;
        }

        double VisionThermal::shgc() const
        {
            return m_SHGC;
        }

        double VisionThermal::area() const
        {
            return m_Area;
        }

        double VisionThermal::vt() const
        {
            return m_VT;
        }

        double VisionThermal::hc() const
        {
            return m_HcExterior;
        }

    }   // namespace ISO15099
}   // namespace Tarcog
