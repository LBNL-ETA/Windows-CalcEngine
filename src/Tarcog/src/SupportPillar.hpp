#ifndef SUPPORTPILLAR_H
#define SUPPORTPILLAR_H

#include "IGUGapLayer.hpp"

namespace Tarcog
{
    class CSupportPillar : public CIGUGapLayer
    {
    public:
        CSupportPillar(CIGUGapLayer const & t_Layer, double const t_Conductivity);

    protected:
        void calculateConvectionOrConductionFlow() override;
        virtual double conductivityOfPillarArray() = 0;
        double m_Conductivity;
    };

    class CCircularPillar : public CSupportPillar
    {
    public:
        CCircularPillar(const CIGUGapLayer & t_Gap,
                        double t_Conductivity,
                        double t_Spacing,
                        double t_Radius);

		std::shared_ptr<CBaseLayer> clone() const override;

    private:
        double conductivityOfPillarArray() override;
        double m_Spacing;
        double m_Radius;
    };

}   // namespace Tarcog

#endif
