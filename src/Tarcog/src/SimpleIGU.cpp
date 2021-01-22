#include "SimpleIGU.hpp"

namespace Tarcog
{
    namespace ISO15099
    {
        SimpleIGU::SimpleIGU(double uValue, double shgc, double hc) :
          m_UValue(uValue), m_SHGC(shgc), m_Hc(hc)
        {}

        double SimpleIGU::getUValue() const
        {
            return m_UValue;
        }

        double SimpleIGU::getSHGC(double) const
        {
            return m_SHGC;
        }

        double SimpleIGU::getHc(System, Environment) const
        {
            return m_Hc;
        }

    }   // namespace ISO15099
}   // namespace Tarcog
