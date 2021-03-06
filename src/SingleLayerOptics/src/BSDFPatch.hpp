#ifndef BSDFPATCH_H
#define BSDFPATCH_H

#include <memory>

namespace SingleLayerOptics
{
    class CBeamDirection;

    class CAngleLimits
    {
    public:
        CAngleLimits(double const t_Low, double const t_High);
        double low() const;
        double high() const;
        double delta() const;
        bool isInLimits(const double t_Angle) const;
        virtual double average() const;

    protected:
        double m_Low;
        double m_High;
    };

    class CCentralAngleLimits : public CAngleLimits
    {
    public:
        explicit CCentralAngleLimits(double t_High);
        double average() const override;
    };

    class CBSDFPatch
    {
    public:
        CBSDFPatch(const std::shared_ptr<CAngleLimits> & t_Theta, const CAngleLimits & t_Phi);
        CBeamDirection centerPoint() const;
        double lambda() const;
        bool isInPatch(double t_Theta, double t_Phi) const;

    private:
        void calculateLambda();
        std::shared_ptr<CAngleLimits> m_Theta;
        const CAngleLimits m_Phi;
        double m_Lambda;
    };

}   // namespace SingleLayerOptics

#endif
