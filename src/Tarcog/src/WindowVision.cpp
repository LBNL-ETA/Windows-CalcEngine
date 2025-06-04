#include <ranges>
#include <numeric>

#include <WCECommon.hpp>

#include "WindowVision.hpp"
#include "EnvironmentConfigurations.hpp"
#include "IGU.hpp"

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

        // Frame U-value weighted sum
        const double frameWU =
          std::accumulate(frames.begin(), frames.end(), 0.0, [](double acc, const auto & frame) {
              return acc + projectedArea(frame) * frame.frameData.UValue;
          });

        // Edge-of-glass weighted sum (functional style)
        const double edgeWU = std::accumulate(
          frames.begin(), frames.end(), 0.0, [this](double acc, const auto & frame) {
              return acc
                     + Tarcog::ISO15099::edgeOfGlassArea(frame)
                         * Helper::frameEdgeUValue(*m_IGUSystem, frame.frameData);
          });

        // COG weighted U-value
        const double cogWU =
          m_IGUUvalue
          * (area() - frameProjectedArea() - edgeOfGlassArea() - dividerArea() - dividerEdgeArea());

        // Divider contributions
        const double divWU =
          m_Divider ? dividerArea() * Helper::dividerUValue(*m_IGUSystem, *m_Divider) : 0.0;
        const double divEdgeWU =
          m_Divider ? dividerEdgeArea() * Helper::dividerEdgeUValue(*m_IGUSystem, *m_Divider) : 0.0;

        auto visionAreaTest{area() - frameProjectedArea() - edgeOfGlassArea() - dividerArea()
                            - dividerEdgeArea()};
        auto divAreaTest{dividerArea()};
        auto divEdgeAreaTest{dividerEdgeArea()};
        std::map<FramePosition, double> frameAreasTest;
        for(const auto & [position, frame] : m_Frame)
        {
            frameAreasTest[position] = projectedArea(frame);
        }
        std::map<FramePosition, double> edgeOfGlassAreasTest;
        for(const auto & [position, frame] : m_Frame)
        {
            edgeOfGlassAreasTest[position] = Tarcog::ISO15099::edgeOfGlassArea(frame);
        }

        return (cogWU + frameWU + edgeWU + divWU + divEdgeWU) / area();
    }

    double WindowVision::shgc() const
    {
        return shgc(m_Tsol);
    }

    double WindowVision::shgc(const double tSol) const
    {
        auto frameWeightedSHGC{0.0};

        for(const auto & frame : m_Frame | std::views::values)
        {
            frameWeightedSHGC += projectedArea(frame)
                                 * ISO15099::frameSHGC(frame.frameData.Absorptance,
                                                       frame.frameData.UValue,
                                                       frame.frameData.ProjectedFrameDimension,
                                                       frame.frameData.WettedLength,
                                                       m_HExterior);
        }

        const auto COGWeightedSHGC{m_IGUSystem->getSHGC(tSol)
                                   * (area() - frameProjectedArea() - dividerArea())};

        auto dividerWeightedSHGC{0.0};
        if(m_Divider.has_value())
        {
            auto shgcTest = ISO15099::frameSHGC(
                m_Divider->Absorptance,
                m_Divider->UValue,
                m_Divider->ProjectedFrameDimension,
                m_Divider->WettedLength,
                m_HExterior
            );
            dividerWeightedSHGC += dividerArea() * ISO15099::frameSHGC(
                m_Divider->Absorptance,
                m_Divider->UValue,
                m_Divider->ProjectedFrameDimension,
                m_Divider->WettedLength,
                m_HExterior
            );
        }

        return (COGWeightedSHGC + frameWeightedSHGC + dividerWeightedSHGC) / area();
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

    void WindowVision::setFrameTypes(std::map<FramePosition, FrameType> frameTypes)
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

    void WindowVision::setDividers(FrameData divider, size_t nHorizontal, size_t nVertical)
    {
        m_Divider = divider;
        m_NumOfHorizontalDividers = nHorizontal;
        m_NumOfVerticalDividers = nVertical;
        std::map<FramePosition, size_t> numOfDivs{
          {FramePosition::Top, m_NumOfVerticalDividers},
          {FramePosition::Bottom, m_NumOfVerticalDividers},
          {FramePosition::Left, m_NumOfHorizontalDividers},
          {FramePosition::Right, m_NumOfHorizontalDividers}};

        for(auto & [key, frame] : m_Frame)
        {
            frame.dividerArea = m_Divider->ProjectedFrameDimension * ConstantsData::EOGHeight;
            frame.numberOfDividers = numOfDivs.at(key);
        }
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

        IGUMismatch
          checkFrameMismatch(const FrameData & frame,
                             IIGUSystem & igu,   // Cannot be const because of lazy evaluation
                             const IGUErrorTolerance & tol)
        {
            IGUMismatch mismatch;

            if(!frame.iguData)
                return mismatch;

            const auto & spec = *frame.iguData;

            if(std::abs(igu.getUValue() - spec.UValue) > tol.UCenter)
            {
                mismatch.uCenterMissmatch = true;
            }

            if(std::abs(igu.thickness() - spec.Thickness) > tol.Thickness)
            {
                mismatch.thicknessMissmatch = true;
            }

            return mismatch;
        }

    }   // namespace


    IGUMismatch WindowVision::iguMissmatch() const
    {
        IGUMismatch result;

        if(!m_IGUSystem)
        {
            return result;   // No IGU assigned ⇒ nothing to check
        }

        // Check all frame positions
        for(const auto & frame : m_Frame | std::views::values)
        {
            const auto mismatch =
              checkFrameMismatch(frame.frameData, *m_IGUSystem, m_IGUErrorTolerance);
            result.uCenterMissmatch |= mismatch.uCenterMissmatch;
            result.thicknessMissmatch |= mismatch.thicknessMissmatch;

            if(result.any())
                break;   // early out if both already true
        }

        // Check divider if set
        if(m_Divider.has_value())
        {
            const auto mismatch = checkFrameMismatch(*m_Divider, *m_IGUSystem, m_IGUErrorTolerance);
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
            const auto areaVertical{m_NumOfVerticalDividers * dividersHeight
                                    * m_Divider->ProjectedFrameDimension};
            const auto areaHorizontal{m_NumOfHorizontalDividers * dividersWidth
                                      * m_Divider->ProjectedFrameDimension};
            const auto areaDoubleCounted{m_NumOfHorizontalDividers * m_NumOfVerticalDividers
                                         * std::pow(m_Divider->ProjectedFrameDimension, 2)};
            result = areaVertical + areaHorizontal - areaDoubleCounted;
        }

        return result;
    }

    double WindowVision::dividerEdgeArea() const
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
            const auto areaVertical{m_NumOfVerticalDividers * 2 * ConstantsData::EOGHeight
                                    * eogHeight};
            const auto areaHorizontal{m_NumOfHorizontalDividers * 2 * ConstantsData::EOGHeight
                                      * eogWidth};
            const auto dividerAreaSubtract{4 * m_NumOfVerticalDividers * m_NumOfHorizontalDividers
                                           * m_Divider->ProjectedFrameDimension
                                           * ConstantsData::EOGHeight};
            const auto eogAreaSubtract{4 * m_NumOfVerticalDividers * m_NumOfHorizontalDividers
                                       * ConstantsData::EOGHeight * ConstantsData::EOGHeight};

            result = areaVertical + areaHorizontal - dividerAreaSubtract - eogAreaSubtract;
        }

        return result;
    }

    double WindowVision::frameProjectedArea() const
    {
        auto area{0.0};

        for(const auto & val : m_Frame | std::views::values)
        {
            area += projectedArea(val);
        }

        return area;
    }

    double WindowVision::edgeOfGlassArea() const
    {
        auto area{0.0};

        for(const auto & frame : m_Frame | std::views::values)
        {
            area += ISO15099::edgeOfGlassArea(frame);
        }

        return area;
    }
}   // namespace Tarcog::ISO15099
