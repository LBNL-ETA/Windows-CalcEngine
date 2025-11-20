#include <map>

#include <WCECommon.hpp>

#include "Frame.hpp"

namespace Tarcog::ISO15099
{
    FrameData splitFrameWidth(const FrameData & frame)
    {
        auto result{frame};
        result.ProjectedFrameDimension = result.ProjectedFrameDimension / 2;
        result.WettedLength = result.WettedLength / 2;

        return result;
    }

    Frame mergeFrameWidths(const Frame & a, const Frame & b, const size_t edgeMultiplier)
    {
        Frame out = a;
        out.frameData.ProjectedFrameDimension =
          a.frameData.ProjectedFrameDimension + b.frameData.ProjectedFrameDimension;
        out.frameData.WettedLength = out.frameData.WettedLength + b.frameData.WettedLength;
        out.edgeMultiplier = edgeMultiplier;
        return out;
    }

    double frameSHGC(double absorptance,
                     double uValue,
                     double projectedFrameDimension,
                     double wettedLength,
                     double hc)
    {
        if(hc == 0 || wettedLength == 0)
        {
            return 0;
        }
        return absorptance * uValue / hc * projectedFrameDimension / wettedLength;
    }

    [[nodiscard]] double frameArea(const Frame & frame)
    {
        double area = frame.length * frame.frameData.ProjectedFrameDimension;

        const double scaleFactor = frame.frameType == FrameType::Interior ? 1.0 : 0.5;

        const auto subtractSideArea = [&](const FrameSide side) {
            const auto & maybeNeighbor =
              frame.frame.contains(side) ? frame.frame.at(side) : std::nullopt;

            if(!maybeNeighbor || maybeNeighbor->get().frameType != FrameType::Exterior)
            {
                return;
            }

            const auto & neighbor = maybeNeighbor->get();
            area -= frame.frameData.ProjectedFrameDimension
                    * neighbor.frameData.ProjectedFrameDimension * scaleFactor;
        };

        subtractSideArea(FrameSide::Left);
        subtractSideArea(FrameSide::Right);

        return area;
    }

    [[nodiscard]] double wettedArea(const Frame & frame)
    {
        double area = frame.length * frame.frameData.WettedLength;

        const double scaleFactor = frame.frameType == FrameType::Interior ? 1.0 : 0.5;

        const auto subtractSideArea = [&](FrameSide side) {
            const auto & maybeNeighbor =
              frame.frame.contains(side) ? frame.frame.at(side) : std::nullopt;

            if(!maybeNeighbor || maybeNeighbor->get().frameType != FrameType::Exterior)
            {
                return;
            }

            const auto & neighbor = maybeNeighbor->get();
            area -= frame.frameData.WettedLength * neighbor.frameData.ProjectedFrameDimension
                    * scaleFactor;
        };

        subtractSideArea(FrameSide::Left);
        subtractSideArea(FrameSide::Right);

        return area;
    }

    [[nodiscard]] double edgeOfGlassArea(const Frame & frame)
    {
        double length = frame.length;

        const auto adjustLengthForSide = [&](FrameSide side) {
            const auto & maybeNeighbor =
              frame.frame.contains(side) ? frame.frame.at(side) : std::nullopt;

            if(!maybeNeighbor)
            {
                return;
            }

            const auto & neighbor = maybeNeighbor->get();
            length -= neighbor.frameData.ProjectedFrameDimension;

            if(frame.frameType == FrameType::Interior)
            {
                length -= ConstantsData::EOGHeight;
            }
        };

        adjustLengthForSide(FrameSide::Left);
        adjustLengthForSide(FrameSide::Right);

        double area = length * ConstantsData::EOGHeight;

        const auto subtractCornerTriangle = [&](FrameSide side) {
            const auto it = frame.frame.find(side);
            if(it != frame.frame.end() && it->second.has_value())
            {
                const Frame & neighbor = it->second.value();
                if(neighbor.frameType == FrameType::Exterior
                   && frame.frameType == FrameType::Exterior)
                {
                    area -= (ConstantsData::EOGHeight * ConstantsData::EOGHeight) / 2.0;
                }
            }
        };

        subtractCornerTriangle(FrameSide::Left);
        subtractCornerTriangle(FrameSide::Right);

        area -= frame.dividerArea * static_cast<double>(frame.numberOfDividers);

        return area * frame.edgeMultiplier;
    }

}   // namespace Tarcog::ISO15099
