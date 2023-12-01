#pragma once

namespace Tarcog
{
    class Deflectable
    {
    public:
        virtual ~Deflectable() = default;

        [[nodiscard]] virtual double getMaxDeflection() const = 0;
        [[nodiscard]] virtual double getMeanDeflection() const = 0;
    };
}