#include <cmath>
#include <WCECommon.hpp>
#include "EffectiveOpenness.hpp"

namespace EffectiveLayers
{
    ShadeOpenness::ShadeOpenness(double dl, double dr, double dtop, double dbot) :
        Dl(dl), Dr(dr), Dtop(dtop), Dbot(dbot)
    {}

    EffectiveOpenness::EffectiveOpenness(const double effectiveFrontThermalOpennessArea,
                                         const double al,
                                         const double ar,
                                         const double atop,
                                         const double abot,
                                         const double permeabilityFactor) :
        EffectiveFrontThermalOpennessArea(effectiveFrontThermalOpennessArea),
        Al(al),
        Ar(ar),
        Atop(atop),
        Abot(abot),
        PermeabilityFactor(permeabilityFactor)
    {}

    bool isClosed(const EffectiveOpenness & effectiveOpenness)
    {
        return effectiveOpenness.EffectiveFrontThermalOpennessArea == 0.0
               && effectiveOpenness.Al == 0.0 && effectiveOpenness.Ar == 0.0
               && effectiveOpenness.Atop == 0.0 && effectiveOpenness.Abot == 0.0
               && effectiveOpenness.PermeabilityFactor == 0.0;
    }

    EffectiveLayer::EffectiveLayer(double width,
                                   double height,
                                   double thickness,
                                   const ShadeOpenness & openness,
                                   const Coefficients & coefficients,
                                   double permeabilityFactor) :
        m_Width(width),
        m_Height(height),
        m_Thickness(thickness),
        m_ShadeOpenness(openness.Dl, openness.Dr, openness.Dtop, openness.Dbot),
        coefficients(coefficients),
        m_PermeabilityFactor(permeabilityFactor)
    {}

    double EffectiveLayer::permeabilityFactor() const
    {
        return m_PermeabilityFactor;
    }

    Coefficients::Coefficients(double c1, double c2, double c3, double c4) :
        C1(c1), C2(c2), C3(c3), C4(c4)
    {}

    EffectiveVenetian::EffectiveVenetian(double width,
                                         double height,
                                         double thickness,
                                         const FenestrationCommon::Venetian::Geometry & geometry,
                                         const ShadeOpenness & openness,
                                         const Coefficients & coefficients) :
        EffectiveLayer(width,
                       height,
                       thickness,
                       openness,
                       coefficients,
                       ThermalPermeability::Venetian::permeabilityFactor(thickness, geometry)),
        m_Geometry(geometry)
    {}

    EffectiveOpenness EffectiveVenetian::getEffectiveOpenness()
    {
        const auto area{m_Width * m_Height};
        const auto Ah_eff{
          area * coefficients.C1
          * std::pow(m_PermeabilityFactor
                       * std::pow(std::cos(FenestrationCommon::radians(m_Geometry.SlatTiltAngle)),
                                  coefficients.C2),
                     coefficients.C3)};

        return {Ah_eff,
                0,
                0,
                m_ShadeOpenness.Dtop * m_Width,
                m_ShadeOpenness.Dbot * m_Width,
                m_PermeabilityFactor};
    }

    double EffectiveVenetian::effectiveThickness()
    {
        return coefficients.C4
               * (m_Geometry.SlatWidth
                    * std::cos(FenestrationCommon::radians(m_Geometry.SlatTiltAngle))
                  + m_Thickness * std::sin(FenestrationCommon::radians(m_Geometry.SlatTiltAngle)));
    }

    EffectiveHorizontalVenetian::EffectiveHorizontalVenetian(
      double width,
      double height,
      double thickness,
      const FenestrationCommon::Venetian::Geometry & geometry,
      const ShadeOpenness & openness) :
        EffectiveVenetian(width, height, thickness, geometry, openness, {0.016, -0.63, 0.53, 0.043})
    {}

    EffectiveVerticalVenetian::EffectiveVerticalVenetian(
      double width,
      double height,
      double thickness,
      const FenestrationCommon::Venetian::Geometry & geometry,
      const ShadeOpenness & openness) :
        EffectiveVenetian(width, height, thickness, geometry, openness, {0.041, 0.0, 0.27, 0.012})
    {}

    EffectiveLayerCommon::EffectiveLayerCommon(double width,
                                               double height,
                                               double thickness,
                                               double permeabilityFactor,
                                               const ShadeOpenness & openness) :
        EffectiveLayer(
          width, height, thickness, openness, {0.078, 1.2, 1.0, 1.0}, permeabilityFactor)
    {}

    EffectiveOpenness EffectiveLayerCommon::getEffectiveOpenness()
    {
        const auto area{m_Width * m_Height};
        const auto Ah_eff{area * coefficients.C1
                          * (std::pow(m_PermeabilityFactor, coefficients.C2))};
        const auto Al_eff{m_ShadeOpenness.Dl * m_Height * coefficients.C3};
        const auto Ar_eff{m_ShadeOpenness.Dr * m_Height * coefficients.C3};
        const auto Atop_eff{m_ShadeOpenness.Dtop * m_Width * coefficients.C4};
        const auto Abop_eff{m_ShadeOpenness.Dbot * m_Width * coefficients.C4};
        return {Ah_eff, Al_eff, Ar_eff, Atop_eff, Abop_eff, m_PermeabilityFactor};
    }

    double EffectiveLayerCommon::effectiveThickness()
    {
        return m_Thickness;
    }

    EffectiveLayerPerforated::EffectiveLayerPerforated(
      double width,
      double height,
      double thickness,
      const FenestrationCommon::Perforated::Geometry & geometry,
      const ShadeOpenness & openness) :
        EffectiveLayerCommon(width,
                             height,
                             thickness,
                             ThermalPermeability::Perforated::permeabilityFactor(geometry),
                             openness)
    {}

    EffectiveLayerWoven::EffectiveLayerWoven(double width,
                                             double height,
                                             double thickness,
                                             const FenestrationCommon::Woven::Geometry & geometry,
                                             const ShadeOpenness & openness) :
        EffectiveLayerCommon(width,
                             height,
                             thickness,
                             ThermalPermeability::Woven::permeabilityFactor(geometry),
                             openness)
    {}
}   // namespace EffectiveLayers
