#include <cassert>
#include <cmath>

#include "BaseShade.hpp"
#include "Surface.hpp"
#include "IGUGapLayer.hpp"
#include "Environment.hpp"
#include "IGUVentilatedGapLayer.hpp"


namespace Tarcog::ISO15099
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// CShadeOpenings
    ////////////////////////////////////////////////////////////////////////////////////////////////

    CShadeOpenings::CShadeOpenings(double const t_Atop,
                                   double const t_Abot,
                                   double const t_Aleft,
                                   double const t_Aright,
                                   double const t_Afront,
                                   double const t_FrontPorosity) :
        m_Atop(t_Atop),
        m_Abot(t_Abot),
        m_Aleft(t_Aleft),
        m_Aright(t_Aright),
        m_Afront(t_Afront),
        m_FrontPorosity(t_FrontPorosity)
    {
        fixForValidity();
    }

    void CShadeOpenings::fixForValidity()
    {
        if(m_Atop == 0)
        {
            m_Atop = OPENING_TOLERANCE;
        }

        if(m_Abot == 0)
        {
            m_Abot = OPENING_TOLERANCE;
        }
    }

    double CShadeOpenings::openingMultiplier() const
    {
        return (m_Aleft + m_Aright + m_Afront) / (m_Abot + m_Atop);
    }

    double CShadeOpenings::Aeq_bot() const
    {
        return m_Abot + 0.5 * m_Atop * openingMultiplier();
    }

    double CShadeOpenings::Aeq_top() const
    {
        return m_Atop + 0.5 * m_Abot * openingMultiplier();
    }

    double CShadeOpenings::frontPorosity() const
    {
        return m_FrontPorosity;
    }

    bool CShadeOpenings::isOpen() const
    {
        return m_Abot > 0 || m_Atop > 0 || m_Aleft > 0 || m_Aright > 0 || m_Afront > 0;
    }

    void CShadeOpenings::checkAndSetDominantWidth(const double gapWidth)
    {
        m_Abot = std::min(gapWidth, m_Abot);
        m_Atop = std::min(gapWidth, m_Atop);
        m_Aleft = std::min(gapWidth, m_Aleft);
        m_Aright = std::min(gapWidth, m_Aright);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// CIGUShadeLayer
    ////////////////////////////////////////////////////////////////////////////////////////////////

    CIGUShadeLayer::CIGUShadeLayer(
      const double t_Thickness,
      const double t_Conductivity,
      CShadeOpenings && t_ShadeOpenings,
      const std::shared_ptr<Tarcog::ISO15099::Surface> & t_FrontSurface,
      const std::shared_ptr<Tarcog::ISO15099::Surface> & t_BackSurface) :
        CIGUSolidLayer(t_Thickness, t_Conductivity, t_FrontSurface, t_BackSurface),
        m_ShadeOpenings(t_ShadeOpenings),
        m_MaterialConductivity(t_Conductivity)
    {}

    CIGUShadeLayer::CIGUShadeLayer(const SolidLayer & t_Layer,
                                   const CShadeOpenings & t_ShadeOpenings) :
        CIGUSolidLayer(*t_Layer),
        m_ShadeOpenings(t_ShadeOpenings),
        m_MaterialConductivity(t_Layer->getConductance())
    {}

    CIGUShadeLayer::CIGUShadeLayer(double t_Thickness, double t_Conductivity) :
        CIGUSolidLayer(t_Thickness, t_Conductivity, nullptr, nullptr),
        m_MaterialConductivity(t_Conductivity)
    {}

    std::shared_ptr<CBaseLayer> CIGUShadeLayer::clone() const
    {
        return std::make_shared<CIGUShadeLayer>(*this);
    }

    bool CIGUShadeLayer::isPermeable() const
    {
        return m_ShadeOpenings.isOpen();
    }

    void CIGUShadeLayer::setDominantAirflowWidth()
    {
        if(getPreviousLayer() != nullptr)
        {
            m_ShadeOpenings.checkAndSetDominantWidth(getPreviousLayer()->getThickness());
        }
        if(getNextLayer() != nullptr)
        {
            m_ShadeOpenings.checkAndSetDominantWidth(getNextLayer()->getThickness());
        }
    }

    void CIGUShadeLayer::calculateConvectionOrConductionFlow()
    {
        setDominantAirflowWidth();
        m_Conductivity =
          equivalentConductivity(m_MaterialConductivity, m_ShadeOpenings.frontPorosity());
        CIGUSolidLayer::calculateConvectionOrConductionFlow();
        assert(getNextLayer() != nullptr);
        assert(getPreviousLayer() != nullptr);

        // This must be set here or gap will be constantly calling this routine back throughout
        // nextLayer property.
        setCalculated();

        if(std::dynamic_pointer_cast<CIGUGapLayer>(getPreviousLayer()) != nullptr
           && std::dynamic_pointer_cast<CIGUGapLayer>(getNextLayer()) != nullptr)
        {
            auto previousGapLayer =
              std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(getPreviousLayer());
            auto nextGapLayer = std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(getNextLayer());
            calcInBetweenShadeFlow(*previousGapLayer, *nextGapLayer);
        }
        else if(std::dynamic_pointer_cast<CEnvironment>(getPreviousLayer()) != nullptr
                && std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(getNextLayer()) != nullptr)
        {
            calcEdgeShadeFlow(*std::dynamic_pointer_cast<CEnvironment>(getPreviousLayer()),
                              *std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(getNextLayer()));
        }
        else if(std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(getPreviousLayer()) != nullptr
                && std::dynamic_pointer_cast<CEnvironment>(getNextLayer()) != nullptr)
        {
            calcEdgeShadeFlow(
              *std::dynamic_pointer_cast<CEnvironment>(getNextLayer()),
              *std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(getPreviousLayer()));
        }
    }

    void CIGUShadeLayer::calcInBetweenShadeFlow(CIGUVentilatedGapLayer & t_Gap1,
                                                CIGUVentilatedGapLayer & t_Gap2)
    {
        t_Gap1.setFlowGeometry(m_ShadeOpenings.Aeq_bot(), m_ShadeOpenings.Aeq_top());
        t_Gap2.setFlowGeometry(m_ShadeOpenings.Aeq_top(), m_ShadeOpenings.Aeq_bot());

        t_Gap1.calculateThermallyDrivenAirflowWithAdjacentGap(t_Gap2);
    }

    void CIGUShadeLayer::calcEdgeShadeFlow(CEnvironment & t_Environment,
                                           CIGUVentilatedGapLayer & t_Gap)
    {
        t_Gap.setFlowGeometry(m_ShadeOpenings.Aeq_bot(), m_ShadeOpenings.Aeq_top());

        t_Gap.calculateVentilatedAirflow(t_Environment.getGasTemperature());
    }

    namespace Helper
    {
        std::optional<double> thermalConductivity(const double temperature,
                                                  const CIGUGapLayer * layer)
        {
            if(layer != nullptr)
            {
                auto gasSpec{layer->getGasSpecification()};
                gasSpec.setTemperature(temperature);
                return gasSpec.gas.getGasProperties().m_ThermalConductivity;
            }
            return std::nullopt;
        }

        double effectiveGasConductivity(std::optional<double> gas1Cond,
                                        std::optional<double> gas2Cond,
                                        double defaultConductivity = 0.0)
        {
            auto average = [](double a, double b) { return (a + b) / 2.0; };

            return gas1Cond && gas2Cond ? average(*gas1Cond, *gas2Cond)
                                        : gas1Cond.value_or(gas2Cond.value_or(defaultConductivity));
        }
    }   // namespace Helper

    double CIGUShadeLayer::equivalentConductivity(const double t_Conductivity,
                                                  const double permeabilityFactor)
    {
        auto previousLayer = std::dynamic_pointer_cast<CIGUGapLayer>(getPreviousLayer());
        auto nextLayer = std::dynamic_pointer_cast<CIGUGapLayer>(getNextLayer());
        auto gas1Cond =
          Helper::thermalConductivity(averageSurfaceTemperature(), previousLayer.get());
        auto gas2Cond = Helper::thermalConductivity(averageSurfaceTemperature(), nextLayer.get());

        return Helper::effectiveGasConductivity(gas1Cond, gas2Cond) * permeabilityFactor
               + (1 - permeabilityFactor) * t_Conductivity;
    }
}   // namespace Tarcog::ISO15099
