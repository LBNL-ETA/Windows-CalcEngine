#pragma once

#include "WholeWindowConfigurations.hpp"
#include "WindowVision.hpp"

namespace Tarcog::ISO15099
{
    // Enum representing the positions of a single vision frame in a window.
    // These positions include Top, Bottom, Left, and Right.
    enum class SingleVisionFramePosition
    {
        Top,
        Bottom,
        Left,
        Right
    };

    // Alias for a map that associates a frame position with its corresponding frame data.
    // This is used to store and manage frame data for each position in a single vision window.
    using SingleVisionFrameMap = std::map<SingleVisionFramePosition, FrameData>;

    ////////////////////////////////////////////////
    /// WindowSingleVision
    ////////////////////////////////////////////////
    class WindowSingleVision : public IWindow
    {
    public:
        WindowSingleVision() = default;

        WindowSingleVision(double width,
                           double height,
                           double tvis,
                           double tsol,
                           std::shared_ptr<IIGUSystem> iguSystem);

        [[nodiscard]] double area() const override;
        [[nodiscard]] double uValue() const override;
        [[nodiscard]] double shgc() const override;
        [[nodiscard]] double shgc(double tSol) const override;
        [[nodiscard]] double vt() const override;
        [[nodiscard]] double vt(double tVis) const override;
        [[nodiscard]] double uValueCOG() const;
        [[nodiscard]] double shgcCOG() const;

        [[nodiscard]] double uValueCOGAverage() const override;
        [[nodiscard]] double shgcCOGAverage() const override;

        void setFrameData(SingleVisionFramePosition position, const FrameData & frameData);
        void setFrameData(const SingleVisionFrameMap & frames);

        void setDividers(const FrameData & frameData, size_t nHorizontal, size_t nVertical);
        void setDividersAuto(const FrameData & frameData);

        [[nodiscard]] IGUDimensions getIGUDimensions() const override;

        void setUValueIGUTolerance(double uValue) override;
        void setThicknessIGUTolerance(double thickness) override;

        [[nodiscard]] IGUMismatch iguMissmatch() const override;

    protected:
        [[nodiscard]] double visionPercentage() const override;

    private:
        WindowVision vision;
    };

    ////////////////////////////////////////////////
    /// WindowDualVisionBase
    ////////////////////////////////////////////////

    template<typename Derived>
    class WindowDualVisionBase : public IWindow
    {
        WindowDualVisionBase() = default;

    protected:
        [[nodiscard]] virtual const WindowVision & vision1() const
        {
            return static_cast<const Derived &>(*this).vision1();
        }

        [[nodiscard]] virtual const WindowVision & vision2() const
        {
            return static_cast<const Derived &>(*this).vision2();
        }

        virtual WindowVision & vision1()
        {
            return static_cast<Derived &>(*this).vision1();
        }
        virtual WindowVision & vision2()   // NOLINT(*-member-function-hidden)
        {
            return static_cast<Derived &>(*this).vision2();
        }

        void averageHc()
        {
            const auto hcavg = (vision1().hc() + vision2().hc()) / 2;
            vision1().setHc(hcavg);
            vision2().setHc(hcavg);
        }

    public:
        [[nodiscard]] double area() const override
        {
            return vision1().area() + vision2().area();
        }

        [[nodiscard]] double uValue() const override
        {
            return (vision1().uValue() * vision1().area() + vision2().uValue() * vision2().area())
                   / area();
        }

        [[nodiscard]] double shgc() const override
        {
            return (vision1().shgc() * vision1().area() + vision2().shgc() * vision2().area())
                   / area();
        }

        [[nodiscard]] double shgc(double tSol1, double tSol2) const
        {
            return (vision1().shgc(tSol1) * vision1().area()
                    + vision2().shgc(tSol2) * vision2().area())
                   / area();
        }

        [[nodiscard]] double shgc(double tSol) const override
        {
            return shgc(tSol, tSol);
        }

        [[nodiscard]] double vt() const override
        {
            return (vision1().vt() * vision1().area() + vision2().vt() * vision2().area()) / area();
        }

        [[nodiscard]] double vt(double tVis1, double tVis2) const
        {
            return (vision1().vt(tVis1) * vision1().area() + vision2().vt(tVis2) * vision2().area())
                   / area();
        }

        [[nodiscard]] double vt(double tVis) const override
        {
            return vt(tVis, tVis);
        }

        [[nodiscard]] double uValueCOGAverage() const override
        {
            return (vision1().uValueCOG() * vision1().area()
                    + vision2().uValueCOG() * vision2().area())
                   / area();
        }

        [[nodiscard]] double shgcCOGAverage() const override
        {
            return (vision1().shgcCOG() * vision1().area() + vision2().shgcCOG() * vision2().area())
                   / area();
        }

        [[nodiscard]] IGUDimensions getIGUDimensions() const override
        {
            return {vision1().getIGUWidth(), vision1().getIGUHeight()};
        }

        void setUValueIGUTolerance(double uValue) override
        {
            vision1().setUValueIGUTolerance(uValue);
            vision2().setUValueIGUTolerance(uValue);
        }

        void setThicknessIGUTolerance(double thickness) override
        {
            vision1().setThicknessIGUTolerance(thickness);
            vision2().setThicknessIGUTolerance(thickness);
        }

        [[nodiscard]] IGUMismatch iguMissmatch() const override
        {
            const auto mismatch1 = vision1().iguMissmatch();
            const auto mismatch2 = vision2().iguMissmatch();

            return {.uCenterMissmatch = mismatch1.uCenterMissmatch || mismatch2.uCenterMissmatch,
                    .thicknessMissmatch =
                      mismatch1.thicknessMissmatch || mismatch2.thicknessMissmatch};
        }

    protected:
        [[nodiscard]] double visionPercentage() const override
        {
            return (vision1().visionPercentage() * vision1().area()
                    + vision2().visionPercentage() * vision2().area())
                   / area();
        }
        friend Derived;
    };

    ////////////////////////////////////////////////
    /// DualVisionHorizontal
    ////////////////////////////////////////////////

    enum class DualHorizontalFramePosition : uint8_t
    {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        Left,
        Right,
        MeetingRail
    };

    using DualHorizontalFrameMap = std::map<DualHorizontalFramePosition, FrameData>;

    //! Concrete implementation of dual vision horizontal slider
    //! Vision1 is the vision on the left side and Vision2 is the vision on the right side.
    class DualVisionHorizontal : public WindowDualVisionBase<DualVisionHorizontal>
    {
    public:
        DualVisionHorizontal() = default;

        DualVisionHorizontal(double width,
                             double height,
                             double tvis1,
                             double tsol1,
                             const std::shared_ptr<IIGUSystem> & iguSystem1,
                             double tvis2,
                             double tsol2,
                             const std::shared_ptr<IIGUSystem> & iguSystem2);

        [[nodiscard]] double uValueCOGLeft() const;
        [[nodiscard]] double uValueCOGRight() const;
        [[nodiscard]] double shgcCOGLeft() const;
        [[nodiscard]] double shgcCOGRight() const;

        void setFrameData(DualHorizontalFramePosition position, const FrameData & frameData);
        void setFrameData(const DualHorizontalFrameMap & frames);

        void setDividers(FrameData frameData, size_t nHorizontal, size_t nVertical);
        void setDividersAuto(const FrameData & frameData);

        void setDividersLeftVision(FrameData frameData, size_t nHorizontal, size_t nVertical);
        void setDividersLeftVisionAuto(const FrameData & frameData);
        void setDividersRightVision(FrameData frameData, size_t nHorizontal, size_t nVertical);
        void setDividersRightVisionAuto(const FrameData & frameData);

        WindowVision & vision1() override
        {
            return m_LeftVision;
        }

        WindowVision & vision2() override
        {
            return m_RightVision;
        }

        [[nodiscard]] const WindowVision & vision1() const override
        {
            return m_LeftVision;
        }

        [[nodiscard]] const WindowVision & vision2() const override
        {
            return m_RightVision;
        }

    private:
        WindowVision m_LeftVision;
        WindowVision m_RightVision;
    };

    ////////////////////////////////////////////////
    /// DualVisionVertical
    ////////////////////////////////////////////////

    enum class DualVerticalFramePosition : uint8_t
    {
        Top,
        Bottom,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        MeetingRail
    };

    using DualVerticalFrameMap = std::map<DualVerticalFramePosition, FrameData>;

    //! Concrete implementation of dual vision vertical slider
    //! Vision1 is the vision on the top and Vision2 is the vision on the bottom.
    class DualVisionVertical : public WindowDualVisionBase<DualVisionVertical>
    {
    public:
        DualVisionVertical() = default;

        DualVisionVertical(double width,
                           double height,
                           double tvis1,
                           double tsol1,
                           const std::shared_ptr<IIGUSystem> & iguSystem1,
                           double tvis2,
                           double tsol2,
                           const std::shared_ptr<IIGUSystem> & iguSystem2);

        [[nodiscard]] double uValueCOGTop() const;
        [[nodiscard]] double uValueCOGBottom() const;
        [[nodiscard]] double shgcCOGTop() const;
        [[nodiscard]] double shgcCOGBottom() const;

        void setFrameData(DualVerticalFramePosition position, const FrameData & frameData);
        void setFrameData(const DualVerticalFrameMap & frames);

        void setDividers(FrameData frameData, size_t nHorizontal, size_t nVertical);
        void setDividersAuto(const FrameData & frameData);

        void setDividersTopVision(FrameData frameData, size_t nHorizontal, size_t nVertical);
        void setDividersTopVisionAuto(const FrameData & frameData);
        void setDividersBottomVision(FrameData frameData, size_t nHorizontal, size_t nVertical);
        void setDividersBottomVisionAuto(const FrameData & frameData);

        // Required by CRTP base
        WindowVision & vision1() override
        {
            return m_TopVision;
        }

        WindowVision & vision2() override
        {
            return m_BottomVision;
        }

        [[nodiscard]] const WindowVision & vision1() const override
        {
            return m_TopVision;
        }

        [[nodiscard]] const WindowVision & vision2() const override
        {
            return m_BottomVision;
        }

    private:
        WindowVision m_TopVision;
        WindowVision m_BottomVision;
    };

}   // namespace Tarcog::ISO15099
