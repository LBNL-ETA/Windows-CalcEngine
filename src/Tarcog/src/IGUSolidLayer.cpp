#include <stdexcept>
#include <cassert>

#include "IGUSolidLayer.hpp"
#include "BaseLayer.hpp"
#include "Surface.hpp"
#include "TarcogConstants.hpp"
#include "LayerInterfaces.hpp"
#include "IGUGapLayer.hpp"
#include "Environment.hpp"
#include "IGUGapLayer.hpp"


using FenestrationCommon::Side;


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
    {}

    double CShadeOpenings::openingMultiplier() const
    {
        const double denom = m_Abot + m_Atop;
        const double safeDenom = (denom == 0.0) ? 2 * OPENING_TOLERANCE : denom;
        return (m_Aleft + m_Aright + m_Afront) / safeDenom;
    }

    double CShadeOpenings::Aeq_bot() const
    {
        const double bot = std::max(OPENING_TOLERANCE, m_Abot);
        const double top = std::max(OPENING_TOLERANCE, m_Atop);
        return bot + 0.5 * top * openingMultiplier();
    }

    double CShadeOpenings::Aeq_top() const
    {
        const double bot = std::max(OPENING_TOLERANCE, m_Abot);
        const double top = std::max(OPENING_TOLERANCE, m_Atop);
        return top + 0.5 * bot * openingMultiplier();
    }

    double CShadeOpenings::frontPorosity() const
    {
        return m_FrontPorosity;
    }

    bool CShadeOpenings::isOpen() const
    {
        return m_Abot > 0 || m_Atop > 0 || m_Aleft > 0 || m_Aright > 0 || m_Afront > 0;
    }

    double CShadeOpenings::effectiveFrontThermalOpennessArea() const
    {
        return m_Afront;
    }

    void CShadeOpenings::checkAndSetDominantWidth(const double gapWidth)
    {
        m_Abot = std::min(gapWidth, m_Abot);
        m_Atop = std::min(gapWidth, m_Atop);
        m_Aleft = std::min(gapWidth, m_Aleft);
        m_Aright = std::min(gapWidth, m_Aright);
    }

    CShadeOpenings getShadeOpenings(double width,
                                    double height,
                                    const EffectiveLayers::EffectiveMultipliers & effectiveMultipliers)
    {
        return {effectiveMultipliers.Mtop * width,
                effectiveMultipliers.Mbot * width,
                effectiveMultipliers.Mleft * height,
                effectiveMultipliers.Mright * height,
                effectiveMultipliers.Mfront * width * height,
                effectiveMultipliers.PermeabilityFactor};
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// CIGUSolidLayer
    ////////////////////////////////////////////////////////////////////////////////////////////////

    CIGUSolidLayer::CIGUSolidLayer(
      double const t_Thickness,
      double const t_Conductivity,
      std::shared_ptr<Tarcog::ISO15099::Surface> const & t_FrontSurface,
      std::shared_ptr<Tarcog::ISO15099::Surface> const & t_BackSurface) :
        CBaseLayer(t_Thickness), m_Conductivity(t_Conductivity), m_SolarAbsorptance(0)
    {
        m_Surface[Side::Front] = t_FrontSurface;
        m_Surface[Side::Back] = t_BackSurface;

        if(t_Conductivity < 0)
        {
            throw std::runtime_error("Conductivity must be positive");
        }
    }

    CIGUSolidLayer::CIGUSolidLayer(double const t_Thickness,
                                   double const t_Conductivity,
                                   double const t_FrontEmissivity,
                                   double const t_FrontIRTransmittance,
                                   double const t_BackEmissivity,
                                   double const t_BackIRTransmittance) :
        CBaseLayer(t_Thickness), m_Conductivity(t_Conductivity), m_SolarAbsorptance(0)
    {
        m_Surface[Side::Front] =
          std::make_shared<Surface>(t_FrontEmissivity, t_FrontIRTransmittance);
        m_Surface[Side::Back] = std::make_shared<Surface>(t_BackEmissivity, t_BackIRTransmittance);
    }

    void CIGUSolidLayer::connectToBackSide(std::shared_ptr<CBaseLayer> const & t_Layer)
    {
        CBaseLayer::connectToBackSide(t_Layer);
        t_Layer->setSurface(m_Surface.at(Side::Back), Side::Front);
    }

    double CIGUSolidLayer::getConductance() const
    {
        return m_Conductivity;
    }

    double CIGUSolidLayer::getSolarAbsorptance() const
    {
        return m_SolarAbsorptance;
    }

    void CIGUSolidLayer::setSolarAbsorptance(double const t_SolarAbsorptance)
    {
        m_SolarAbsorptance = t_SolarAbsorptance;
        resetCalculated();
    }

    void CIGUSolidLayer::calculateConvectionOrConductionFlow()
    {
        if(m_EffectiveMultipliers.has_value())
        {
            calculateShadeConvectionOrConductionFlow();
        }
        else
        {
            calculateSolidConvectionOrConductionFlow();
        }
    }

    void CIGUSolidLayer::calculateSolidConvectionOrConductionFlow()
    {
        if(m_Thickness == 0)
        {
            throw std::runtime_error("Solid layer thickness is set to zero.");
        }

        m_ConductiveConvectiveCoeff = m_Conductivity / m_Thickness;
    }

    void CIGUSolidLayer::calculateShadeConvectionOrConductionFlow()
    {
        m_Conductivity =
          equivalentConductivity(m_MaterialConductivity, getEffectiveOpenings().frontPorosity());
        calculateSolidConvectionOrConductionFlow();
        assert(getNextLayer() != nullptr);
        assert(getPreviousLayer() != nullptr);

        // This must be set here or gap will be constantly calling this routine back throughout
        // nextLayer property.
        setCalculated();

        const auto & prev = getPreviousLayer();
        const auto & next = getNextLayer();

        if(prev->isGapLayer() && next->isGapLayer())
        {
            calcInBetweenShadeFlow(
              static_cast<CIGUGapLayer &>(*prev),
              static_cast<CIGUGapLayer &>(*next));
        }
        else if(prev->isEnvironment() && next->isGapLayer())
        {
            calcEdgeShadeFlow(
              static_cast<CEnvironment &>(*prev),
              static_cast<CIGUGapLayer &>(*next));
        }
        else if(prev->isGapLayer() && next->isEnvironment())
        {
            calcEdgeShadeFlow(
              static_cast<CEnvironment &>(*next),
              static_cast<CIGUGapLayer &>(*prev));
        }
    }

    CShadeOpenings CIGUSolidLayer::getEffectiveOpenings() const
    {
        if(m_EffectiveMultipliers.has_value())
        {
            CShadeOpenings result{
              getShadeOpenings(m_Width, m_Height, m_EffectiveMultipliers.value())};
            if(getPreviousLayer() != nullptr)
            {
                result.checkAndSetDominantWidth(getPreviousLayer()->getThickness());
            }
            if(getNextLayer() != nullptr)
            {
                result.checkAndSetDominantWidth(getNextLayer()->getThickness());
            }
            return result;
        }
        return {};
    }

    void CIGUSolidLayer::calcInBetweenShadeFlow(CIGUGapLayer & gap1,
                                                 CIGUGapLayer & gap2)
    {
        const auto effectiveOpenings = getEffectiveOpenings();
        gap1.setFlowGeometry(effectiveOpenings.Aeq_bot(), effectiveOpenings.Aeq_top());
        gap2.setFlowGeometry(effectiveOpenings.Aeq_top(), effectiveOpenings.Aeq_bot());
        gap1.calculateThermallyDrivenAirflowWithAdjacentGap(gap2);
    }

    void CIGUSolidLayer::calcEdgeShadeFlow(CEnvironment & environment,
                                            CIGUGapLayer & gap)
    {
        const auto effectiveOpenings = getEffectiveOpenings();
        gap.setFlowGeometry(effectiveOpenings.Aeq_bot(), effectiveOpenings.Aeq_top());
        gap.calculateVentilatedAirflow(environment.getAirTemperature());
    }

    namespace
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
            auto average = [](double aaa, double bbb) { return (aaa + bbb) / 2.0; };

            return gas1Cond && gas2Cond
                     ? average(*gas1Cond, *gas2Cond)
                     : gas1Cond.value_or(gas2Cond.value_or(defaultConductivity));
        }
    }   // anonymous namespace

    double CIGUSolidLayer::equivalentConductivity(const double conductivity,
                                                  const double permeabilityFactor)
    {
        const auto & prev = getPreviousLayer();
        const auto & next = getNextLayer();
        auto * prevGap = prev && prev->isGapLayer() ? static_cast<CIGUGapLayer *>(prev.get()) : nullptr;
        auto * nxtGap = next && next->isGapLayer() ? static_cast<CIGUGapLayer *>(next.get()) : nullptr;
        auto gas1Cond = thermalConductivity(averageSurfaceTemperature(), prevGap);
        auto gas2Cond = thermalConductivity(averageSurfaceTemperature(), nxtGap);

        return effectiveGasConductivity(gas1Cond, gas2Cond) * permeabilityFactor
               + (1 - permeabilityFactor) * conductivity;
    }

    bool CIGUSolidLayer::isPorous() const
    {
        return getEffectiveOpenings().isOpen();
    }

    bool CIGUSolidLayer::isShadeLayer() const
    {
        return m_EffectiveMultipliers.has_value();
    }

    void CIGUSolidLayer::assignEffectiveMultipliers(
      const EffectiveLayers::EffectiveMultipliers & effectiveMultipliers)
    {
        m_EffectiveMultipliers = effectiveMultipliers;
        m_MaterialConductivity = m_Conductivity;
    }

    void CIGUSolidLayer::setLayerState(double const t_Tf,
                                       double const t_Tb,
                                       double const t_Jf,
                                       double const t_Jb)
    {
        setSurfaceState(t_Tf, t_Jf, Side::Front);
        setSurfaceState(t_Tb, t_Jb, Side::Back);
        resetCalculated();
        if(getNextLayer() != nullptr)
        {
            getNextLayer()->resetCalculated();
        }
        if(getPreviousLayer() != nullptr)
        {
            getPreviousLayer()->resetCalculated();
        }
    }

    void CIGUSolidLayer::setSolarRadiation(double const t_SolarRadiation)
    {
        m_LayerGainFlow = t_SolarRadiation * m_SolarAbsorptance;
        resetCalculated();
    }

    void CIGUSolidLayer::setSolarHeatGain(double const t_SolarAbsorptance,
                                          const double t_SolarRadiation)
    {
        m_SolarAbsorptance = t_SolarAbsorptance;
        m_LayerGainFlow = t_SolarRadiation * m_SolarAbsorptance;
        resetCalculated();
    }

    std::shared_ptr<CBaseLayer> CIGUSolidLayer::clone() const
    {
        return std::make_shared<CIGUSolidLayer>(*this);
    }

    void CIGUSolidLayer::applyDeflection(double meanDeflection, double maxDeflection)
    {
        for(auto aSide : FenestrationCommon::allSides())
        {
            m_Surface[aSide]->applyDeflection(meanDeflection, maxDeflection);
        }
    }

    void CIGUSolidLayer::setConductivity(double t_Conductivity)
    {
        m_Conductivity = t_Conductivity;
        resetCalculated();
    }

    double CIGUSolidLayer::youngsModulus() const
    {
        if(m_DeflectionMaterial.has_value())
        {
            return m_DeflectionMaterial->youngsModulus;
        }
        return DeflectionConstants::YOUNGSMODULUS;
    }

    double CIGUSolidLayer::getMaxDeflection() const
    {
        assert(FenestrationCommon::isEqual(surfaceDeflectionMax(Side::Front),
                                           surfaceDeflectionMax(Side::Back)));
        return surfaceDeflectionMax(Side::Front);
    }

    double CIGUSolidLayer::getMeanDeflection() const
    {
        assert(FenestrationCommon::isEqual(surfaceDeflectionMean(Side::Front),
                                           surfaceDeflectionMean(Side::Back)));
        return surfaceDeflectionMean(Side::Front);
    }

    double CIGUSolidLayer::density() const
    {
        if(m_DeflectionMaterial.has_value())
        {
            return m_DeflectionMaterial->density;
        }
        return MaterialConstants::GLASSDENSITY;
    }

    double CIGUSolidLayer::flexuralRigidity() const
    {
        const auto poissonRatio = m_DeflectionMaterial.has_value()
                                    ? m_DeflectionMaterial->poissonRatio
                                    : DeflectionConstants::POISONRATIO;
        return youngsModulus() * pow(m_Thickness, 3)
               / (12 * (1 - pow(poissonRatio, 2)));
    }

    bool CIGUSolidLayer::hasMeasuredDeflection() const
    {
        return m_HasMeasuredDeflection;
    }

    void CIGUSolidLayer::setMeasuredDeflection()
    {
        m_HasMeasuredDeflection = true;
    }

    void CIGUSolidLayer::setDeflectionMaterial(const DeflectionMaterial & mat)
    {
        m_DeflectionMaterial = mat;
    }

    double CIGUSolidLayer::getRadiationFlow()
    {
        return transmittance(Side::Front)
               * (getNextLayer()->J(FenestrationCommon::Side::Back)
                  - getPreviousLayer()->J(FenestrationCommon::Side::Front));
    }
}   // namespace Tarcog::ISO15099
