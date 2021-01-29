#include "WindowVision.hpp"

#include "EnvironmentConfigurations.hpp"

namespace Tarcog::ISO15099
{
    WindowVision::WindowVision(double width,
                               double height,
                               double tvis,
                               double tsol,
                               std::shared_ptr<IIGUSystem> iguSystem) :
        m_IGUSystem(std::move(iguSystem)),
        m_Width(width),
        m_Height(height),
        m_VT(tvis),
        m_Tsol(tsol),
        m_Frame({{FramePosition::Top, {width}},
                 {FramePosition::Bottom, {width}},
                 {FramePosition::Left, {height}},
                 {FramePosition::Right, {height}}})
    {
        m_IGUSystem->setWidthAndHeight(width, height);
        m_IGUUvalue = m_IGUSystem->getUValue();
        m_IGUSHGC = m_IGUSystem->getSHGC(m_Tsol);
        m_HcExterior = m_IGUSystem->getHc(System::SHGC, Environment::Outdoor);
    }

    double WindowVision::uValue() const
    {
        auto frameWeightedUValue{0.0};
        auto edgeOfGlassWeightedUValue{0.0};

        for(const auto & [key, frame] : m_Frame)
        {
            frameWeightedUValue += frame.projectedArea() * frame.frameData().UValue;
            edgeOfGlassWeightedUValue += frame.edgeOfGlassArea() * frame.frameData().EdgeUValue;
        }

        const auto COGWeightedUValue{m_IGUUvalue
                                     * (area() - frameProjectedArea() - edgeOfGlassArea())};

        return (COGWeightedUValue + frameWeightedUValue + edgeOfGlassWeightedUValue) / area();
    }

    double WindowVision::shgc() const
    {
        auto frameWeightedSHGC{0.0};

        for(const auto & [key, frame] : m_Frame)
        {
            frameWeightedSHGC += frame.projectedArea() * frame.frameData().shgc(m_HcExterior);
        }

        const auto COGWeightedSHGC{m_IGUSHGC * (area() - frameProjectedArea())};

        return (COGWeightedSHGC + frameWeightedSHGC) / area();
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

    void WindowVision::setHc(double hc)
    {
        m_HcExterior = hc;
    }

    void WindowVision::setFrameData(FramePosition position, FrameData frameData)
    {
        m_Frame.at(position).setFrameData(frameData);

        connectFrames();
        resizeIGU();
    }

    void WindowVision::connectFrames()
    {
        m_Frame.at(FramePosition::Top)
          .assignFrame(m_Frame.at(FramePosition::Right), FrameSide::Left);
        m_Frame.at(FramePosition::Top)
          .assignFrame(m_Frame.at(FramePosition::Left), FrameSide::Right);

        m_Frame.at(FramePosition::Bottom)
          .assignFrame(m_Frame.at(FramePosition::Right), FrameSide::Right);
        m_Frame.at(FramePosition::Bottom)
          .assignFrame(m_Frame.at(FramePosition::Left), FrameSide::Left);

        m_Frame.at(FramePosition::Left)
          .assignFrame(m_Frame.at(FramePosition::Top), FrameSide::Left);
        m_Frame.at(FramePosition::Left)
          .assignFrame(m_Frame.at(FramePosition::Bottom), FrameSide::Right);

        m_Frame.at(FramePosition::Right)
          .assignFrame(m_Frame.at(FramePosition::Bottom), FrameSide::Left);
        m_Frame.at(FramePosition::Right)
          .assignFrame(m_Frame.at(FramePosition::Top), FrameSide::Right);
    }

    void WindowVision::resizeIGU()
    {
        const auto width{m_Width - m_Frame.at(FramePosition::Left).projectedFrameDimension()
                         - m_Frame.at(FramePosition::Right).projectedFrameDimension()};
        const auto height{m_Height - m_Frame.at(FramePosition::Top).projectedFrameDimension()
                          - m_Frame.at(FramePosition::Bottom).projectedFrameDimension()};
        m_IGUSystem->setWidthAndHeight(width, height);
        m_IGUSystem->setExteriorSurfaceHeight(m_Height);
        m_IGUUvalue = m_IGUSystem->getUValue();
        m_IGUSHGC = m_IGUSystem->getSHGC(m_Tsol);
        m_HcExterior = m_IGUSystem->getHc(System::SHGC, Environment::Outdoor);

    }

    double WindowVision::frameProjectedArea() const
    {
        auto area{0.0};

        for(const auto & [key, frame] : m_Frame)
        {
            area += frame.projectedArea();
        }

        return area;
    }

    double WindowVision::edgeOfGlassArea() const
    {
        auto area{0.0};

        for(const auto & [key, frame] : m_Frame)
        {
            area += frame.edgeOfGlassArea();
        }

        return area;
    }
}   // namespace Tarcog::ISO15099
