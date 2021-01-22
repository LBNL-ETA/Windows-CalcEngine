#include <map>
#include "Frame.hpp"

namespace Tarcog::ISO15099
{
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

    Frame::Frame(double length, FrameGeometryType frameGeometryType, FrameData frameData) :
        m_Length(length), m_GeometryType(frameGeometryType), m_FrameData(frameData)
    {}

    double Frame::area() const
    {
        static std::map<FrameGeometryType, double> subtraction{
          {FrameGeometryType::Square, 0},
          {FrameGeometryType::Trapezoid,
           m_FrameData.ProjectedFrameDimension * m_FrameData.ProjectedFrameDimension},
          {FrameGeometryType::Trapezoid,
           m_FrameData.ProjectedFrameDimension * m_FrameData.ProjectedFrameDimension / 2}};
        return m_Length * m_FrameData.WettedLength - subtraction.at(m_GeometryType);
    }
}   // namespace Tarcog::ISO15099
