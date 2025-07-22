#include "SimpleIGU.hpp"

#include <numeric>

namespace Tarcog::ISO15099
{
    SimpleIGU::SimpleIGU(double uValue,
                         double shgc,
                         double h,
                         double thickness,
                         std::vector<double> gapLayerThicknesses) :
        m_UValue(uValue),
        m_SHGC(shgc),
        m_H(h),
        m_Thickness(thickness),
        m_gapLayerThicknesses(std::move(gapLayerThicknesses))
    {}

    double SimpleIGU::getUValue()
    {
        return m_UValue;
    }

    double SimpleIGU::getSHGC(double)
    {
        return m_SHGC;
    }

    double SimpleIGU::getHc(System, Environment) const
    {
        return 0;
    }

    double SimpleIGU::getHr(System, Environment) const
    {
        return 0;
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

    std::vector<double> SimpleIGU::gapLayerThicknesses() const
    {
        return m_gapLayerThicknesses;
    }

    void SimpleIGU::setTilt(double)
    {}
}   // namespace Tarcog::ISO15099