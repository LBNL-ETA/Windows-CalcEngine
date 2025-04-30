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
           && m_Frame.at(FrameSide::Right)->frameType() == FrameType::Exterior)
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
            area -= m_FrameData.WettedLength
                    * m_Frame.at(FrameSide::Left)->projectedFrameDimension() * scaleFactor;
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

    const FrameData & Frame::frameData() const
    {
        return m_FrameData;
    }

    double Frame::edgeOfGlassArea() const
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

        auto area{length * ConstantsData::EOGHeight};

        if(m_Frame.count(FrameSide::Left) && m_Frame.at(FrameSide::Left).has_value()
           && m_Frame.at(FrameSide::Left)->frameType() == FrameType::Exterior
           && m_FrameType == FrameType::Exterior)
        {
            area -= ConstantsData::EOGHeight * ConstantsData::EOGHeight / 2;
        }

        if(m_Frame.count(FrameSide::Right) && m_Frame.at(FrameSide::Right).has_value()
           && m_Frame.at(FrameSide::Right)->frameType() == FrameType::Exterior
           && m_FrameType == FrameType::Exterior)
        {
            area -= ConstantsData::EOGHeight * ConstantsData::EOGHeight / 2;
        }

        area -= m_DividerArea * m_NumberOfDividers;

        return area;
    }

    double Frame::projectedFrameDimension() const
    {
        return m_FrameData.ProjectedFrameDimension;
    }

    void Frame::assignDividerArea(double area, size_t nDividers)
    {
        m_DividerArea = area;
        m_NumberOfDividers = nDividers;
    }

    void Frame::setFrameType(const FrameType type)
    {
        m_FrameType = type;
    }
}   // namespace Tarcog::ISO15099
