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

    //! Data structure for IGU mismatch in WholeWindow calculations.
    //! If any of the frames is not within the tolerance, the mismatch is set to true.
    struct IGUMismatch
    {
        bool uCenterMissmatch{false};
        bool thicknessMissmatch{false};

        [[nodiscard]] bool any() const noexcept
        {
            return uCenterMissmatch || thicknessMissmatch;
        }
    };

    class IVision
    {
    public:
        virtual ~IVision() = default;

        [[nodiscard]] virtual double uValue() const = 0;
        [[nodiscard]] virtual double shgc() const = 0;
        [[nodiscard]] virtual double shgc(double tSol) const = 0;
        [[nodiscard]] virtual double vt() const = 0;
        [[nodiscard]] virtual double vt(double tVis) const = 0;
        [[nodiscard]] virtual double visionPercentage() const = 0;
        [[nodiscard]] virtual double area() const = 0;
        [[nodiscard]] virtual double hc() const = 0;

        [[nodiscard]] virtual IGUMismatch iguMissmatch() const = 0;


        virtual void setHc(double hc) = 0;
    };

    struct IGUDimensions
    {
        IGUDimensions(double width, double height);

        double width;
        double height;
    };

    inline IGUDimensions::IGUDimensions(double width, double height) : width(width), height(height)
    {}

    class IWindow
    {
    public:
        virtual ~IWindow() = default;

        [[nodiscard]] virtual double area() const = 0;
        [[nodiscard]] virtual double uValue() const = 0;
        [[nodiscard]] virtual double shgc() const = 0;
        [[nodiscard]] virtual double shgc(double tSol) const = 0;
        [[nodiscard]] virtual double vt() const = 0;
        [[nodiscard]] virtual double vt(double tVis) const = 0;
        [[nodiscard]] virtual double visionPercentage() const = 0;

        [[nodiscard]] virtual double uValueCOGAverage() const = 0;
        [[nodiscard]] virtual double shgcCOGAverage() const = 0;

        [[nodiscard]] virtual IGUDimensions getIGUDimensions() const = 0;

        virtual void setUValueIGUTolerance(double uValue) = 0;
        virtual void setThicknessIGUTolerance(double thickness) = 0;

        [[nodiscard]] virtual IGUMismatch iguMissmatch() const = 0;
    };

}   // namespace Tarcog
