#pragma once

#include <memory>
#include "BaseLayer.hpp"

namespace FenestrationCommon
{
    enum class Side;
}

namespace Tarcog::ISO15099
{
    class CBaseIGULayer : public Tarcog::ISO15099::CBaseLayer
    {
    public:
        explicit CBaseIGULayer(double t_Thickness);
        virtual ~CBaseIGULayer() = default;

        double getThickness() const override;
        virtual double getBaseThickness() const final;
        virtual double getMaxDeflection() const = 0;
        virtual double getMeanDeflection() const = 0;

        double getConductivity();
        double getEffectiveThermalConductivity();

    private:
        double m_Thickness;
    };
}   // namespace Tarcog::ISO15099
