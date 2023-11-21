#include <memory>
#include <cassert>

#include "BaseIGULayer.hpp"
#include "Surface.hpp"

using FenestrationCommon::Side;


    namespace Tarcog::ISO15099
    {
        CBaseIGULayer::CBaseIGULayer(double const t_Thickness) : m_Thickness(t_Thickness)
        {}

        double CBaseIGULayer::getThickness() const
        {
            return m_Thickness + getSurface(Side::Front)->getMeanDeflection()
                   - getSurface(Side::Back)->getMeanDeflection();
        }

        double CBaseIGULayer::getBaseThickness() const
        {
            return m_Thickness;
        }

        double CBaseIGULayer::getConductivity()
        {
            return getConductionConvectionCoefficient() * m_Thickness;
        }

        double CBaseIGULayer::getEffectiveThermalConductivity()
        {
            return std::abs(getHeatFlow() * m_Thickness
                            / (m_Surface.at(FenestrationCommon::Side::Front)->getTemperature()
                               - m_Surface.at(FenestrationCommon::Side::Back)->getTemperature()));
        }

    } // namespace Tarcog::ISO15099


