#pragma once

namespace Tarcog
{
    enum class FramePosition
    {
        Top,
        Bottom,
        Left,
        Right
    };

    class IVision
    {
    public:
        [[nodiscard]] virtual double uValue() const = 0;
        [[nodiscard]] virtual double shgc() const = 0;
        [[nodiscard]] virtual double vt() const = 0;
        [[nodiscard]] virtual double area() const = 0;
        [[nodiscard]] virtual double hc() const = 0;
    };

    class IWindow
    {
    public:
        [[nodiscard]] virtual double area() const = 0;
        [[nodiscard]] virtual double uValue() const = 0;
        [[nodiscard]] virtual double shgc() const = 0;
        [[nodiscard]] virtual double vt() const = 0;
    };

}   // namespace Tarcog
