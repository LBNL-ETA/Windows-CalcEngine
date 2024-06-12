#include <cmath>
#include <WCECommon.hpp>
#include "EffectiveOpenness.hpp"

namespace EffectiveLayers
{
    ShadeOpenness::ShadeOpenness(double ah, double dl, double dr, double dtop, double dbot) :
        Ah(ah), Dl(dl), Dr(dr), Dtop(dtop), Dbot(dbot)
    {}

    EffectiveOpenness::EffectiveOpenness(const double ah,
                                         const double al,
                                         const double ar,
                                         const double atop,
                                         const double abot,
                                         const double frontPorosity) :
        Ah(ah), Al(al), Ar(ar), Atop(atop), Abot(abot), FrontPorosity(frontPorosity)
    {}

    bool isClosed(const EffectiveOpenness & effectiveOpenness)
    {
        return effectiveOpenness.Ah == 0.0 && effectiveOpenness.Al == 0.0
               && effectiveOpenness.Ar == 0.0 && effectiveOpenness.Atop == 0.0
               && effectiveOpenness.Abot == 0.0 && effectiveOpenness.FrontPorosity == 0.0;
    }

    EffectiveLayer::EffectiveLayer(double width,
                                   double height,
                                   double thickness,
                                   const ShadeOpenness & openness,
                                   const Coefficients & coefficients) :
        m_Width(width),
        m_Height(height),
        m_Thickness(thickness),
        m_ShadeOpenness(
          openness.Ah * width * height, openness.Dl, openness.Dr, openness.Dtop, openness.Dbot),
        coefficients{coefficients}
    {}

    Coefficients::Coefficients(double c1, double c2, double c3, double c4) :
        C1(c1), C2(c2), C3(c3), C4(c4)
    {}

    EffectiveVenetian::EffectiveVenetian(double width,
                                         double height,
                                         double thickness,
                                         double slatAngle,
                                         double slatWidth,
                                         const ShadeOpenness & openness,
                                         const Coefficients & coefficients) :
        EffectiveLayer(width, height, thickness, openness, coefficients),
        m_SlatAngleRad(slatAngle * 2.0 * ConstantsData::WCE_PI / 360.0),
        m_SlatWidth(slatWidth)
    {}

    EffectiveOpenness EffectiveVenetian::getEffectiveOpenness()
    {
        const auto area{m_Width * m_Height};
        const auto Ah_eff{area * coefficients.C1
                          * std::pow(m_ShadeOpenness.Ah / area
                                       * std::pow(std::cos(m_SlatAngleRad), coefficients.C2),
                                     coefficients.C3)};

        return {Ah_eff,
                0,
                0,
                m_ShadeOpenness.Dtop * m_Width,
                m_ShadeOpenness.Dbot * m_Width,
                m_ShadeOpenness.Ah};
    }

    double EffectiveVenetian::effectiveThickness()
    {
        return coefficients.C4
               * (m_SlatWidth * std::cos(m_SlatAngleRad) + m_Thickness * std::sin(m_SlatAngleRad));
    }

    EffectiveHorizontalVenetian::EffectiveHorizontalVenetian(double width,
                                                             double height,
                                                             double thickness,
                                                             const ShadeOpenness & openness,
                                                             double slatAngle,
                                                             double slatWidth) :
        EffectiveVenetian(
          width, height, thickness, slatAngle, slatWidth, openness, {0.016, -0.63, 0.53, 0.043})
    {}

    EffectiveVerticalVenetian::EffectiveVerticalVenetian(double width,
                                                         double height,
                                                         double thickness,
                                                         const ShadeOpenness & openness,
                                                         double slatAngle,
                                                         double slatWidth) :
        EffectiveVenetian(
          width, height, thickness, slatAngle, slatWidth, openness, {0.041, 0.0, 0.27, 0.012})
    {}

    EffectiveLayerCommonType::EffectiveLayerCommonType(double width,
                                                       double height,
                                                       double thickness,
                                                       const ShadeOpenness & openness) :
        EffectiveLayer(width, height, thickness, openness, {0.078, 1.2, 1.0, 1.0})
    {}

    EffectiveOpenness EffectiveLayerCommonType::getEffectiveOpenness()
    {
        const auto area{m_Width * m_Height};
        const auto Ah_eff{area * coefficients.C1
                          * (std::pow(m_ShadeOpenness.Ah / area, coefficients.C2))};
        const auto Al_eff{m_ShadeOpenness.Dl * m_Height * coefficients.C3};
        const auto Ar_eff{m_ShadeOpenness.Dr * m_Height * coefficients.C3};
        const auto Atop_eff{m_ShadeOpenness.Dtop * m_Width * coefficients.C4};
        const auto Abop_eff{m_ShadeOpenness.Dbot * m_Width * coefficients.C4};
        return {Ah_eff, Al_eff, Ar_eff, Atop_eff, Abop_eff, m_ShadeOpenness.Ah};
    }

    double EffectiveLayerCommonType::effectiveThickness()
    {
        return m_Thickness;
    }

    EffectiveLayerPerforated::EffectiveLayerPerforated(double width,
                                                       double height,
                                                       double thickness,
                                                       const ShadeOpenness & openness) :
        EffectiveLayerCommonType(width, height, thickness, openness)
    {}

    EffectiveLayerDiffuse::EffectiveLayerDiffuse(double width,
                                                 double height,
                                                 double thickness,
                                                 const ShadeOpenness & openness) :
        EffectiveLayerCommonType(width, height, thickness, openness)
    {}

    EffectiveLayerWoven::EffectiveLayerWoven(double width,
                                             double height,
                                             double thickness,
                                             const ShadeOpenness & openness) :
        EffectiveLayerCommonType(width, height, thickness, openness)
    {}

    EffectiveLayerBSDF::EffectiveLayerBSDF(double width,
                                           double height,
                                           double thickness,
                                           const ShadeOpenness & openness) :
        EffectiveLayerCommonType(width, height, thickness, openness)
    {}

    EffectiveLayerOther::EffectiveLayerOther(double width,
                                             double height,
                                             double thickness,
                                             const ShadeOpenness & openness) :
        EffectiveLayer(width, height, thickness, openness)
    {}
    EffectiveOpenness EffectiveLayerOther::getEffectiveOpenness()
    {
        return {m_ShadeOpenness.Ah,
                m_ShadeOpenness.Dl * m_Height,
                m_ShadeOpenness.Dr * m_Height,
                m_ShadeOpenness.Dtop * m_Width,
                m_ShadeOpenness.Dbot * m_Width,
                m_ShadeOpenness.Ah};
    }

    double EffectiveLayerOther::effectiveThickness()
    {
        return m_Thickness;
    }
}   // namespace EffectiveLayers
