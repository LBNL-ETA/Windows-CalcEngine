#include "WholeWindow.hpp"

#include <utility>
#include <array>
#include <map>
#include <algorithm>
#include <optional>
#include <numeric>
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

        inline constexpr std::array kAllSinglePositions{
          SingleVisionFramePosition::Top,
          SingleVisionFramePosition::Bottom,
          SingleVisionFramePosition::Left,
          SingleVisionFramePosition::Right,
        };
    }   // namespace Helper

    ////////////////////////////////////////////////
    /// WindowSingleVision
    ////////////////////////////////////////////////
    WindowSingleVision::WindowSingleVision(double width,
                                           double height,
                                           double tvis,
                                           double tsol,
                                           std::shared_ptr<IIGUSystem> iguSystem) :
        m_vision(width, height, tvis, tsol, std::move(iguSystem))
    {}

    double WindowSingleVision::area() const
    {
        return m_vision.area();
    }

    double WindowSingleVision::uValue() const
    {
        return m_vision.uValue();
    }

    double WindowSingleVision::shgc() const
    {
        return m_vision.shgc();
    }

    double WindowSingleVision::shgc(const double tSol) const
    {
        return m_vision.shgc(tSol);
    }

    double WindowSingleVision::shgc0() const
    {
        return m_vision.shgc0();
    }

    double WindowSingleVision::shgc1() const
    {
        return m_vision.shgc1();
    }

    double WindowSingleVision::vt() const
    {
        return m_vision.vt();
    }

    double WindowSingleVision::vt(const double tVis) const
    {
        return m_vision.vt(tVis);
    }

    double WindowSingleVision::vt0() const
    {
        return m_vision.vt0();
    }

    double WindowSingleVision::vt1() const
    {
        return m_vision.vt1();
    }

    double WindowSingleVision::uValueCOG() const
    {
        return m_vision.uValueCOG();
    }

    double WindowSingleVision::shgcCOG() const
    {
        return m_vision.shgcCOG();
    }

    double WindowSingleVision::uValueCOGAverage() const
    {
        return m_vision.uValueCOG();
    }

    double WindowSingleVision::shgcCOGAverage() const
    {
        return m_vision.shgcCOG();
    }

    double WindowSingleVision::visionPercentage() const
    {
        return m_vision.visionPercentage();
    }

    void WindowSingleVision::setFrameData(SingleVisionFramePosition position,
                                          const FrameData & frameData)
    {
        m_vision.setFrameData(Helper::kFrameMapping.at(position), frameData);
    }

    void WindowSingleVision::setFrameData(const SingleVisionFrameMap & frames)
    {
        std::ranges::for_each(frames, [this](const auto & pair) {
            auto [position, frameData] = pair;
            setFrameData(position, frameData);
        });
    }
    std::map<SingleVisionFramePosition, Frame> WindowSingleVision::frames() const
    {
        std::map<SingleVisionFramePosition, Frame> out;
        for(const auto pos : Helper::kAllSinglePositions)
        {
            const FramePosition fp = Helper::kFrameMapping.at(pos);
            out.emplace(pos, m_vision.frame(fp));
        }
        return out;
    }

    double WindowSingleVision::getFrameArea(const SingleVisionFramePosition position) const
    {
        return frameArea(m_vision.frame(Helper::kFrameMapping.at(position)));
    }

    double
      WindowSingleVision::getFrameEdgeOfGlassArea(const SingleVisionFramePosition position) const
    {
        return edgeOfGlassArea(m_vision.frame(Helper::kFrameMapping.at(position)));
    }

    double WindowSingleVision::getFrameArea() const
    {
        double total = 0.0;
        for(const auto pos : Helper::kAllSinglePositions)
        {
            total += getFrameArea(pos);
        }
        return total;
    }

    double WindowSingleVision::getFrameEdgeOfGlassArea() const
    {
        double total = 0.0;
        for(const auto pos : Helper::kAllSinglePositions)
        {
            total += getFrameEdgeOfGlassArea(pos);
        }
        return total;
    }

    void WindowSingleVision::setDividers(const FrameData & frameData,
                                         size_t nHorizontal,
                                         size_t nVertical)
    {
        m_vision.setDividers(frameData, nHorizontal, nVertical);
    }

    void WindowSingleVision::setDividersAuto(const FrameData & frameData)
    {
        m_vision.setDividersAuto(frameData);
    }

    double WindowSingleVision::getDividerArea() const
    {
        return m_vision.dividerArea();
    }

    double WindowSingleVision::getDividerEdgeOfGlassArea() const
    {
        return m_vision.dividerEdgeOfGlassArea();
    }

    std::optional<DividerData> WindowSingleVision::divider() const
    {
        return m_vision.divider();
    }

    IGUDimensions WindowSingleVision::getIGUDimensions() const
    {
        return {m_vision.getIGUWidth(), m_vision.getIGUHeight()};
    }

    void WindowSingleVision::setUValueIGUTolerance(double uValue)
    {
        m_vision.setUValueIGUTolerance(uValue);
    }

    void WindowSingleVision::setThicknessIGUTolerance(double thickness)
    {
        m_vision.setThicknessIGUTolerance(thickness);
    }

    const WindowVision & WindowSingleVision::vision() const
    {
        return m_vision;
    }

    IGUMismatch WindowSingleVision::iguMissmatch(const double geometricalThickness) const
    {
        return m_vision.iguMissmatch(geometricalThickness);
    }

    ////////////////////////////////////////////////
    /// DualVisionHorizontal
    ////////////////////////////////////////////////

    namespace
    {

        struct FrameRef
        {
            int vision;               // 1 or 2
            FramePosition position;   // Top/Bottom/Left/Right
        };

        using FrameRefPair = std::array<std::optional<FrameRef>, 2>;

        // Map dual-horizontal positions to 1 or 2 underlying frame references.
        // MeetingRail maps to BOTH: vision1.Right and vision2.Left.
        const std::map<DualHorizontalFramePosition, FrameRefPair> kDualHPosToFrames = {
          {DualHorizontalFramePosition::TopLeft,
           FrameRefPair{FrameRef{1, FramePosition::Top}, std::nullopt}},
          {DualHorizontalFramePosition::TopRight,
           FrameRefPair{FrameRef{2, FramePosition::Top}, std::nullopt}},
          {DualHorizontalFramePosition::BottomLeft,
           FrameRefPair{FrameRef{1, FramePosition::Bottom}, std::nullopt}},
          {DualHorizontalFramePosition::BottomRight,
           FrameRefPair{FrameRef{2, FramePosition::Bottom}, std::nullopt}},
          {DualHorizontalFramePosition::Left,
           FrameRefPair{FrameRef{1, FramePosition::Left}, std::nullopt}},
          {DualHorizontalFramePosition::Right,
           FrameRefPair{FrameRef{2, FramePosition::Right}, std::nullopt}},
          {DualHorizontalFramePosition::MeetingRail,
           FrameRefPair{FrameRef{1, FramePosition::Right}, FrameRef{2, FramePosition::Left}}},
        };

        // Pull the concrete Frame& from a FrameRef
        const Frame & getFrameFromRef(const DualVisionHorizontal & self, const FrameRef & ref)
        {
            if(ref.vision == 1)
            {
                return self.vision1().frame(ref.position);
            }
            if(ref.vision == 2)
            {
                return self.vision2().frame(ref.position);
            }
            throw std::invalid_argument("Invalid vision index in FrameRef (expected 1 or 2).");
        }

        // Generic accumulator for a frame metric (e.g., frameArea or edgeOfGlassArea)
        template<typename MetricFn>
        double accumulateFor(const DualVisionHorizontal & self,
                             DualHorizontalFramePosition where,
                             MetricFn && metric)
        {
            const auto it = kDualHPosToFrames.find(where);
            if(it == kDualHPosToFrames.end())
            {
                throw std::invalid_argument("Invalid dual-horizontal frame position");
            }

            double sum = 0.0;
            for(const auto & maybeRef : it->second)
            {
                if(!maybeRef)
                {
                    continue;
                }
                const Frame & frame = getFrameFromRef(self, *maybeRef);
                sum += metric(frame);
            }
            return sum;
        }

        inline constexpr std::array kAllDualHPositions{DualHorizontalFramePosition::TopLeft,
                                                       DualHorizontalFramePosition::TopRight,
                                                       DualHorizontalFramePosition::BottomLeft,
                                                       DualHorizontalFramePosition::BottomRight,
                                                       DualHorizontalFramePosition::Left,
                                                       DualHorizontalFramePosition::Right,
                                                       DualHorizontalFramePosition::MeetingRail};
    }   // anonymous namespace


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

    std::map<DualHorizontalFramePosition, Frame> DualVisionHorizontal::frames() const
    {
        std::map<DualHorizontalFramePosition, Frame> out;

        for(const auto pos : kAllDualHPositions)
        {
            const auto it = kDualHPosToFrames.find(pos);
            if(it == kDualHPosToFrames.end())
            {
                continue;
            }

            const auto & refs = it->second;
            std::vector<Frame> found;

            for(const auto & maybeRef : refs)
            {
                if(!maybeRef)
                {
                    continue;
                }

                const Frame & f = getFrameFromRef(*this, *maybeRef);
                found.push_back(f);   // <-- push whole Frame, not FrameData
            }

            if(found.empty())
            {
                continue;
            }

            if(found.size() == 1)
            {
                out.emplace(pos, found[0]);
            }
            else
            {
                const size_t edgeMultiplier =
                  (pos == DualHorizontalFramePosition::MeetingRail ? 2 : 1);

                out.emplace(pos, mergeFrameWidths(found[0], found[1], edgeMultiplier));
            }
        }

        return out;
    }

    double DualVisionHorizontal::getFrameArea(const DualHorizontalFramePosition position) const
    {
        return accumulateFor(*this, position, [](const Frame & f) { return frameArea(f); });
    }

    double DualVisionHorizontal::getFrameEdgeOfGlassArea(DualHorizontalFramePosition position) const
    {
        return accumulateFor(*this, position, [](const Frame & f) { return edgeOfGlassArea(f); });
    }

    double DualVisionHorizontal::getFrameArea() const
    {
        double total = 0.0;
        for(const auto p : kAllDualHPositions)
        {
            total += getFrameArea(p);
        }
        return total;
    }

    double DualVisionHorizontal::getFrameEdgeOfGlassArea() const
    {
        double total = 0.0;
        for(const auto p : kAllDualHPositions)
        {
            total += getFrameEdgeOfGlassArea(p);
        }
        return total;
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

    namespace
    {
        // --- Vertical helpers (distinct names to avoid collisions) ---------------
        struct FrameRefV
        {
            int vision;               // 1 => top vision, 2 => bottom vision
            FramePosition position;   // Top/Bottom/Left/Right
        };

        using FrameRefPairV = std::array<std::optional<FrameRefV>, 2>;

        // Vision1 = TOP vision, Vision2 = BOTTOM vision
        // MeetingRail = Vision1.Bottom + Vision2.Top
        const std::map<DualVerticalFramePosition, FrameRefPairV> kDualVPosToFrames = {
          {DualVerticalFramePosition::Top,
           FrameRefPairV{FrameRefV{1, FramePosition::Top}, std::nullopt}},
          {DualVerticalFramePosition::Bottom,
           FrameRefPairV{FrameRefV{2, FramePosition::Bottom}, std::nullopt}},
          {DualVerticalFramePosition::TopLeft,
           FrameRefPairV{FrameRefV{1, FramePosition::Left}, std::nullopt}},
          {DualVerticalFramePosition::TopRight,
           FrameRefPairV{FrameRefV{1, FramePosition::Right}, std::nullopt}},
          {DualVerticalFramePosition::BottomLeft,
           FrameRefPairV{FrameRefV{2, FramePosition::Left}, std::nullopt}},
          {DualVerticalFramePosition::BottomRight,
           FrameRefPairV{FrameRefV{2, FramePosition::Right}, std::nullopt}},
          {DualVerticalFramePosition::MeetingRail,
           FrameRefPairV{FrameRefV{1, FramePosition::Bottom}, FrameRefV{2, FramePosition::Top}}},
        };

        inline const Frame & getFrameFromRefV(const DualVisionVertical & self,
                                              const FrameRefV & ref)
        {
            if(ref.vision == 1)
            {
                return self.vision1().frame(ref.position);
            }
            if(ref.vision == 2)
            {
                return self.vision2().frame(ref.position);
            }
            throw std::invalid_argument("Invalid vision index in FrameRefV (expected 1 or 2).");
        }

        template<typename MetricFn>
        double accumulateForV(const DualVisionVertical & self,
                              DualVerticalFramePosition where,
                              MetricFn && metric)
        {
            const auto it = kDualVPosToFrames.find(where);
            if(it == kDualVPosToFrames.end())
            {
                throw std::invalid_argument("Invalid dual-vertical frame position");
            }

            double sum = 0.0;
            for(const auto & maybeRef : it->second)
            {
                if(!maybeRef)
                {
                    continue;
                }
                const Frame & frame = getFrameFromRefV(self, *maybeRef);
                sum += metric(frame);
            }
            return sum;
        }

        inline constexpr std::array kAllDualVPositions{DualVerticalFramePosition::Top,
                                                       DualVerticalFramePosition::Bottom,
                                                       DualVerticalFramePosition::TopLeft,
                                                       DualVerticalFramePosition::TopRight,
                                                       DualVerticalFramePosition::BottomLeft,
                                                       DualVerticalFramePosition::BottomRight,
                                                       DualVerticalFramePosition::MeetingRail};
    }   // anonymous namespace


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

    std::map<DualVerticalFramePosition, Frame> DualVisionVertical::frames() const
    {
        std::map<DualVerticalFramePosition, Frame> out;

        for(const auto pos : kAllDualVPositions)
        {
            const auto it = kDualVPosToFrames.find(pos);
            if(it == kDualVPosToFrames.end())
            {
                continue;
            }

            const auto & refs = it->second;
            std::vector<Frame> found;

            for(const auto & maybeRef : refs)
            {
                if(!maybeRef)
                {
                    continue;
                }

                const Frame & f = getFrameFromRefV(*this, *maybeRef);
                found.push_back(f);   // <-- Use full Frame, not FrameData
            }

            if(found.empty())
            {
                continue;
            }

            if(found.size() == 1)
            {
                out.emplace(pos, found[0]);
            }
            else
            {
                const size_t edgeMultiplier =
                  (pos == DualVerticalFramePosition::MeetingRail ? 2 : 1);

                out.emplace(pos, mergeFrameWidths(found[0], found[1], edgeMultiplier));
            }
        }

        return out;
    }


    double DualVisionVertical::getFrameArea(const DualVerticalFramePosition position) const
    {
        return accumulateForV(*this, position, [](const Frame & f) { return frameArea(f); });
    }

    double DualVisionVertical::getFrameEdgeOfGlassArea(DualVerticalFramePosition position) const
    {
        return accumulateForV(*this, position, [](const Frame & f) { return edgeOfGlassArea(f); });
    }

    double DualVisionVertical::getFrameArea() const
    {
        double total = 0.0;
        for(const auto p : kAllDualVPositions)
        {
            total += getFrameArea(p);
        }
        return total;
    }

    double DualVisionVertical::getFrameEdgeOfGlassArea() const
    {
        double total = 0.0;
        for(const auto p : kAllDualVPositions)
        {
            total += getFrameEdgeOfGlassArea(p);
        }
        return total;
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
