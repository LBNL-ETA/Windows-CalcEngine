#include "WholeWindow.hpp"

#include <utility>

namespace Tarcog::ISO15099
{
    ////////////////////////////////////////////////
    /// WindowSingleVision
    ////////////////////////////////////////////////

    WindowSingleVision WindowSingleVision::Create(
      double width, double height, double tvis, double tsol, std::shared_ptr<IIGUSystem> iguSystem)
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

    void WindowSingleVision::setDividers(FrameData frameData, size_t nHorizontal, size_t nVertical)
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
    {
        averageHc();
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

    ////////////////////////////////////////////////
    /// DualVisionHorizontal
    ////////////////////////////////////////////////

    DualVisionHorizontal::DualVisionHorizontal(double width,
                                               double height,
                                               double tvis1,
                                               double tsol1,
                                               const std::shared_ptr<IIGUSystem> & iguSystem1,
                                               double tvis2,
                                               double tsol2,
                                               const std::shared_ptr<IIGUSystem> & iguSystem2) :
        WindowDualVision(width / 2, height, tvis1, tsol1, iguSystem1, tvis2, tsol2, iguSystem2)
    {
        const std::map<FramePosition, FrameType> leftVisionFrameTypes{
          {FramePosition::Top, FrameType::Exterior},
          {FramePosition::Bottom, FrameType::Exterior},
          {FramePosition::Left, FrameType::Exterior},
          {FramePosition::Right, FrameType::Interior}};
        m_Vision1.setFrameTypes(leftVisionFrameTypes);

        const std::map<FramePosition, FrameType> rightVisionFrameTypes{
          {FramePosition::Top, FrameType::Exterior},
          {FramePosition::Bottom, FrameType::Exterior},
          {FramePosition::Left, FrameType::Interior},
          {FramePosition::Right, FrameType::Exterior}};
        m_Vision2.setFrameTypes(rightVisionFrameTypes);
    }

    void DualVisionHorizontal::setFrameTopLeft(FrameData frameData)
    {
        m_Vision1.setFrameData(FramePosition::Top, frameData);
    }

    void DualVisionHorizontal::setFrameTopRight(FrameData frameData)
    {
        m_Vision2.setFrameData(FramePosition::Top, frameData);
    }

    void DualVisionHorizontal::setFrameBottomLeft(FrameData frameData)
    {
        m_Vision1.setFrameData(FramePosition::Bottom, frameData);
    }

    void DualVisionHorizontal::setFrameBottomRight(FrameData frameData)
    {
        m_Vision2.setFrameData(FramePosition::Bottom, frameData);
    }

    void DualVisionHorizontal::setFrameLeft(FrameData frameData)
    {
        m_Vision1.setFrameData(FramePosition::Left, frameData);
    }

    void DualVisionHorizontal::setFrameRight(FrameData frameData)
    {
        m_Vision2.setFrameData(FramePosition::Right, frameData);
    }

    void DualVisionHorizontal::setMeetingRail(FrameData frameData)
    {
        frameData.splitFrameWidth();
        m_Vision1.setFrameData(FramePosition::Right, frameData);
        m_Vision2.setFrameData(FramePosition::Left, frameData);
    }

    void
      DualVisionHorizontal::setDividers(FrameData frameData, size_t nHorizontal, size_t nVertical)
    {
        m_Vision1.setDividers(frameData, nHorizontal, nVertical);
        m_Vision2.setDividers(frameData, nHorizontal, nVertical);
    }

    void DualVisionHorizontal::setDividersLeftVision(FrameData frameData,
                                                     size_t nHorizontal,
                                                     size_t nVertical)
    {
        m_Vision1.setDividers(frameData, nHorizontal, nVertical);
    }

    void DualVisionHorizontal::setDividersRightVision(FrameData frameData,
                                                      size_t nHorizontal,
                                                      size_t nVertical)
    {
        m_Vision2.setDividers(frameData, nHorizontal, nVertical);
    }

    ////////////////////////////////////////////////
    /// DualVisionVertical
    ////////////////////////////////////////////////

    DualVisionVertical::DualVisionVertical(double width,
                                           double height,
                                           double tvis1,
                                           double tsol1,
                                           const std::shared_ptr<IIGUSystem> & iguSystem1,
                                           double tvis2,
                                           double tsol2,
                                           const std::shared_ptr<IIGUSystem> & iguSystem2) :
        WindowDualVision(width, height / 2, tvis1, tsol1, iguSystem1, tvis2, tsol2, iguSystem2)
    {
        const std::map<FramePosition, FrameType> topVisionFrameTypes{
          {FramePosition::Top, FrameType::Exterior},
          {FramePosition::Bottom, FrameType::Interior},
          {FramePosition::Left, FrameType::Exterior},
          {FramePosition::Right, FrameType::Exterior}};
        m_Vision1.setFrameTypes(topVisionFrameTypes);

        const std::map<FramePosition, FrameType> bottomVisionFrameTypes{
          {FramePosition::Top, FrameType::Interior},
          {FramePosition::Bottom, FrameType::Exterior},
          {FramePosition::Left, FrameType::Exterior},
          {FramePosition::Right, FrameType::Exterior}};
        m_Vision2.setFrameTypes(bottomVisionFrameTypes);

        // In case of vertical vision we must adjust exterior surface height for film coefficient
        // calculations since visions will scale that coefficient only to their heights.
        m_Vision1.setExteriorSurfaceHeight(height);
        m_Vision2.setExteriorSurfaceHeight(height);
    }

    void DualVisionVertical::setFrameMettingRail(FrameData frameData)
    {
        frameData.splitFrameWidth();
        m_Vision1.setFrameData(FramePosition::Bottom, frameData);
        m_Vision2.setFrameData(FramePosition::Top, frameData);
    }

    void DualVisionVertical::setDividers(FrameData frameData, size_t nHorizontal, size_t nVertical)
    {
        m_Vision1.setDividers(frameData, nHorizontal, nVertical);
        m_Vision2.setDividers(frameData, nHorizontal, nVertical);
    }

    void DualVisionVertical::setDividersTopVision(FrameData frameData,
                                                  size_t nHorizontal,
                                                  size_t nVertical)
    {
        m_Vision1.setDividers(frameData, nHorizontal, nVertical);
    }

    void DualVisionVertical::setDividersBottomVision(FrameData frameData,
                                                     size_t nHorizontal,
                                                     size_t nVertical)
    {
        m_Vision2.setDividers(frameData, nHorizontal, nVertical);
    }

    void DualVisionVertical::setFrameBottomRight(FrameData frameData)
    {
        m_Vision2.setFrameData(FramePosition::Right, frameData);
    }

    void DualVisionVertical::setFrameBottomLeft(FrameData frameData)
    {
        m_Vision2.setFrameData(FramePosition::Left, frameData);
    }

    void DualVisionVertical::setFrameTopRight(FrameData frameData)
    {
        m_Vision1.setFrameData(FramePosition::Right, frameData);
    }

    void DualVisionVertical::setFrameTopLeft(FrameData frameData)
    {
        m_Vision1.setFrameData(FramePosition::Left, frameData);
    }

    void DualVisionVertical::setFrameBottom(FrameData frameData)
    {
        m_Vision2.setFrameData(FramePosition::Bottom, frameData);
    }

    void DualVisionVertical::setFrameTop(FrameData frameData)
    {
        m_Vision1.setFrameData(FramePosition::Top, frameData);
    }
}   // namespace Tarcog::ISO15099
