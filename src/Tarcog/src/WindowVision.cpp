#include "WindowVision.hpp"

#include "EnvironmentConfigurations.hpp"

namespace Tarcog
{
    namespace ISO15099
    {
        Vision::Vision(double area, double tvis, double tsol, const IIGUSystem & iguSystem) :
          m_Area(area),
          m_Uvalue(iguSystem.getUValue()),
          m_SHGC(iguSystem.getSHGC(tsol)),
          m_VT(tvis),
          m_HcExterior(iguSystem.getHc(System::SHGC, Environment::Outdoor))
        {}

        Vision::Vision(double area, double Uvalue, double shgc, double vt, double hcExterior) :
          m_Area(area), m_Uvalue(Uvalue), m_SHGC(shgc), m_VT(vt), m_HcExterior(hcExterior)
        {}

        double Vision::uValue() const
        {
            return m_Uvalue;
        }

        double Vision::shgc() const
        {
            return m_SHGC;
        }

        double Vision::area() const
        {
            return m_Area;
        }

        double Vision::vt() const
        {
            return m_VT;
        }

        double Vision::hc() const
        {
            return m_HcExterior;
        }

    }   // namespace ISO15099
}   // namespace Tarcog
