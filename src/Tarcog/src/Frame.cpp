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

    double shgc(const FrameData & frame, double hc)
    {
        if(hc == 0 || frame.WettedLength == 0)
        {
            return 0;
        }
        return frame.Absorptance * frame.UValue / hc * frame.ProjectedFrameDimension
               / frame.WettedLength;
    }

    Frame::Frame(double length, FrameType frameType, FrameData frameData) :
        m_Length(length), m_FrameType(frameType), m_FrameData(frameData)
    {}

    [[nodiscard]] double projectedArea(const Frame & frame)
    {
        double area = frame.m_Length * frame.m_FrameData.ProjectedFrameDimension;

        const double scaleFactor = frame.m_FrameType == FrameType::Interior ? 1.0 : 0.5;

        const auto subtractSideArea = [&](FrameSide side) {
            const auto it = frame.m_Frame.find(side);
            if(it != frame.m_Frame.end() && it->second.has_value())
            {
                const Frame & neighbor = it->second.value();
                if(neighbor.m_FrameType == FrameType::Exterior)
                {
                    area -= frame.m_FrameData.ProjectedFrameDimension
                            * neighbor.m_FrameData.ProjectedFrameDimension * scaleFactor;
                }
            }
        };

        subtractSideArea(FrameSide::Left);
        subtractSideArea(FrameSide::Right);

        return area;
    }

    [[nodiscard]] double wettedArea(const Frame & frame)
    {
        double area = frame.m_Length * frame.m_FrameData.WettedLength;

        const double scaleFactor = frame.m_FrameType == FrameType::Interior ? 1.0 : 0.5;

        const auto subtractSideArea = [&](FrameSide side) {
            const auto it = frame.m_Frame.find(side);
            if(it != frame.m_Frame.end() && it->second.has_value())
            {
                const Frame & neighbor = it->second.value();
                if(neighbor.m_FrameType == FrameType::Exterior)
                {
                    area -= frame.m_FrameData.WettedLength
                            * neighbor.m_FrameData.ProjectedFrameDimension * scaleFactor;
                }
            }
        };

        subtractSideArea(FrameSide::Left);
        subtractSideArea(FrameSide::Right);

        return area;
    }

    [[nodiscard]] double edgeOfGlassArea(const Frame& frame)
    {
        double length = frame.m_Length;

        const auto adjustLengthForSide = [&](FrameSide side) {
            const auto it = frame.m_Frame.find(side);
            if(it != frame.m_Frame.end() && it->second.has_value())
            {
                const Frame& neighbor = it->second.value();
                length -= neighbor.m_FrameData.ProjectedFrameDimension;
                if(frame.m_FrameType == FrameType::Interior)
                {
                    length -= ConstantsData::EOGHeight;
                }
            }
        };

        adjustLengthForSide(FrameSide::Left);
        adjustLengthForSide(FrameSide::Right);

        double area = length * ConstantsData::EOGHeight;

        const auto subtractCornerTriangle = [&](FrameSide side) {
            const auto it = frame.m_Frame.find(side);
            if(it != frame.m_Frame.end() && it->second.has_value())
            {
                const Frame& neighbor = it->second.value();
                if(neighbor.m_FrameType == FrameType::Exterior && frame.m_FrameType == FrameType::Exterior)
                {
                    area -= (ConstantsData::EOGHeight * ConstantsData::EOGHeight) / 2.0;
                }
            }
        };

        subtractCornerTriangle(FrameSide::Left);
        subtractCornerTriangle(FrameSide::Right);

        area -= frame.m_DividerArea * static_cast<double>(frame.m_NumberOfDividers);

        return area;
    }

}   // namespace Tarcog::ISO15099
