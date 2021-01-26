#include "WindowVision.hpp"

#include "EnvironmentConfigurations.hpp"

namespace Tarcog::ISO15099
{    
    WindowVision::WindowVision(
      double width, double height, double tvis, double tsol, const IIGUSystem & iguSystem) :
        m_Width(width),
        m_Height(height),
        m_IGUUvalue(iguSystem.getUValue()),
        m_IGUSHGC(iguSystem.getSHGC(tsol)),
        m_VT(tvis),
        m_HcExterior(iguSystem.getHc(System::SHGC, Environment::Outdoor)),
        m_Frame({{FramePosition::Top, {width}},
                 {FramePosition::Bottom, {width}},
                 {FramePosition::Left, {height}},
                 {FramePosition::Right, {height}}})
    {}

    double WindowVision::uValue() const
    {
        auto frameWeightedUValue{0.0};
        auto edgeOfGlassWeightedUValue{0.0};

        for(const auto & frame : m_Frame)
        {
            frameWeightedUValue += frame.second.projectedArea() * frame.second.frameData().UValue;
            edgeOfGlassWeightedUValue += frame.second.edgeOfGlassArea() * frame.second.frameData().EdgeUValue;
        }

        auto COGWeightedUValue{m_IGUUvalue * (area() - frameProjectedArea() - edgeOfGlassArea())};

        return (COGWeightedUValue + frameWeightedUValue + edgeOfGlassWeightedUValue) / area();
    }

    double WindowVision::shgc() const
    {
        return m_IGUSHGC;
    }

    double WindowVision::area() const
    {
        return m_Width * m_Height;
    }

    double WindowVision::vt() const
    {
        return (area() - frameProjectedArea()) / area() * m_VT;
    }

    double WindowVision::hc() const
    {
        return m_HcExterior;
    }

    void WindowVision::setFrameData(FramePosition position, FrameData frameData)
    {
        m_Frame.at(position).setFrameData(frameData);

        connectFrames();
    }

    void WindowVision::connectFrames()
    {
        m_Frame.at(FramePosition::Top).assignFrame(m_Frame.at(FramePosition::Right), FrameSide::Left);
        m_Frame.at(FramePosition::Top).assignFrame(m_Frame.at(FramePosition::Left), FrameSide::Right);

        m_Frame.at(FramePosition::Bottom).assignFrame(m_Frame.at(FramePosition::Right), FrameSide::Right);
        m_Frame.at(FramePosition::Bottom).assignFrame(m_Frame.at(FramePosition::Left), FrameSide::Left);

        m_Frame.at(FramePosition::Left).assignFrame(m_Frame.at(FramePosition::Top), FrameSide::Left);
        m_Frame.at(FramePosition::Left).assignFrame(m_Frame.at(FramePosition::Bottom), FrameSide::Right);

        m_Frame.at(FramePosition::Right).assignFrame(m_Frame.at(FramePosition::Bottom), FrameSide::Left);
        m_Frame.at(FramePosition::Right).assignFrame(m_Frame.at(FramePosition::Top), FrameSide::Right);
    }

    double WindowVision::frameProjectedArea() const
    {
        auto area{0.0};

        for(const auto & frame : m_Frame)
        {
            area += frame.second.projectedArea();
        }

        return area;
    }

    double WindowVision::edgeOfGlassArea() const
    {
        auto area{0.0};

        for(const auto & frame : m_Frame)
        {
            area += frame.second.edgeOfGlassArea();
        }

        return area;
    }
}   // namespace Tarcog::ISO15099
