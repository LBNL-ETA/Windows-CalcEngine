#include "WholeWindow.hpp"

#include <utility>

namespace Tarcog::ISO15099
{
    ////////////////////////////////////////////////
    /// WindowSingleVision
    ////////////////////////////////////////////////

    WindowSingleVision WindowSingleVision::Create(double width,
        double height,
        double tvis,
        double tsol,
        std::shared_ptr<IIGUSystem> iguSystem)
    {
        return WindowSingleVision(width, height, tvis, tsol, std::move(iguSystem));
    }

    WindowSingleVision::WindowSingleVision(double width,
                                           double height,
                                           double tvis,
                                           double tsol,
                                           std::shared_ptr<IIGUSystem> iguSystem) :
        vision(width, height, tvis, tsol, std::move(iguSystem))
    {}

    double WindowSingleVision::area() const
    {
        return vision.area();
    }

    double WindowSingleVision::uValue() const
    {
        return vision.uValue();
    }

    double WindowSingleVision::shgc() const
    {
        return vision.shgc();
    }

    double WindowSingleVision::vt() const
    {
        return vision.vt();
    }

    void WindowSingleVision::setFrameTop(FrameData frameData)
    {
        vision.setFrameData(FramePosition::Top, frameData);
    }

    void WindowSingleVision::setFrameBottom(FrameData frameData)
    {
        vision.setFrameData(FramePosition::Bottom, frameData);
    }

    void WindowSingleVision::setFrameLeft(FrameData frameData)
    {
        vision.setFrameData(FramePosition::Left, frameData);
    }

    void WindowSingleVision::setFrameRight(FrameData frameData)
    {
        vision.setFrameData(FramePosition::Right, frameData);
    }

    void WindowSingleVision::setDividers(FrameData frameData,
        size_t nHorizontal,
        size_t nVertical)
    {
        vision.setDividers(frameData, nHorizontal, nVertical);
    }

    ////////////////////////////////////////////////
    /// WindowDualVision
    ////////////////////////////////////////////////

    WindowDualVision::WindowDualVision(double width,
                                       double height,
                                       double tvis1,
                                       double tsol1,
                                       std::shared_ptr<IIGUSystem> iguSystem1,
                                       double tvis2,
                                       double tsol2,
                                       std::shared_ptr<IIGUSystem> iguSystem2) :
        m_Vision1(width, height, tvis1, tsol1, std::move(iguSystem1)),
        m_Vision2(width, height, tvis2, tsol2, std::move(iguSystem2))
    {}

    WindowDualVision WindowDualVision::CreateHorizontal(double width,
                                                        double height,
                                                        double tvis_left,
                                                        double tsol_left,
                                                        std::shared_ptr<IIGUSystem> iguSystem_left,
                                                        double tvis_right,
                                                        double tsol_right,
                                                        std::shared_ptr<IIGUSystem> iguSystem_right)
    {
        return WindowDualVision(width / 2,
                                height,
                                tvis_left,
                                tsol_left,
                                std::move(iguSystem_left),
                                tvis_right,
                                tsol_right,
                                std::move(iguSystem_right));
    }

    double WindowDualVision::area() const
    {
        return m_Vision1.area() + m_Vision2.area();
    }

    double WindowDualVision::uValue() const
    {
        return (m_Vision1.uValue() * m_Vision1.area() + m_Vision2.uValue() * m_Vision2.area())
               / area();
    }

    double WindowDualVision::shgc() const
    {
        return (m_Vision1.shgc() * m_Vision1.area() + m_Vision2.shgc() * m_Vision2.area()) / area();
    }

    double WindowDualVision::vt() const
    {
        return (m_Vision1.vt() * m_Vision1.area() + m_Vision2.vt() * m_Vision2.area()) / area();
    }

    void WindowDualVision::averageHc()
    {
        const auto hc1{m_Vision1.hc()};
        const auto hc2{m_Vision2.hc()};
        const auto hcavg{(hc1 + hc2) / 2};
        m_Vision1.setHc(hcavg);
        m_Vision2.setHc(hcavg);
    }
}   // namespace Tarcog::ISO15099
