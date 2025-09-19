#include <cmath>
#include <WCECommon.hpp>
#include "EffectiveMultipliers.hpp"

#include "PermeabilityFactor.hpp"

namespace EffectiveLayers
{
    bool isClosed(const EffectiveMultipliers & effectiveOpenness)
    {
        using FenestrationCommon::isEqual;
        return isEqual(effectiveOpenness.Mfront, 0.0) && isEqual(effectiveOpenness.Mleft, 0.0)
               && isEqual(effectiveOpenness.Mright, 0.0) && isEqual(effectiveOpenness.Mtop, 0.0)
               && isEqual(effectiveOpenness.Mbot, 0.0)
               && isEqual(effectiveOpenness.PermeabilityFactor, 0.0);
    }

    EffectiveLayerProperties::EffectiveLayerProperties(const double thickness,
                                               const EffectiveMultipliers & openness,
                                               const Coefficients & coeffs) :
        m_Thickness(thickness), m_Openness(openness), m_Coeffs(coeffs)
    {}

    EffectiveLayerProperties
      makeCommonValues(double thickness, double PF, const ShadeOpenness & openness)
    {
        constexpr Coefficients coeff{0.078, 1.2, 1.0, 1.0};
        const double Ah_eff = coeff.C1 * std::pow(PF, coeff.C2);
        const EffectiveMultipliers mult{Ah_eff,
                                        openness.Dl * coeff.C3,
                                        openness.Dr * coeff.C3,
                                        openness.Dtop * coeff.C4,
                                        openness.Dbot * coeff.C4,
                                        PF};
        return {thickness, mult, coeff};
    }

    EffectiveLayerProperties
      makeHorizontalVenetianValues(double thickness,
                                   const FenestrationCommon::Venetian::Geometry & geometry,
                                   const ShadeOpenness & openness)
    {
        constexpr Coefficients coeff{0.016, -0.63, 0.53, 0.043};
        const double PF = ThermalPermeability::Venetian::permeabilityFactor(thickness, geometry);

        const double tilt = FenestrationCommon::radians(geometry.SlatTiltAngle);
        const double Ah_eff =
          coeff.C1 * std::pow(PF * std::pow(std::cos(tilt), coeff.C2), coeff.C3);

        const EffectiveMultipliers mult{Ah_eff, 0.0, 0.0, openness.Dtop, openness.Dbot, PF};
        const double t_eff =
          coeff.C4 * (geometry.SlatWidth * std::cos(tilt) + thickness * std::sin(tilt));
        return {t_eff, mult, coeff};
    }

    EffectiveLayerProperties
      makeVerticalVenetianValues(double thickness,
                                 const FenestrationCommon::Venetian::Geometry & geometry,
                                 const ShadeOpenness & openness)
    {
        constexpr Coefficients coeff{0.041, 0.0, 0.27, 0.012};
        const double PF = ThermalPermeability::Venetian::permeabilityFactor(thickness, geometry);

        const double tilt = FenestrationCommon::radians(geometry.SlatTiltAngle);
        const double Ah_eff =
          coeff.C1 * std::pow(PF * std::pow(std::cos(tilt), coeff.C2), coeff.C3);

        const EffectiveMultipliers mult{Ah_eff, 0.0, 0.0, openness.Dtop, openness.Dbot, PF};
        const double t_eff =
          coeff.C4 * (geometry.SlatWidth * std::cos(tilt) + thickness * std::sin(tilt));
        return {t_eff, mult, coeff};
    }

    EffectiveLayerProperties
      makePerforatedValues(const double thickness,
                           const FenestrationCommon::Perforated::Geometry & geometry,
                           const ShadeOpenness & openness)
    {
        const double PF = ThermalPermeability::Perforated::permeabilityFactor(geometry);
        return makeCommonValues(thickness, PF, openness);
    }

    EffectiveLayerProperties makeWovenValues(const double thickness,
                                         const FenestrationCommon::Woven::Geometry & geometry,
                                         const ShadeOpenness & openness)
    {
        const double PF = ThermalPermeability::Woven::permeabilityFactor(geometry);
        return makeCommonValues(thickness, PF, openness);
    }

    EffectiveLayerProperties
      makeLouveredShutterValues(const FenestrationCommon::LouveredShutter::Geometry & geometry,
                                const ShadeOpenness & openness)
    {
        constexpr Coefficients coeff{0.12, 0.82, 0.059, 0.11};
        const double PF = ThermalPermeability::LouveredShutter::permeabilityFactor(geometry);

        const double ang = FenestrationCommon::radians(geometry.SlatAngle);
        const double openness_factor =
          std::max(0.0, 1.0 - std::pow(2.0 / FenestrationCommon::WCE_PI * ang, 4));
        const double Ah_eff = std::max(
          0.0, coeff.C1 * std::pow(PF * openness_factor, coeff.C2) - coeff.C3 * std::cos(ang));

        (void)openness;
        const EffectiveMultipliers mult{Ah_eff, 0.0, 0.0, 0.0, 0.0, PF};

        const double t_eff =
          coeff.C4
          * (geometry.SlatWidth * std::cos(ang) + geometry.SlatThickness * std::abs(std::sin(ang)));

        return {t_eff, mult, coeff};
    }

    EffectiveLayerProperties makeUserDefinedValues(const double thickness,
                                               const double permeabilityFactor,
                                               const double effectiveFrontThermalOpennessArea,
                                               const ShadeOpenness & openness)
    {
        constexpr Coefficients coeff{0, 0, 0, 0};
        const EffectiveMultipliers mult{effectiveFrontThermalOpennessArea,
                                        openness.Dl,
                                        openness.Dr,
                                        openness.Dtop,
                                        openness.Dbot,
                                        permeabilityFactor};
        return {thickness, mult, coeff};
    }


}   // namespace EffectiveLayers
