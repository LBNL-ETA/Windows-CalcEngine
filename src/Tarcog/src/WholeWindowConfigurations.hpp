#pragma once

namespace Tarcog
{
    //! Window configurations will be used in various classes to determine which algorithms should
    //! be used to calculate properties
    enum class WindowConfiguration
    {
        SingleVision,
        HorizontalDualVision,
        VerticalDualVision
    };

    //! Basic definition for whole window calculations.
    class IWholeWindow
    {
    public:
        virtual double uValue() const = 0;
        virtual double shgc() const = 0;
        virtual double vt() const = 0;
    };

    enum class FramePosition
    {
        Top,
        Bottom,
        Left,
        Right
    };

    class IVision : public IWholeWindow
    {
    public:
        virtual double area() const = 0;
        virtual double hc() const = 0;
    };

}   // namespace Tarcog
