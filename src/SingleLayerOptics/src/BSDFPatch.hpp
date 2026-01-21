#ifndef BSDFPATCH_H
#define BSDFPATCH_H

#include <memory>

#include "BeamDirection.hpp"

namespace SingleLayerOptics
{

    class AngleLimits
    {
    public:
        AngleLimits(double t_Low, double t_High);
        AngleLimits(double t_High);
        [[nodiscard]] double low() const;
        [[nodiscard]] double high() const;
        [[nodiscard]] double delta() const;
        [[nodiscard]] bool isInLimits(const double t_Angle) const;
        virtual double average() const;

    protected:
        enum class Type
        {
            Central,
            NonCentral
        };
        Type m_Type{Type::NonCentral};
        double m_Low;
        double m_High;
    };

    class CBSDFPatch
    {
    public:
        CBSDFPatch(const AngleLimits & t_Theta, const AngleLimits & t_Phi);
        [[nodiscard]] const CBeamDirection & centerPoint() const;
        [[nodiscard]] double lambda() const;
        [[nodiscard]] bool isInPatch(double t_Theta, double t_Phi) const;

    private:
        double calculateLambda(double lowerTheta, double upperTheta, double phiDelta);
        AngleLimits m_Theta;
        AngleLimits m_Phi;
        double m_Lambda;
        CBeamDirection m_CenterPoint;  // Cached center point
    };

}   // namespace SingleLayerOptics

#endif
