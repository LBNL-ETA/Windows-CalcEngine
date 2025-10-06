#include "WholeWindow.hpp"

#include <utility>
#include <array>
#include <map>
#include <ranges>
#include <algorithm>
#include <stdexcept>

namespace Tarcog::ISO15099
{
    namespace Helper
    {
        inline constexpr std::array kFrameMappingPairs{
          std::pair{SingleVisionFramePosition::Top, FramePosition::Top},
          std::pair{SingleVisionFramePosition::Bottom, FramePosition::Bottom},
          std::pair{SingleVisionFramePosition::Left, FramePosition::Left},
          std::pair{SingleVisionFramePosition::Right, FramePosition::Right},
        };

        inline const std::map kFrameMapping{std::begin(kFrameMappingPairs),
                                            std::end(kFrameMappingPairs)};
    }   // namespace Helper

    ////////////////////////////////////////////////
    /// WindowSingleVision
    ////////////////////////////////////////////////
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

    double WindowSingleVision::shgc(const double tSol) const
    {
        return vision.shgc(tSol);
    }

    double WindowSingleVision::vt() const
    {
        return vision.vt();
    }

    double WindowSingleVision::vt(const double tVis) const
    {
        return vision.vt(tVis);
    }

    double WindowSingleVision::uValueCOG() const
    {
        return vision.uValueCOG();
    }

    double WindowSingleVision::shgcCOG() const
    {
        return vision.shgcCOG();
    }

    double WindowSingleVision::uValueCOGAverage() const
    {
        return vision.uValueCOG();
    }

    double WindowSingleVision::shgcCOGAverage() const
    {
        return vision.shgcCOG();
    }

    double WindowSingleVision::visionPercentage() const
    {
        return vision.visionPercentage();
    }

    void WindowSingleVision::setFrameData(SingleVisionFramePosition position,
                                          const FrameData & frameData)
    {
        vision.setFrameData(Helper::kFrameMapping.at(position), frameData);
    }

    void WindowSingleVision::setFrameData(const SingleVisionFrameMap & frames)
    {
        std::ranges::for_each(frames, [this](const auto & pair) {
            auto [position, frameData] = pair;
            setFrameData(position, frameData);
        });
    }

    double WindowSingleVision::getFrameArea(const SingleVisionFramePosition position) const
    {
        return frameArea(vision.frame(Helper::kFrameMapping.at(position)));
    }

    double
      WindowSingleVision::getFrameEdgeOfGlassArea(const SingleVisionFramePosition position) const
    {
        return edgeOfGlassArea(vision.frame(Helper::kFrameMapping.at(position)));
    }

    void WindowSingleVision::setDividers(const FrameData & frameData,
                                         size_t nHorizontal,
                                         size_t nVertical)
    {
        vision.setDividers(frameData, nHorizontal, nVertical);
    }

    void WindowSingleVision::setDividersAuto(const FrameData & frameData)
    {
        vision.setDividersAuto(frameData);
    }

    IGUDimensions WindowSingleVision::getIGUDimensions() const
    {
        return {vision.getIGUWidth(), vision.getIGUHeight()};
    }

    void WindowSingleVision::setUValueIGUTolerance(double uValue)
    {
        vision.setUValueIGUTolerance(uValue);
    }

    void WindowSingleVision::setThicknessIGUTolerance(double thickness)
    {
        vision.setThicknessIGUTolerance(thickness);
    }

    IGUMismatch WindowSingleVision::iguMissmatch(const double geometricalThickness) const
    {
        return vision.iguMissmatch(geometricalThickness);
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
        m_LeftVision(width / 2, height, tvis1, tsol1, iguSystem1),
        m_RightVision(width / 2, height, tvis2, tsol2, iguSystem2)
    {
        averageHc();

        m_LeftVision.setFrameTypes({{FramePosition::Top, FrameType::Exterior},
                                    {FramePosition::Bottom, FrameType::Exterior},
                                    {FramePosition::Left, FrameType::Exterior},
                                    {FramePosition::Right, FrameType::Interior}});

        m_RightVision.setFrameTypes({{FramePosition::Top, FrameType::Exterior},
                                     {FramePosition::Bottom, FrameType::Exterior},
                                     {FramePosition::Left, FrameType::Interior},
                                     {FramePosition::Right, FrameType::Exterior}});
    }

    double DualVisionHorizontal::uValueCOGLeft() const
    {
        return vision1().uValueCOG();
    }

    double DualVisionHorizontal::uValueCOGRight() const
    {
        return vision2().uValueCOG();
    }

    double DualVisionHorizontal::shgcCOGLeft() const
    {
        return vision1().shgcCOG();
    }

    double DualVisionHorizontal::shgcCOGRight() const
    {
        return vision2().shgcCOG();
    }

    void DualVisionHorizontal::setFrameData(DualHorizontalFramePosition position,
                                            const FrameData & frameData)
    {
        switch(position)
        {
            case DualHorizontalFramePosition::TopLeft:
                vision1().setFrameData(FramePosition::Top, frameData);
                break;
            case DualHorizontalFramePosition::TopRight:
                vision2().setFrameData(FramePosition::Top, frameData);
                break;
            case DualHorizontalFramePosition::BottomLeft:
                vision1().setFrameData(FramePosition::Bottom, frameData);
                break;
            case DualHorizontalFramePosition::BottomRight:
                vision2().setFrameData(FramePosition::Bottom, frameData);
                break;
            case DualHorizontalFramePosition::Left:
                vision1().setFrameData(FramePosition::Left, frameData);
                break;
            case DualHorizontalFramePosition::Right:
                vision2().setFrameData(FramePosition::Right, frameData);
                break;
            case DualHorizontalFramePosition::MeetingRail: {
                FrameData split = splitFrameWidth(frameData);
                vision1().setFrameData(FramePosition::Right, split);
                vision2().setFrameData(FramePosition::Left, split);
                break;
            }
            default:
                throw std::invalid_argument("Invalid frame position");
        }
    }

    void DualVisionHorizontal::setFrameData(const DualHorizontalFrameMap & frames)
    {
        std::ranges::for_each(frames, [this](const auto & pair) {
            auto [position, frameData] = pair;
            setFrameData(position, frameData);
        });
    }

    void
      DualVisionHorizontal::setDividers(FrameData frameData, size_t nHorizontal, size_t nVertical)
    {
        vision1().setDividers(frameData, nHorizontal, nVertical);
        vision2().setDividers(frameData, nHorizontal, nVertical);
    }

    void DualVisionHorizontal::setDividersAuto(const FrameData & frameData)
    {
        vision1().setDividersAuto(frameData);
        vision2().setDividersAuto(frameData);
    }

    void DualVisionHorizontal::setDividersLeftVision(FrameData frameData,
                                                     size_t nHorizontal,
                                                     size_t nVertical)
    {
        vision1().setDividers(frameData, nHorizontal, nVertical);
    }

    void DualVisionHorizontal::setDividersLeftVisionAuto(const FrameData & frameData)
    {
        vision1().setDividersAuto(frameData);
    }

    void DualVisionHorizontal::setDividersRightVision(FrameData frameData,
                                                      size_t nHorizontal,
                                                      size_t nVertical)
    {
        vision2().setDividers(frameData, nHorizontal, nVertical);
    }

    void DualVisionHorizontal::setDividersRightVisionAuto(const FrameData & frameData)
    {
        vision2().setDividersAuto(frameData);
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
        m_TopVision(width, height / 2, tvis1, tsol1, iguSystem1),
        m_BottomVision(width, height / 2, tvis2, tsol2, iguSystem2)
    {
        averageHc();

        m_TopVision.setFrameTypes({{FramePosition::Top, FrameType::Exterior},
                                   {FramePosition::Bottom, FrameType::Interior},
                                   {FramePosition::Left, FrameType::Exterior},
                                   {FramePosition::Right, FrameType::Exterior}});

        m_BottomVision.setFrameTypes({{FramePosition::Top, FrameType::Interior},
                                      {FramePosition::Bottom, FrameType::Exterior},
                                      {FramePosition::Left, FrameType::Exterior},
                                      {FramePosition::Right, FrameType::Exterior}});

        // Both visions use the full height for calculating surface height
        m_TopVision.setInteriorAndExteriorSurfaceHeight(height);
        m_BottomVision.setInteriorAndExteriorSurfaceHeight(height);
    }

    double DualVisionVertical::uValueCOGTop() const
    {
        return vision1().uValueCOG();
    }

    double DualVisionVertical::uValueCOGBottom() const
    {
        return vision2().uValueCOG();
    }

    double DualVisionVertical::shgcCOGTop() const
    {
        return vision1().shgcCOG();
    }

    double DualVisionVertical::shgcCOGBottom() const
    {
        return vision2().shgcCOG();
    }

    void DualVisionVertical::setFrameData(DualVerticalFramePosition position,
                                          const FrameData & frameData)
    {
        switch(position)
        {
            case DualVerticalFramePosition::Top:
                vision1().setFrameData(FramePosition::Top, frameData);
                break;

            case DualVerticalFramePosition::Bottom:
                vision2().setFrameData(FramePosition::Bottom, frameData);
                break;

            case DualVerticalFramePosition::TopLeft:
                vision1().setFrameData(FramePosition::Left, frameData);
                break;

            case DualVerticalFramePosition::TopRight:
                vision1().setFrameData(FramePosition::Right, frameData);
                break;

            case DualVerticalFramePosition::BottomLeft:
                vision2().setFrameData(FramePosition::Left, frameData);
                break;

            case DualVerticalFramePosition::BottomRight:
                vision2().setFrameData(FramePosition::Right, frameData);
                break;

            case DualVerticalFramePosition::MeetingRail: {
                FrameData split = splitFrameWidth(frameData);
                vision1().setFrameData(FramePosition::Bottom, split);
                vision2().setFrameData(FramePosition::Top, split);
                break;
            }

            default:
                throw std::invalid_argument("Invalid frame position");
        }
    }

    void DualVisionVertical::setFrameData(const DualVerticalFrameMap & frames)
    {
        std::ranges::for_each(frames, [this](const auto & pair) {
            auto [position, frameData] = pair;
            setFrameData(position, frameData);
        });
    }

    void DualVisionVertical::setDividers(const FrameData & frameData,
                                         const size_t nHorizontal,
                                         const size_t nVertical)
    {
        vision1().setDividers(frameData, nHorizontal, nVertical);
        vision2().setDividers(frameData, nHorizontal, nVertical);
    }

    void DualVisionVertical::setDividersAuto(const FrameData & frameData)
    {
        vision1().setDividersAuto(frameData);
        vision2().setDividersAuto(frameData);
    }

    void DualVisionVertical::setDividersTopVision(const FrameData & frameData,
                                                  const size_t nHorizontal,
                                                  const size_t nVertical)
    {
        vision1().setDividers(frameData, nHorizontal, nVertical);
    }

    void DualVisionVertical::setDividersTopVisionAuto(const FrameData & frameData)
    {
        vision1().setDividersAuto(frameData);
    }

    void DualVisionVertical::setDividersBottomVision(const FrameData & frameData,
                                                     const size_t nHorizontal,
                                                     const size_t nVertical)
    {
        vision2().setDividers(frameData, nHorizontal, nVertical);
    }

    void DualVisionVertical::setDividersBottomVisionAuto(const FrameData & frameData)
    {
        vision2().setDividersAuto(frameData);
    }

}   // namespace Tarcog::ISO15099
