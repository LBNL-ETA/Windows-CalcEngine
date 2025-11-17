#include <ranges>
#include <numeric>

#include <WCECommon.hpp>

#include "WindowVision.hpp"
#include "EnvironmentConfigurations.hpp"
#include "IGU.hpp"

namespace Tarcog::ISO15099
{
    WindowVision::WindowVision(const WindowVision & other) :
        IVision(other),
        m_IGUSystem(other.m_IGUSystem),
        m_Width(other.m_Width),
        m_Height(other.m_Height),
        m_IGUUvalue(other.m_IGUUvalue),
        m_VT(other.m_VT),
        m_Tsol(other.m_Tsol),
        m_HExterior(other.m_HExterior),
        m_ExteriorSurfaceHeight(other.m_ExteriorSurfaceHeight),
        m_Frame(other.m_Frame),
        m_Divider(other.m_Divider),
        m_IGUErrorTolerance(other.m_IGUErrorTolerance)
    {
        connectFrames();
    }

    WindowVision & WindowVision::operator=(const WindowVision & other)
    {
        if(this != &other)
        {
            m_IGUSystem = other.m_IGUSystem;
            m_Width = other.m_Width;
            m_Height = other.m_Height;
            m_IGUUvalue = other.m_IGUUvalue;
            m_VT = other.m_VT;
            m_Tsol = other.m_Tsol;
            m_HExterior = other.m_HExterior;
            m_ExteriorSurfaceHeight = other.m_ExteriorSurfaceHeight;
            m_Frame = other.m_Frame;
            m_Divider = other.m_Divider;
            m_IGUErrorTolerance = other.m_IGUErrorTolerance;
            connectFrames();
        }
        return *this;
    }

    WindowVision::WindowVision(WindowVision && other) noexcept :
        IVision(std::move(other)),
        m_IGUSystem(std::move(other.m_IGUSystem)),
        m_Width(other.m_Width),
        m_Height(other.m_Height),
        m_IGUUvalue(other.m_IGUUvalue),
        m_VT(other.m_VT),
        m_Tsol(other.m_Tsol),
        m_HExterior(other.m_HExterior),
        m_ExteriorSurfaceHeight(other.m_ExteriorSurfaceHeight),
        m_Frame(std::move(other.m_Frame)),
        m_Divider(other.m_Divider),
        m_IGUErrorTolerance(other.m_IGUErrorTolerance)
    {
        connectFrames();
    }

    WindowVision & WindowVision::operator=(WindowVision && other) noexcept
    {
        if(this != &other)
        {
            IVision::operator=(std::move(other));
            m_IGUSystem = std::move(other.m_IGUSystem);
            m_Width = other.m_Width;
            m_Height = other.m_Height;
            m_IGUUvalue = other.m_IGUUvalue;
            m_VT = other.m_VT;
            m_Tsol = other.m_Tsol;
            m_HExterior = other.m_HExterior;
            m_ExteriorSurfaceHeight = other.m_ExteriorSurfaceHeight;
            m_Frame = std::move(other.m_Frame);
            m_Divider = other.m_Divider;
            m_IGUErrorTolerance = other.m_IGUErrorTolerance;
            connectFrames();
        }
        return *this;
    }

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
        m_ExteriorSurfaceHeight(height),
        m_Frame({{FramePosition::Top, {width}},
                 {FramePosition::Bottom, {width}},
                 {FramePosition::Left, {height}},
                 {FramePosition::Right, {height}}})
    {
        m_IGUSystem->setWidthAndHeight(width, height);
        m_IGUSystem->setInteriorAndExteriorSurfacesHeight(m_ExteriorSurfaceHeight);
        m_IGUUvalue = m_IGUSystem->getUValue();
        m_HExterior = m_IGUSystem->getH(System::SHGC, Environment::Outdoor);
    }

    namespace Helper
    {
        inline double cogWeightedUValue(double uCenter,
                                        double totalArea,
                                        double frameProjArea,
                                        double edgeArea,
                                        double dividerArea,
                                        double dividerEdgeArea)
        {
            return uCenter * (totalArea - frameProjArea - edgeArea - dividerArea - dividerEdgeArea);
        }

        inline double dividerWeightedUValue(double dividerArea, double dividerUValue)
        {
            return dividerArea * dividerUValue;
        }

        inline double dividerEdgeWeightedUValue(double dividerEdgeArea, double dividerEdgeUValue)
        {
            return dividerEdgeArea * dividerEdgeUValue;
        }

        inline double totalGapThickness(const IIGUSystem & igu)
        {
            const auto gapThicknesses{igu.gapLayerThicknesses()};
            return std::accumulate(gapThicknesses.begin(), gapThicknesses.end(), 0.0);
        }

        inline double frameEdgeUValue(IIGUSystem & igu, const FrameData & frameData)
        {
            if(std::holds_alternative<std::monostate>(frameData.Class))
            {
                return frameData.EdgeUValue;
            }

            return std::visit(
              [&]<typename T0>(const T0 & arg) -> double {
                  using T = std::decay_t<T0>;
                  if constexpr(std::is_same_v<T, GenericFrame>)
                  {
                      return ISO15099::frameEdgeUValue(
                        arg, igu.getUValue(), totalGapThickness(igu));
                  }
                  else if constexpr(std::is_same_v<T, GenericDivider>)
                  {
                      return ISO15099::dividerEdgeUValue(
                        arg.EdgePoly, igu.getUValue(), totalGapThickness(igu));
                  }
                  else
                  {
                      return frameData.EdgeUValue;
                  }
              },
              frameData.Class);
        }

        inline double dividerEdgeUValue(IIGUSystem & igu, const FrameData & dividerData)
        {
            if(std::holds_alternative<std::monostate>(dividerData.Class))
            {
                return dividerData.EdgeUValue;
            }
            return std::visit(
              [&]<typename T0>(const T0 & arg) -> double {
                  using T = std::decay_t<T0>;
                  if constexpr(std::is_same_v<T, GenericDivider>)
                  {
                      return ISO15099::dividerEdgeUValue(
                        arg.EdgePoly, igu.getUValue(), Helper::totalGapThickness(igu));
                  }
                  else
                  {
                      return dividerData.EdgeUValue;
                  }
              },
              dividerData.Class);
        }

        inline double dividerUValue(IIGUSystem & igu, const FrameData & dividerData)
        {
            if(std::holds_alternative<std::monostate>(dividerData.Class))
            {
                return dividerData.UValue;
            }
            return std::visit(
              [&]<typename T0>(const T0 & arg) -> double {
                  using T = std::decay_t<T0>;
                  if constexpr(std::is_same_v<T, GenericDivider>)
                  {
                      return ISO15099::dividerUValue(
                        arg.BodyPoly, igu.getUValue(), Helper::totalGapThickness(igu));
                  }
                  else
                  {
                      return dividerData.UValue;
                  }
              },
              dividerData.Class);
        }

    }   // namespace Helper


    double WindowVision::uValue() const
    {
        const auto frames = m_Frame | std::views::values;

        const double frameWU =
          std::accumulate(frames.begin(), frames.end(), 0.0, [](double acc, const auto & frame) {
              return acc + (frameArea(frame) * frame.frameData.UValue);
          });

        const double edgeWU = std::accumulate(
          frames.begin(), frames.end(), 0.0, [this](double acc, const auto & frame) {
              return acc
                     + (ISO15099::edgeOfGlassArea(frame)
                        * Helper::frameEdgeUValue(*m_IGUSystem, frame.frameData));
          });

        // COG weighted U-value
        const double cogWU = m_IGUUvalue
                             * (area() - frameProjectedArea() - totalEdgeOfGlassArea()
                                - dividerArea() - dividerEdgeOfGlassArea());

        // Divider contributions
        const double divWU =
          m_Divider ? dividerArea() * Helper::dividerUValue(*m_IGUSystem, m_Divider->frame) : 0.0;
        const double divEdgeWU =
          m_Divider
            ? dividerEdgeOfGlassArea() * Helper::dividerEdgeUValue(*m_IGUSystem, m_Divider->frame)
            : 0.0;

        return (cogWU + frameWU + edgeWU + divWU + divEdgeWU) / area();
    }

    double WindowVision::shgc() const
    {
        return shgc(m_Tsol);
    }

    double WindowVision::shgc(const double tSol) const
    {
        const double cogWeighted =
          m_IGUSystem->getSHGC(tSol) * (area() - frameProjectedArea() - dividerArea());

        return shgcCommon(cogWeighted);
    }

    double WindowVision::shgc0() const
    {
        constexpr double cogWeighted = 0.0;

        return shgcCommon(cogWeighted);
    }

    double WindowVision::shgc1() const
    {
        const double cogWeighted = area() - frameProjectedArea() - dividerArea();

        return shgcCommon(cogWeighted);
    }

    double WindowVision::area() const
    {
        return m_Width * m_Height;
    }

    double WindowVision::vt() const
    {
        return vt(m_VT);
    }

    double WindowVision::vt(const double tVis) const
    {
        return visionPercentage() * tVis;
    }

    double WindowVision::vt0() const
    {
        return vt(0);
    }

    double WindowVision::vt1() const
    {
        return vt(1);
    }

    double WindowVision::visionPercentage() const
    {
        return (area() - frameProjectedArea() - dividerArea()) / area();
    }

    double WindowVision::hc() const
    {
        return m_HExterior;
    }

    double WindowVision::uValueCOG() const
    {
        return m_IGUUvalue;
    }

    double WindowVision::shgcCOG() const
    {
        return m_IGUSystem->getSHGC(m_Tsol);
    }

    void WindowVision::setHc(double hc)
    {
        m_HExterior = hc;
    }

    void WindowVision::setFrameData(FramePosition position, FrameData frameData)
    {
        m_Frame.at(position).frameData = frameData;

        connectFrames();
        resizeIGU();
    }

    void WindowVision::setFrameTypes(const std::map<FramePosition, FrameType> & frameTypes)
    {
        for(const auto & [position, type] : frameTypes)
        {
            if(m_Frame.contains(position))
            {
                m_Frame.at(position).frameType = type;
            }
        }

        connectFrames();
    }

    const Frame & WindowVision::frame(const FramePosition position) const
    {
        return m_Frame.at(position);
    }

    void WindowVision::setDividers(const FrameData & divider, size_t nHorizontal, size_t nVertical)
    {
        if(!m_Divider.has_value())
        {
            m_Divider.emplace();
        }

        m_Divider->frame = divider;
        m_Divider->numberOfHorizontal = nHorizontal;
        m_Divider->numberOfVertical = nVertical;

        // TODO: Do not cache precalculated divider areas, calculate on demand. Try to refactor this
        std::map<FramePosition, size_t> numOfDivs{
          {FramePosition::Top, m_Divider->numberOfVertical},
          {FramePosition::Bottom, m_Divider->numberOfVertical},
          {FramePosition::Left, m_Divider->numberOfHorizontal},
          {FramePosition::Right, m_Divider->numberOfHorizontal}};

        for(auto & [key, frame] : m_Frame)
        {
            frame.dividerArea = m_Divider->frame.ProjectedFrameDimension * ConstantsData::EOGHeight;
            frame.numberOfDividers = numOfDivs.at(key);
        }
    }

    namespace Helper
    {
        inline std::pair<size_t, size_t> autoCalculateDividers(const double width,
                                                               const double height)
        {
            const int nHor = static_cast<int>(width / 0.305);
            const int nVer = static_cast<int>(height / 0.305);
            return {static_cast<size_t>(std::max(0, nHor)), static_cast<size_t>(std::max(0, nVer))};
        }
    }   // namespace Helper


    void WindowVision::setDividersAuto(const FrameData & divider)
    {
        auto [nHor, nVer] = Helper::autoCalculateDividers(m_Width, m_Height);
        setDividers(divider, nHor, nVer);
    }

    void WindowVision::setInteriorAndExteriorSurfaceHeight(const double height)
    {
        m_ExteriorSurfaceHeight = height;

        m_IGUSystem->setInteriorAndExteriorSurfacesHeight(m_ExteriorSurfaceHeight);

        m_IGUUvalue = m_IGUSystem->getUValue();
        m_HExterior = m_IGUSystem->getH(System::SHGC, Environment::Outdoor);
    }

    void WindowVision::setUValueIGUTolerance(double uValue)
    {
        m_IGUErrorTolerance.UCenter = uValue;
    }

    void WindowVision::setThicknessIGUTolerance(double thickness)
    {
        m_IGUErrorTolerance.Thickness = thickness;
    }

    double WindowVision::getIGUWidth() const
    {
        return m_Width - m_Frame.at(FramePosition::Left).frameData.ProjectedFrameDimension
               - m_Frame.at(FramePosition::Right).frameData.ProjectedFrameDimension;
    }

    double WindowVision::getIGUHeight() const
    {
        return m_Height - m_Frame.at(FramePosition::Top).frameData.ProjectedFrameDimension
               - m_Frame.at(FramePosition::Bottom).frameData.ProjectedFrameDimension;
    }

    namespace
    {
        struct Tol
        {
            double abs{0.0};   // absolute tolerance
            double rel{0.0};   // fractional tolerance (0.0 means unused)
        };

        [[nodiscard]] inline bool nearlyEqual(const double a, const double b, const Tol t)
        {
            const double scale = std::max(std::abs(a), std::abs(b));
            return std::abs(a - b) <= t.abs + t.rel * scale;
        }

        IGUMismatch checkFrameMismatch(const IGUData & frameData,
                                       const IGUData & cogData,
                                       const IGUErrorTolerance & tol)
        {
            IGUMismatch mismatch{};


            if(!nearlyEqual(cogData.UValue, frameData.UValue, Tol{.abs = tol.UCenter, .rel = 0.0}))
            {
                mismatch.uCenterMissmatch = true;
            }

            if(!nearlyEqual(
                 cogData.Thickness, frameData.Thickness, Tol{.abs = tol.Thickness, .rel = 0.0}))
            {
                mismatch.thicknessMissmatch = true;
            }

            return mismatch;
        }

    }   // namespace


    IGUMismatch WindowVision::iguMissmatch(const double geometricalThickness) const
    {
        IGUMismatch result;

        if(!m_IGUSystem)
        {
            return result;   // No IGU assigned ⇒ nothing to check
        }

        const IGUData reference{.UValue = m_IGUSystem->getUValue(),
                                .Thickness = geometricalThickness};

        auto accumulateMismatch = [&](const std::optional<IGUData> & igu) {
            if(!igu)
            {
                return IGUMismatch{};
            }

            return checkFrameMismatch(IGUData{.UValue = igu->UValue, .Thickness = igu->Thickness},
                                      reference,
                                      m_IGUErrorTolerance);
        };

        for(const auto & frame : m_Frame | std::views::values)
        {
            const IGUMismatch mismatch = accumulateMismatch(frame.frameData.iguData);
            result.uCenterMissmatch |= mismatch.uCenterMissmatch;
            result.thicknessMissmatch |= mismatch.thicknessMissmatch;

            if(result.any())
            {
                break;   // early out
            }
        }

        if(m_Divider)
        {
            const IGUMismatch mismatch = accumulateMismatch(m_Divider->frame.iguData);
            result.uCenterMissmatch |= mismatch.uCenterMissmatch;
            result.thicknessMissmatch |= mismatch.thicknessMissmatch;
        }

        return result;
    }

    void WindowVision::connectFrames()
    {
        m_Frame.at(FramePosition::Top).frame[FrameSide::Left] = m_Frame.at(FramePosition::Right);
        m_Frame.at(FramePosition::Top).frame[FrameSide::Right] = m_Frame.at(FramePosition::Left);

        m_Frame.at(FramePosition::Bottom).frame[FrameSide::Right] =
          m_Frame.at(FramePosition::Right);
        m_Frame.at(FramePosition::Bottom).frame[FrameSide::Left] = m_Frame.at(FramePosition::Left);

        m_Frame.at(FramePosition::Left).frame[FrameSide::Left] = m_Frame.at(FramePosition::Top);
        m_Frame.at(FramePosition::Left).frame[FrameSide::Right] = m_Frame.at(FramePosition::Bottom);

        m_Frame.at(FramePosition::Right).frame[FrameSide::Left] = m_Frame.at(FramePosition::Bottom);
        m_Frame.at(FramePosition::Right).frame[FrameSide::Right] = m_Frame.at(FramePosition::Top);
    }


    void WindowVision::resizeIGU()
    {
        const auto width{m_Width - m_Frame.at(FramePosition::Left).frameData.ProjectedFrameDimension
                         - m_Frame.at(FramePosition::Right).frameData.ProjectedFrameDimension};
        const auto height{m_Height
                          - m_Frame.at(FramePosition::Top).frameData.ProjectedFrameDimension
                          - m_Frame.at(FramePosition::Bottom).frameData.ProjectedFrameDimension};
        m_IGUSystem->setWidthAndHeight(width, height);
        m_IGUSystem->setInteriorAndExteriorSurfacesHeight(m_ExteriorSurfaceHeight);
        m_IGUUvalue = m_IGUSystem->getUValue();
        m_HExterior = m_IGUSystem->getH(System::SHGC, Environment::Outdoor);
    }

    double WindowVision::shgcCommon(double cogSHGC) const
    {
        double frameWeighted = 0.0;

        // Frame contribution
        for(const auto & frame : m_Frame | std::views::values)
        {
            frameWeighted += frameArea(frame)
                             * frameSHGC(frame.frameData.Absorptance,
                                         frame.frameData.UValue,
                                         frame.frameData.ProjectedFrameDimension,
                                         frame.frameData.WettedLength,
                                         m_HExterior);
        }

        // Divider contribution
        double dividerWeighted = 0.0;

        if(m_Divider)
        {
            const auto & d = m_Divider->frame;

            dividerWeighted += dividerArea()
                               * frameSHGC(d.Absorptance,
                                           Helper::dividerUValue(*m_IGUSystem, d),
                                           d.ProjectedFrameDimension,
                                           d.WettedLength,
                                           m_HExterior);
        }

        // Combine COG + frames + dividers
        return (cogSHGC + frameWeighted + dividerWeighted) / area();
    }

    double WindowVision::dividerArea() const
    {
        auto result{0.0};

        if(m_Divider.has_value())
        {
            const auto dividersWidth{
              m_Width - m_Frame.at(FramePosition::Left).frameData.ProjectedFrameDimension
              - m_Frame.at(FramePosition::Right).frameData.ProjectedFrameDimension};
            const auto dividersHeight{
              m_Height - m_Frame.at(FramePosition::Top).frameData.ProjectedFrameDimension
              - m_Frame.at(FramePosition::Bottom).frameData.ProjectedFrameDimension};
            const auto areaVertical{m_Divider->numberOfVertical * dividersHeight
                                    * m_Divider->frame.ProjectedFrameDimension};
            const auto areaHorizontal{m_Divider->numberOfHorizontal * dividersWidth
                                      * m_Divider->frame.ProjectedFrameDimension};
            const auto areaDoubleCounted{m_Divider->numberOfHorizontal * m_Divider->numberOfVertical
                                         * std::pow(m_Divider->frame.ProjectedFrameDimension, 2)};
            result = areaVertical + areaHorizontal - areaDoubleCounted;
        }

        return result;
    }

    double WindowVision::dividerEdgeOfGlassArea() const
    {
        auto result{0.0};

        if(m_Divider.has_value())
        {
            const auto eogWidth{m_Width
                                - m_Frame.at(FramePosition::Left).frameData.ProjectedFrameDimension
                                - m_Frame.at(FramePosition::Right).frameData.ProjectedFrameDimension
                                - 2 * ConstantsData::EOGHeight};
            const auto eogHeight{
              m_Height - m_Frame.at(FramePosition::Top).frameData.ProjectedFrameDimension
              - m_Frame.at(FramePosition::Bottom).frameData.ProjectedFrameDimension
              - 2 * ConstantsData::EOGHeight};
            const auto areaVertical{m_Divider->numberOfVertical * 2 * ConstantsData::EOGHeight
                                    * eogHeight};
            const auto areaHorizontal{m_Divider->numberOfHorizontal * 2 * ConstantsData::EOGHeight
                                      * eogWidth};
            const auto dividerAreaSubtract{
              4 * m_Divider->numberOfVertical * m_Divider->numberOfHorizontal
              * m_Divider->frame.ProjectedFrameDimension * ConstantsData::EOGHeight};
            const auto eogAreaSubtract{4 * m_Divider->numberOfVertical
                                       * m_Divider->numberOfHorizontal * ConstantsData::EOGHeight
                                       * ConstantsData::EOGHeight};

            result = areaVertical + areaHorizontal - dividerAreaSubtract - eogAreaSubtract;
        }

        return result;
    }

    double WindowVision::frameProjectedArea() const
    {
        auto area{0.0};

        for(const auto & val : m_Frame | std::views::values)
        {
            area += frameArea(val);
        }

        return area;
    }

    double WindowVision::totalEdgeOfGlassArea() const
    {
        auto area{0.0};

        for(const auto & frame : m_Frame | std::views::values)
        {
            area += ISO15099::edgeOfGlassArea(frame);
        }

        return area;
    }
}   // namespace Tarcog::ISO15099
