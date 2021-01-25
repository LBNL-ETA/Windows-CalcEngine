#include <map>
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

        if(m_Frame.count(FrameSide::Left) && m_Frame.at(FrameSide::Left).has_value()
           && m_Frame.at(FrameSide::Left)->frameType() == FrameType::Exterior)
        {
            area -= m_FrameData.ProjectedFrameDimension
                    * m_Frame.at(FrameSide::Left)->projectedFrameDimension() / 2;
        }

        if(m_Frame.count(FrameSide::Right) && m_Frame.at(FrameSide::Right).has_value()
           && m_Frame.at(FrameSide::Left)->frameType() == FrameType::Exterior)
        {
            area -= m_FrameData.ProjectedFrameDimension
                    * m_Frame.at(FrameSide::Right)->projectedFrameDimension() / 2;
        }

        return area;
    }

    double Frame::wettedArea() const
    {
        auto area{m_Length * m_FrameData.WettedLength};

        if(m_Frame.count(FrameSide::Left) && m_Frame.at(FrameSide::Left).has_value()
           && m_Frame.at(FrameSide::Left)->frameType() == FrameType::Exterior)
        {
            area -=
              m_FrameData.WettedLength * m_Frame.at(FrameSide::Left)->projectedFrameDimension() / 2;
        }

        if(m_Frame.count(FrameSide::Right) && m_Frame.at(FrameSide::Right).has_value()
           && m_Frame.at(FrameSide::Left)->frameType() == FrameType::Exterior)
        {
            area -= m_FrameData.WettedLength
                    * m_Frame.at(FrameSide::Right)->projectedFrameDimension() / 2;
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
        }
        if(m_Frame.count(FrameSide::Right) && m_Frame.at(FrameSide::Right).has_value())
        {
            length -= m_Frame.at(FrameSide::Right)->projectedFrameDimension();
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
