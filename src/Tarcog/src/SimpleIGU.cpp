#include "SimpleIGU.hpp"

namespace Tarcog::ISO15099
{
    SimpleIGU::SimpleIGU(double uValue, double shgc, double h, double thickness) :
        m_UValue(uValue), m_SHGC(shgc), m_H(h), m_Thickness(thickness)
    {}

    double SimpleIGU::getUValue()
    {
        return m_UValue;
    }

    double SimpleIGU::getSHGC(double)
    {
        return m_SHGC;
    }

    double SimpleIGU::getH(System, Environment) const
    {
        return m_H;
    }

    void SimpleIGU::setWidth(double)
    {}

    void SimpleIGU::setHeight(double)
    {}

    void SimpleIGU::setWidthAndHeight(double, double)
    {}

    void SimpleIGU::setInteriorAndExteriorSurfacesHeight(double)
    {}

    double SimpleIGU::thickness() const
    {
        return m_Thickness;
    }

    void SimpleIGU::setTilt(double)
    {}
}   // namespace Tarcog::ISO15099