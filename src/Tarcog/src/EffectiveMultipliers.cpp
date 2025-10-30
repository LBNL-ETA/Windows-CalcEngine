#include <cmath>
#include <algorithm>
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

    EffectiveLayerProperties makeCommonValues(const double thickness,
                                              const double permeabilityFactor,
                                              const ShadeOpenness & openness) noexcept
    {
        constexpr Coefficients coeff{.C1 = 0.078, .C2 = 1.2, .C3 = 1.0, .C4 = 1.0};
        const double Ah_eff = coeff.C1 * std::pow(permeabilityFactor, coeff.C2);
        const EffectiveMultipliers mult{.Mfront = Ah_eff,
                                        .Mleft = openness.Dl * coeff.C3,
                                        .Mright = openness.Dr * coeff.C3,
                                        .Mtop = openness.Dtop * coeff.C4,
                                        .Mbot = openness.Dbot * coeff.C4,
                                        .PermeabilityFactor = permeabilityFactor};
        return {thickness, mult, coeff};
    }

    EffectiveLayerProperties
      makeHorizontalVenetianValues(const double thickness,
                                   const FenestrationCommon::Venetian::Geometry & geometry,
                                   const ShadeOpenness & openness) noexcept
    {
        constexpr Coefficients coeff{.C1 = 0.016, .C2 = -0.63, .C3 = 0.53, .C4 = 0.043};
        const double permeabilityFactor =
          ThermalPermeability::Venetian::permeabilityFactor(thickness, geometry);

        const double tilt = FenestrationCommon::radians(geometry.SlatTiltAngle);
        const double Ah_eff =
          coeff.C1 * std::pow(permeabilityFactor * std::pow(std::cos(tilt), coeff.C2), coeff.C3);

        const EffectiveMultipliers mult{.Mfront = Ah_eff,
                                        .Mleft = 0.0,
                                        .Mright = 0.0,
                                        .Mtop = openness.Dtop,
                                        .Mbot = openness.Dbot,
                                        .PermeabilityFactor = permeabilityFactor};
        const double t_eff =
          coeff.C4 * (geometry.SlatWidth * std::cos(tilt) + thickness * std::sin(tilt));
        return {t_eff, mult, coeff};
    }

    EffectiveLayerProperties
      makeVerticalVenetianValues(const double thickness,
                                 const FenestrationCommon::Venetian::Geometry & geometry,
                                 const ShadeOpenness & openness) noexcept
    {
        constexpr Coefficients coeff{.C1 = 0.041, .C2 = 0.0, .C3 = 0.27, .C4 = 0.012};
        const double permeabilityFactor =
          ThermalPermeability::Venetian::permeabilityFactor(thickness, geometry);

        const double tilt = FenestrationCommon::radians(geometry.SlatTiltAngle);
        const double Ah_eff =
          coeff.C1 * std::pow(permeabilityFactor * std::pow(std::cos(tilt), coeff.C2), coeff.C3);

        const EffectiveMultipliers mult{.Mfront = Ah_eff,
                                        .Mleft = 0.0,
                                        .Mright = 0.0,
                                        .Mtop = openness.Dtop,
                                        .Mbot = openness.Dbot,
                                        .PermeabilityFactor = permeabilityFactor};

        const double t_eff =
          coeff.C4 * (geometry.SlatWidth * std::cos(tilt) + thickness * std::sin(tilt));
        return {t_eff, mult, coeff};
    }

    EffectiveLayerProperties
      makePerforatedValues(const double thickness,
                           const FenestrationCommon::Perforated::Geometry & geometry,
                           const ShadeOpenness & openness) noexcept
    {
        const double permeabilityFactor =
          ThermalPermeability::Perforated::permeabilityFactor(geometry);
        return makeCommonValues(thickness, permeabilityFactor, openness);
    }

    EffectiveLayerProperties makeWovenValues(const double thickness,
                                             const FenestrationCommon::Woven::Geometry & geometry,
                                             const ShadeOpenness & openness) noexcept
    {
        const double permeabilityFactor = ThermalPermeability::Woven::permeabilityFactor(geometry);
        return makeCommonValues(thickness, permeabilityFactor, openness);
    }

    EffectiveLayerProperties
      makeLouveredShutterValues(const FenestrationCommon::LouveredShutter::Geometry & geometry,
                                const ShadeOpenness & openness) noexcept
    {
        constexpr Coefficients coeff{.C1 = 0.12, .C2 = 0.82, .C3 = 0.059, .C4 = 0.11};
        const double permeabilityFactor =
          ThermalPermeability::LouveredShutter::permeabilityFactor(geometry);

        const double ang = FenestrationCommon::radians(geometry.SlatAngle);
        const double openness_factor =
          std::max(0.0, 1.0 - std::pow(2.0 / FenestrationCommon::WCE_PI * ang, 4));
        const double Ah_eff =
          std::max(0.0,
                   (coeff.C1 * std::pow(permeabilityFactor * openness_factor, coeff.C2))
                     - (coeff.C3 * std::cos(ang)));

        (void)openness;
        const EffectiveMultipliers mult{.Mfront = Ah_eff,
                                        .Mleft = 0.0,
                                        .Mright = 0.0,
                                        .Mtop = 0.0,
                                        .Mbot = 0.0,
                                        .PermeabilityFactor = permeabilityFactor};

        const double t_eff =
          coeff.C4
          * (geometry.SlatWidth * std::cos(ang) + geometry.SlatThickness * std::abs(std::sin(ang)));

        return {t_eff, mult, coeff};
    }

    EffectiveLayerProperties makeUserDefinedValues(const double thickness,
                                                   const double permeabilityFactor,
                                                   const double effectiveFrontThermalOpennessArea,
                                                   const ShadeOpenness & openness) noexcept
    {
        constexpr Coefficients coeff{.C1 = 0, .C2 = 0, .C3 = 0, .C4 = 0};
        const EffectiveMultipliers mult{.Mfront = effectiveFrontThermalOpennessArea,
                                        .Mleft = openness.Dl,
                                        .Mright = openness.Dr,
                                        .Mtop = openness.Dtop,
                                        .Mbot = openness.Dbot,
                                        .PermeabilityFactor = permeabilityFactor};
        return {thickness, mult, coeff};
    }


}   // namespace EffectiveLayers
