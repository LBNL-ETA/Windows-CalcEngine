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
        explicit CBaseIGULayer(double thickness) : CBaseLayer(thickness) {}
        virtual ~CBaseIGULayer() = default;

        virtual double getMaxDeflection() const = 0;
        virtual double getMeanDeflection() const = 0;
    };
}   // namespace Tarcog::ISO15099
