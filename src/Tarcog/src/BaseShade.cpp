#include <cassert>
#include <cmath>

#include "BaseShade.hpp"
#include "Surface.hpp"
#include "IGUGapLayer.hpp"
#include "Environment.hpp"
#include "IGUVentilatedGapLayer.hpp"


namespace Tarcog::ISO15099 {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// CShadeOpenings
    ////////////////////////////////////////////////////////////////////////////////////////////////

    CShadeOpenings::CShadeOpenings(double const t_Atop,
                                   double const t_Abot,
                                   double const t_Aleft,
                                   double const t_Aright,
                                   double const t_Afront,
                                   double const t_FrontPorosity) : m_Atop(t_Atop),
                                                                   m_Abot(t_Abot),
                                                                   m_Aleft(t_Aleft),
                                                                   m_Aright(t_Aright),
                                                                   m_Afront(t_Afront),
                                                                   m_FrontPorosity(t_FrontPorosity) {
    }

    double CShadeOpenings::openingMultiplier() const {
        const double denom = m_Abot + m_Atop;
        // It is extremely important to use 2 * OPENING_TOLERANCE because the airflow equation
        // becomes very sensitive around zero and will produce different results. The whole point is
        // that Abot + Atop are 1e-6 each and should add up to 2e-6 (or whatever the tolerance is).
        const double safeDenom = (denom == 0.0) ? 2 * OPENING_TOLERANCE : denom;

        return (m_Aleft + m_Aright + m_Afront) / safeDenom;
    }

    double CShadeOpenings::Aeq_bot() const {
        const double bot = std::max(OPENING_TOLERANCE, m_Abot);
        const double top = std::max(OPENING_TOLERANCE, m_Atop);
        return bot + 0.5 * top * openingMultiplier();
    }

    double CShadeOpenings::Aeq_top() const {
        const double bot = std::max(OPENING_TOLERANCE, m_Abot);
        const double top = std::max(OPENING_TOLERANCE, m_Atop);
        return top + 0.5 * bot * openingMultiplier();
    }

    double CShadeOpenings::frontPorosity() const {
        return m_FrontPorosity;
    }

    bool CShadeOpenings::isOpen() const {
        return m_Abot > 0 || m_Atop > 0 || m_Aleft > 0 || m_Aright > 0 || m_Afront > 0;
    }

    void CShadeOpenings::checkAndSetDominantWidth(const double gapWidth) {
        m_Abot = std::min(gapWidth, m_Abot);
        m_Atop = std::min(gapWidth, m_Atop);
        m_Aleft = std::min(gapWidth, m_Aleft);
        m_Aright = std::min(gapWidth, m_Aright);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// CIGUShadeLayer
    ////////////////////////////////////////////////////////////////////////////////////////////////

    CShadeOpenings getShadeOpenings(double width, double height,
                                    const EffectiveLayers::EffectiveMultipliers &effectiveMultipliers) {
        return {
            effectiveMultipliers.Mtop * width,
            effectiveMultipliers.Mbot * width,
            effectiveMultipliers.Mleft * height,
            effectiveMultipliers.Mright * height,
            effectiveMultipliers.Mfront * width * height,
            effectiveMultipliers.PermeabilityFactor
        };
    }

    CIGUShadeLayer::CIGUShadeLayer(
        const double t_Thickness,
        const double t_Conductivity,
        const std::shared_ptr<Surface> &t_FrontSurface,
        const std::shared_ptr<Surface> &t_BackSurface) : CIGUSolidLayer(t_Thickness, t_Conductivity,
                                                                        t_FrontSurface, t_BackSurface),
                                                         m_MaterialConductivity(t_Conductivity) {
    }

    CIGUShadeLayer::CIGUShadeLayer(const SolidLayer &t_Layer) : CIGUSolidLayer(*t_Layer),
                                                                m_MaterialConductivity(
                                                                    t_Layer->getConductance()) {
    }

    CIGUShadeLayer::CIGUShadeLayer(double t_Thickness, double t_Conductivity) : CIGUSolidLayer(t_Thickness,
            t_Conductivity, nullptr, nullptr),
        m_MaterialConductivity(t_Conductivity) {
    }

    std::shared_ptr<CBaseLayer> CIGUShadeLayer::clone() const {
        return std::make_shared<CIGUShadeLayer>(*this);
    }

    bool CIGUShadeLayer::isPermeable() const {
        return getEffectiveOpenings().isOpen();
    }

    void CIGUShadeLayer::assignEffectiveMultipliers(const EffectiveLayers::EffectiveMultipliers &effectiveMultipliers) {
        m_Multipliers = effectiveMultipliers;
    }

    CShadeOpenings CIGUShadeLayer::getEffectiveOpenings() const {
        if (m_Multipliers) {
            CShadeOpenings result{getShadeOpenings(m_Width, m_Height, m_Multipliers.value())};
            if (getPreviousLayer() != nullptr) {
                result.checkAndSetDominantWidth(getPreviousLayer()->getThickness());
            }
            if (getNextLayer() != nullptr) {
                result.checkAndSetDominantWidth(getNextLayer()->getThickness());
            }

            return result;
        }
        return {};
    }

    void CIGUShadeLayer::calculateConvectionOrConductionFlow() {
        m_Conductivity =
                equivalentConductivity(m_MaterialConductivity, getEffectiveOpenings().frontPorosity());
        CIGUSolidLayer::calculateConvectionOrConductionFlow();
        assert(getNextLayer() != nullptr);
        assert(getPreviousLayer() != nullptr);

        // This must be set here or gap will be constantly calling this routine back throughout
        // nextLayer property.
        setCalculated();

        if (std::dynamic_pointer_cast<CIGUGapLayer>(getPreviousLayer()) != nullptr
            && std::dynamic_pointer_cast<CIGUGapLayer>(getNextLayer()) != nullptr) {
            auto previousGapLayer =
                    std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(getPreviousLayer());
            auto nextGapLayer = std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(getNextLayer());
            calcInBetweenShadeFlow(*previousGapLayer, *nextGapLayer);
        } else if (std::dynamic_pointer_cast<CEnvironment>(getPreviousLayer()) != nullptr
                   && std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(getNextLayer()) != nullptr) {
            calcEdgeShadeFlow(*std::dynamic_pointer_cast<CEnvironment>(getPreviousLayer()),
                              *std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(getNextLayer()));
        } else if (std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(getPreviousLayer()) != nullptr
                   && std::dynamic_pointer_cast<CEnvironment>(getNextLayer()) != nullptr) {
            calcEdgeShadeFlow(
                *std::dynamic_pointer_cast<CEnvironment>(getNextLayer()),
                *std::dynamic_pointer_cast<CIGUVentilatedGapLayer>(getPreviousLayer()));
        }
    }

    void CIGUShadeLayer::calcInBetweenShadeFlow(CIGUVentilatedGapLayer &t_Gap1,
                                                CIGUVentilatedGapLayer &t_Gap2) {
        const auto effectiveOpenings = getEffectiveOpenings();
        t_Gap1.setFlowGeometry(effectiveOpenings.Aeq_bot(), effectiveOpenings.Aeq_top());
        t_Gap2.setFlowGeometry(effectiveOpenings.Aeq_top(), effectiveOpenings.Aeq_bot());

        t_Gap1.calculateThermallyDrivenAirflowWithAdjacentGap(t_Gap2);
    }

    void CIGUShadeLayer::calcEdgeShadeFlow(CEnvironment &t_Environment,
                                           CIGUVentilatedGapLayer &t_Gap) {
        const auto effectiveOpenings = getEffectiveOpenings();
        t_Gap.setFlowGeometry(effectiveOpenings.Aeq_bot(), effectiveOpenings.Aeq_top());

        t_Gap.calculateVentilatedAirflow(t_Environment.getAirTemperature());
    }

    namespace Helper {
        std::optional<double> thermalConductivity(const double temperature,
                                                  const CIGUGapLayer *layer) {
            if (layer != nullptr) {
                auto gasSpec{layer->getGasSpecification()};
                gasSpec.setTemperature(temperature);
                return gasSpec.gas.getGasProperties().m_ThermalConductivity;
            }
            return std::nullopt;
        }

        double effectiveGasConductivity(std::optional<double> gas1Cond,
                                        std::optional<double> gas2Cond,
                                        double defaultConductivity = 0.0) {
            auto average = [](double a, double b) { return (a + b) / 2.0; };

            return gas1Cond && gas2Cond
                       ? average(*gas1Cond, *gas2Cond)
                       : gas1Cond.value_or(gas2Cond.value_or(defaultConductivity));
        }
    } // namespace Helper

    double CIGUShadeLayer::equivalentConductivity(const double t_Conductivity,
                                                  const double permeabilityFactor) {
        auto previousLayer = std::dynamic_pointer_cast<CIGUGapLayer>(getPreviousLayer());
        auto nextLayer = std::dynamic_pointer_cast<CIGUGapLayer>(getNextLayer());
        auto gas1Cond =
                Helper::thermalConductivity(averageSurfaceTemperature(), previousLayer.get());
        auto gas2Cond = Helper::thermalConductivity(averageSurfaceTemperature(), nextLayer.get());

        return Helper::effectiveGasConductivity(gas1Cond, gas2Cond) * permeabilityFactor
               + (1 - permeabilityFactor) * t_Conductivity;
    }
} // namespace Tarcog::ISO15099
