
#include <cmath>
#include <functional>

#include "WCECommon.hpp"
#include "BSDFPatch.hpp"
#include "BeamDirection.hpp"

namespace SingleLayerOptics
{
    /////////////////////////////////////////////////////////////////
    ///  AngleLimits
    /////////////////////////////////////////////////////////////////

    AngleLimits::AngleLimits(double const t_Low, double const t_High) : m_Low(t_Low), m_High(t_High)
    {}

    AngleLimits::AngleLimits(double t_High) : m_Type(Type::Central), m_Low(0), m_High(t_High)
    {}

    double AngleLimits::low() const
    {
        return m_Low;
    }

    double AngleLimits::high() const
    {
        return m_High;
    }

    double AngleLimits::delta() const
    {
        return m_High > m_Low ? m_High - m_Low : m_High + (360 - m_Low);
    }

    bool AngleLimits::isInLimits(const double t_Angle) const
    {
        // To assure that negative patch angles are covered as well
        double aAngle = (m_Low + 360) < t_Angle ? t_Angle - 360 : t_Angle;
        return (aAngle >= m_Low) && (aAngle <= m_High);
    }

    double AngleLimits::average() const
    {
        const std::map<Type, std::function<double()>> func{
          {Type::NonCentral, [&]() { return (m_Low + m_High) / 2; }},
          {Type::Central, [&]() { return m_Low; }}};
        return func.at(m_Type)();
    }

    /////////////////////////////////////////////////////////////////
    ///  CBSDFPatch
    /////////////////////////////////////////////////////////////////

    CBSDFPatch::CBSDFPatch(const AngleLimits & t_Theta, const AngleLimits & t_Phi) :
        m_Theta(t_Theta),
        m_Phi(t_Phi),
        m_Lambda(calculateLambda(m_Theta.low(), m_Theta.high(), m_Phi.delta()))
    {}

    CBeamDirection CBSDFPatch::centerPoint() const
    {
        return {m_Theta.average(), m_Phi.average()};
    }

    double CBSDFPatch::lambda() const
    {
        return m_Lambda;
    }

    bool CBSDFPatch::isInPatch(const double t_Theta, const double t_Phi) const
    {
        return m_Theta.isInLimits(t_Theta) && m_Phi.isInLimits(t_Phi);
    }

    double CBSDFPatch::calculateLambda(double lowerTheta, double upperTheta, double phiDelta)
    {
        using ConstantsData::WCE_PI;

        const double thetaLow = lowerTheta * WCE_PI / 180;
        const double thetaHigh = upperTheta * WCE_PI / 180;
        const double deltaPhi = phiDelta * WCE_PI / 180;
        return 0.5 * deltaPhi
               * (std::sin(thetaHigh) * std::sin(thetaHigh)
                  - std::sin(thetaLow) * std::sin(thetaLow));
    }

}   // namespace SingleLayerOptics
