#include "WindowVision.hpp"

#include "EnvironmentConfigurations.hpp"

namespace Tarcog::ISO15099
{    
    WindowVision::WindowVision(
      double width, double height, double tvis, double tsol, const IIGUSystem & iguSystem) :
        m_Width(width),
        m_Height(height),
        m_Uvalue(iguSystem.getUValue()),
        m_SHGC(iguSystem.getSHGC(tsol)),
        m_VT(tvis),
        m_HcExterior(iguSystem.getHc(System::SHGC, Environment::Outdoor)),
        m_Frame({{FramePosition::Top, {width}},
                 {FramePosition::Bottom, {width}},
                 {FramePosition::Left, {height}},
                 {FramePosition::Right, {height}}})
    {}

    WindowVision::WindowVision(
      double width, double height, double Uvalue, double shgc, double vt, double hcExterior) :
        m_Width(width),
        m_Height(height),
        m_Uvalue(Uvalue),
        m_SHGC(shgc),
        m_VT(vt),
        m_HcExterior(hcExterior),
        m_Frame({{FramePosition::Top, {width}},
                 {FramePosition::Bottom, {width}},
                 {FramePosition::Left, {height}},
                 {FramePosition::Right, {height}}})
    {}

    double WindowVision::uValue() const
    {
        return m_Uvalue;
    }

    double WindowVision::shgc() const
    {
        return m_SHGC;
    }

    double WindowVision::area() const
    {
        return m_Width * m_Height;
    }

    double WindowVision::vt() const
    {
        return m_VT;
    }

    double WindowVision::hc() const
    {
        return m_HcExterior;
    }

    void WindowVision::setFrameData(FramePosition position, FrameData frameData)
    {
        m_Frame.at(position).setFrameData(frameData);
    }
}   // namespace Tarcog::ISO15099
