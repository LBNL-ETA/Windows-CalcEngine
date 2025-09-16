#include <cmath>
#include <WCECommon.hpp>
#include "EffectiveMultipliers.hpp"

namespace EffectiveLayers {
    bool isClosed(const EffectiveMultipliers &effectiveOpenness) {
        return effectiveOpenness.Mfront == 0.0
               && effectiveOpenness.Mleft == 0.0 && effectiveOpenness.Mrigh == 0.0
               && effectiveOpenness.Mtop == 0.0 && effectiveOpenness.Mbot == 0.0
               && effectiveOpenness.PermeabilityFactor == 0.0;
    }

    EffectiveLayer::EffectiveLayer(double thickness,
                                   const ShadeOpenness &openness,
                                   const Coefficients &coefficients,
                                   double permeabilityFactor) : m_Thickness(thickness),
                                                                m_ShadeOpenness(
                                                                    openness.Dl, openness.Dr, openness.Dtop,
                                                                    openness.Dbot),
                                                                coefficients(coefficients),
                                                                m_PermeabilityFactor(permeabilityFactor) {
    }

    double EffectiveLayer::permeabilityFactor() const {
        return m_PermeabilityFactor;
    }

    EffectiveVenetian::EffectiveVenetian(double thickness,
                                         const FenestrationCommon::Venetian::Geometry &geometry,
                                         const ShadeOpenness &openness,
                                         const Coefficients &coefficients) : EffectiveLayer(thickness,
                                                                                 openness,
                                                                                 coefficients,
                                                                                 ThermalPermeability::Venetian::permeabilityFactor(
                                                                                     thickness, geometry)),
                                                                             m_Geometry(geometry) {
    }

    EffectiveMultipliers EffectiveVenetian::getEffectiveOpenness() {
        const auto Ah_eff{
            coefficients.C1
            * std::pow(m_PermeabilityFactor
                       * std::pow(std::cos(FenestrationCommon::radians(m_Geometry.SlatTiltAngle)),
                                  coefficients.C2),
                       coefficients.C3)
        };

        return {
            Ah_eff,
            0,
            0,
            m_ShadeOpenness.Dtop,
            m_ShadeOpenness.Dbot,
            m_PermeabilityFactor
        };
    }

    double EffectiveVenetian::effectiveThickness() {
        return coefficients.C4
               * (m_Geometry.SlatWidth
                  * std::cos(FenestrationCommon::radians(m_Geometry.SlatTiltAngle))
                  + m_Thickness * std::sin(FenestrationCommon::radians(m_Geometry.SlatTiltAngle)));
    }

    EffectiveHorizontalVenetian::EffectiveHorizontalVenetian(
        double thickness,
        const FenestrationCommon::Venetian::Geometry &geometry,
        const ShadeOpenness &openness) : EffectiveVenetian(thickness, geometry, openness, {0.016, -0.63, 0.53, 0.043}) {
    }

    EffectiveVerticalVenetian::EffectiveVerticalVenetian(
        double thickness,
        const FenestrationCommon::Venetian::Geometry &geometry,
        const ShadeOpenness &openness) : EffectiveVenetian(thickness, geometry, openness, {0.041, 0.0, 0.27, 0.012}) {
    }

    EffectiveLayerCommon::EffectiveLayerCommon(double thickness,
                                               double permeabilityFactor,
                                               const ShadeOpenness &openness) : EffectiveLayer(
        thickness, openness, {0.078, 1.2, 1.0, 1.0}, permeabilityFactor) {
    }

    EffectiveMultipliers EffectiveLayerCommon::getEffectiveOpenness() {
        const auto Ah_eff{
            coefficients.C1
            * (std::pow(m_PermeabilityFactor, coefficients.C2))
        };
        const auto Al_eff{m_ShadeOpenness.Dl * coefficients.C3};
        const auto Ar_eff{m_ShadeOpenness.Dr * coefficients.C3};
        const auto Atop_eff{m_ShadeOpenness.Dtop * coefficients.C4};
        const auto Abot_eff{m_ShadeOpenness.Dbot * coefficients.C4};
        return {Ah_eff, Al_eff, Ar_eff, Atop_eff, Abot_eff, m_PermeabilityFactor};
    }

    double EffectiveLayerCommon::effectiveThickness() {
        return m_Thickness;
    }

    EffectiveLayerLouveredShutter::EffectiveLayerLouveredShutter(
        const FenestrationCommon::LouveredShutter::Geometry &geometry,
        const ShadeOpenness &openness) : EffectiveLayer(geometry.SlatThickness,
                                                        openness,
                                                        {1.385e-01, 8.805e-01, 0.0580255, 1.225e-01},
                                                        ThermalPermeability::LouveredShutter::permeabilityFactor(
                                                            geometry)),
                                         m_Geometry(geometry) {
    }

    EffectiveMultipliers EffectiveLayerLouveredShutter::getEffectiveOpenness() {
        const auto openness_factor{
            std::max(0.0,
                     1
                     - std::pow(2 / FenestrationCommon::WCE_PI
                                * FenestrationCommon::radians(m_Geometry.SlatAngle),
                                4))
        };
        const auto Ah_eff{
            std::max(
                0.0,
                coefficients.C1
                * (std::pow(m_PermeabilityFactor * openness_factor, coefficients.C2))
                - coefficients.C3 * std::cos(FenestrationCommon::radians(m_Geometry.SlatAngle)))
        };

        // Side openings are forbidden for louvered shutters for now
        return {Ah_eff, 0.0, 0.0, 0.0, 0.0, m_PermeabilityFactor};
    }

    double EffectiveLayerLouveredShutter::effectiveThickness() {
        return coefficients.C4
               * (m_Geometry.SlatWidth * std::cos(FenestrationCommon::radians(m_Geometry.SlatAngle))
                  + m_Geometry.SlatThickness
                  * std::abs(std::sin(FenestrationCommon::radians(m_Geometry.SlatAngle))));
    }

    EffectiveLayerPerforated::EffectiveLayerPerforated(
        double thickness,
        const FenestrationCommon::Perforated::Geometry &geometry,
        const ShadeOpenness &openness) : EffectiveLayerCommon(thickness,
                                                              ThermalPermeability::Perforated::permeabilityFactor(
                                                                  geometry),
                                                              openness) {
    }

    EffectiveLayerWoven::EffectiveLayerWoven(double thickness,
                                             const FenestrationCommon::Woven::Geometry &geometry,
                                             const ShadeOpenness &openness) : EffectiveLayerCommon(thickness,
        ThermalPermeability::Woven::permeabilityFactor(geometry),
        openness) {
    }


    EffectiveLayerUserDefined::EffectiveLayerUserDefined(double thickness,
                                                         double permeabilityFactor,
                                                         double effectiveFrontThermalOpennessArea,
                                                         const ShadeOpenness &openness) : EffectiveLayer(
            thickness, openness, {0, 0, 0, 0}, permeabilityFactor),
        m_EffectiveFrontThermalOpennessMultiplier(effectiveFrontThermalOpennessArea) {
    }

    EffectiveMultipliers EffectiveLayerUserDefined::getEffectiveOpenness() {
        const auto Al_eff{m_ShadeOpenness.Dl};
        const auto Ar_eff{m_ShadeOpenness.Dr};
        const auto Atop_eff{m_ShadeOpenness.Dtop};
        const auto Abot_eff{m_ShadeOpenness.Dbot};
        return {
            m_EffectiveFrontThermalOpennessMultiplier,
            Al_eff,
            Ar_eff,
            Atop_eff,
            Abot_eff,
            m_PermeabilityFactor
        };
    }

    double EffectiveLayerUserDefined::effectiveThickness() {
        return m_Thickness;
    }
} // namespace EffectiveLayers
