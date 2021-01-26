#include <map>
#include <WCECommon.hpp>
#include "Frame.hpp"

namespace Tarcog::ISO15099
{
    FrameData::FrameData() :
        UValue(0), EdgeUValue(0), ProjectedFrameDimension(0), WettedLength(0), Absorptance(0)
    {}

    FrameData::FrameData(double uValue,
                         double edgeUValue,
                         double projectedFrameDimension,
                         double wettedLength,
                         double absorptance) :
        UValue(uValue),
        EdgeUValue(edgeUValue),
        ProjectedFrameDimension(projectedFrameDimension),
        WettedLength(wettedLength),
        Absorptance(absorptance)
    {}

    Frame::Frame(double length, FrameType frameType, FrameData frameData) :
        m_Length(length), m_FrameType(frameType), m_FrameData(frameData)
    {}

    FrameType Frame::frameType() const
    {
        return m_FrameType;
    }

    double Frame::projectedArea() const
    {
        auto area{m_Length * m_FrameData.ProjectedFrameDimension};

        const auto scaleFactor{m_FrameType == FrameType::Interior ? 1.0 : 0.5};

        if(m_Frame.count(FrameSide::Left) && m_Frame.at(FrameSide::Left).has_value()
           && m_Frame.at(FrameSide::Left)->frameType() == FrameType::Exterior)
        {
            area -= m_FrameData.ProjectedFrameDimension
                    * m_Frame.at(FrameSide::Left)->projectedFrameDimension() * scaleFactor;
        }

        if(m_Frame.count(FrameSide::Right) && m_Frame.at(FrameSide::Right).has_value()
           && m_Frame.at(FrameSide::Left)->frameType() == FrameType::Exterior)
        {
            area -= m_FrameData.ProjectedFrameDimension
                    * m_Frame.at(FrameSide::Right)->projectedFrameDimension() * scaleFactor;
        }

        return area;
    }

    double Frame::wettedArea() const
    {
        auto area{m_Length * m_FrameData.WettedLength};

        const auto scaleFactor{m_FrameType == FrameType::Interior ? 1.0 : 0.5};

        if(m_Frame.count(FrameSide::Left) && m_Frame.at(FrameSide::Left).has_value()
           && m_Frame.at(FrameSide::Left)->frameType() == FrameType::Exterior)
        {
            area -=
              m_FrameData.WettedLength * m_Frame.at(FrameSide::Left)->projectedFrameDimension() * scaleFactor;
        }

        if(m_Frame.count(FrameSide::Right) && m_Frame.at(FrameSide::Right).has_value()
           && m_Frame.at(FrameSide::Left)->frameType() == FrameType::Exterior)
        {
            area -= m_FrameData.WettedLength
                    * m_Frame.at(FrameSide::Right)->projectedFrameDimension() * scaleFactor;
        }

        return area;
    }

    void Frame::setFrameData(FrameData frameData)
    {
        m_FrameData = frameData;
    }

    double Frame::edgeOfAreaLength() const
    {
        auto length{m_Length};

        if(m_Frame.count(FrameSide::Left) && m_Frame.at(FrameSide::Left).has_value())
        {
            length -= m_Frame.at(FrameSide::Left)->projectedFrameDimension();
            if(m_FrameType == FrameType::Interior)
            {
                length -= ConstantsData::EOGHeight;
            }
        }
        if(m_Frame.count(FrameSide::Right) && m_Frame.at(FrameSide::Right).has_value())
        {
            length -= m_Frame.at(FrameSide::Right)->projectedFrameDimension();
            if(m_FrameType == FrameType::Interior)
            {
                length -= ConstantsData::EOGHeight;
            }
        }

        return length;
    }

    double Frame::projectedFrameDimension() const
    {
        return m_FrameData.ProjectedFrameDimension;
    }

    void Frame::assignFrame(Frame frame, FrameSide side)
    {
        m_Frame[side] = std::move(frame);
    }
}   // namespace Tarcog::ISO15099
