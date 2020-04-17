#include <cmath>
#include <WCECommon.hpp>
#include "EffectiveOpenness.hpp"

namespace EffectiveLayers
{
    ShadeOpenness::ShadeOpenness(double ah, double dl, double dr, double dtop, double dbot) :
        Ah(ah), Dl(dl), Dr(dr), Dtop(dtop), Dbot(dbot)
    {}

    EffectiveOpenness::EffectiveOpenness(
      const double ah, const double al, const double ar, const double atop, const double abot) :
        Ah(ah), Al(al), Ar(ar), Atop(atop), Abot(abot)
    {}

    EffectiveLayer::EffectiveLayer(double width,
                                   double height,
                                   double thickness,
                                   const ShadeOpenness & openness,
                                   Coefficients coefficients) :
        m_Width(width),
        m_Height(height),
        m_Thickness(thickness),
        m_ShadeOpenness(openness),
        coefficients{coefficients}
    {}

    Coefficients::Coefficients(double c1, double c2, double c3, double c4) :
        C1(c1), C2(c2), C3(c3), C4(c4)
    {}

    EffectiveVenetian::EffectiveVenetian(double width,
                                         double height,
                                         double thickness,
                                         const ShadeOpenness & openness,
                                         double slatAngle,
                                         double slatWidth,
                                         Coefficients coefficients) :
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

        return {Ah_eff, 0, 0, m_ShadeOpenness.Dtop * m_Width, m_ShadeOpenness.Dbot * m_Width};
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
          width, height, thickness, openness, slatAngle, slatWidth, {0.016, -0.63, 0.53, 0.043})
    {}

    EffectiveVerticalVenentian::EffectiveVerticalVenentian(double width,
                                                           double height,
                                                           double thickness,
                                                           const ShadeOpenness & openness,
                                                           double slatAngle,
                                                           double slatWidth) :
        EffectiveVenetian(
          width, height, thickness, openness, slatAngle, slatWidth, {0.041, 0.0, 0.27, 0.012})
    {}
}   // namespace EffectiveLayers