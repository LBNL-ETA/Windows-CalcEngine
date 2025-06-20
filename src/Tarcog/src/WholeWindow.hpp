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
        void setFrameData(const SingleVisionFrameMap& frames);

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
    /// WindowDualVision
    ////////////////////////////////////////////////

    class WindowDualVision : public IWindow
    {
    public:
        WindowDualVision() = default;

        [[nodiscard]] double area() const override;
        [[nodiscard]] double uValue() const override;
        [[nodiscard]] double shgc() const override;
        [[nodiscard]] double shgc(double tSol1, double tSol2) const;
        //! Both vision areas have same solar transmittance
        [[nodiscard]] double shgc(double tSol) const override;
        [[nodiscard]] double vt() const override;
        [[nodiscard]] double vt(double tVis1, double tVis2) const;
        //! Both vision areas have same visible transmittance
        [[nodiscard]] double vt(double tVis) const override;

        [[nodiscard]] double uValueCOGAverage() const override;
        [[nodiscard]] double shgcCOGAverage() const override;

        [[nodiscard]] IGUDimensions getIGUDimensions() const override;

        void setUValueIGUTolerance(double uValue) override;
        void setThicknessIGUTolerance(double thickness) override;

        [[nodiscard]] IGUMismatch iguMissmatch() const override;

    protected:
        WindowDualVision(double width,
                         double height,
                         double tvis1,
                         double tsol1,
                         std::shared_ptr<IIGUSystem> iguSystem1,
                         double tvis2,
                         double tsol2,
                         std::shared_ptr<IIGUSystem> iguSystem2);

        [[nodiscard]] double visionPercentage() const override;

        [[nodiscard]] double uValueCOG1() const;
        [[nodiscard]] double uValueCOG2() const;
        [[nodiscard]] double shgcCOG1() const;
        [[nodiscard]] double shgcCOG2() const;

        WindowVision m_Vision1;
        WindowVision m_Vision2;

    private:
        //! Makes single convective film coefficient averaged over the both visions
        void averageHc();
    };

    ////////////////////////////////////////////////
    /// DualVisionHorizontal
    ////////////////////////////////////////////////

    enum class DualHorizontalFramePosition {
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
    class DualVisionHorizontal : public WindowDualVision
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
    };

    ////////////////////////////////////////////////
    /// DualVisionVertical
    ////////////////////////////////////////////////

    enum class DualVerticalFramePosition {
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
    class DualVisionVertical : public WindowDualVision
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
    };
}   // namespace Tarcog::ISO15099
