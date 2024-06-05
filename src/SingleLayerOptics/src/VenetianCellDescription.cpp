#include <cassert>
#include <memory>
#include <stdexcept>

#include "VenetianCellDescription.hpp"
#include "BeamDirection.hpp"
#include "WCECommon.hpp"

namespace SingleLayerOptics
{
    namespace Helper
    {

        FenestrationCommon::VenetianGeometry
          bottomGeometry(const FenestrationCommon::VenetianGeometry & t_Geometry)
        {
            auto bottomGeometry{t_Geometry};
            bottomGeometry.SlatSpacing = 0.0;
            return bottomGeometry;
        }
    }   // namespace Helper

    CVenetianCellDescription::CVenetianCellDescription(
      const FenestrationCommon::VenetianGeometry & t_Geometry, size_t t_NumOfSlatSegments) :
        m_VenetianGeometry(FenestrationCommon::adjustSlatTiltAngle(t_Geometry)),
        m_NumOfSegments(t_NumOfSlatSegments),
        m_Top(buildViewerSlat(t_Geometry, t_NumOfSlatSegments, SegmentsDirection::Positive)),
        m_Bottom(buildViewerSlat(
          Helper::bottomGeometry(t_Geometry), t_NumOfSlatSegments, SegmentsDirection::Negative))
    {
        Viewer::CViewSegment2D exteriorSegment(m_Bottom.lastPoint(), m_Top.firstPoint());

        Viewer::CViewSegment2D interiorSegment(m_Top.lastPoint(), m_Bottom.firstPoint());

        m_Geometry.appendSegment(exteriorSegment);
        m_Geometry.appendGeometry2D(m_Top);
        m_Geometry.appendSegment(interiorSegment);
        m_Geometry.appendGeometry2D(m_Bottom);

        m_BeamGeometry.appendGeometry2D(m_Top);
        m_BeamGeometry.appendGeometry2D(m_Bottom);
    }

    size_t CVenetianCellDescription::numberOfSegments() const
    {
        // Two additional segments are for interior and exterior openness
        return 2 + m_Top.segments().size() + m_Bottom.segments().size();
    }

    namespace Helper
    {
        template<typename Func>
        double getSegmentProperty(const Viewer::CGeometry2D & geometry, size_t index, Func func)
        {
            const auto segments = geometry.segments();
            if(index >= segments.size())
            {
                throw std::runtime_error("Incorrect index for venetian segment.");
            }
            const auto & segment = segments[index];
            return (segment.*func)();
        }
    }   // namespace Helper


    double CVenetianCellDescription::segmentLength(const size_t Index) const
    {
        return Helper::getSegmentProperty(m_Geometry, Index, &Viewer::CSegment2D::length);
    }

    double CVenetianCellDescription::segmentAngle(size_t Index) const
    {
        return Helper::getSegmentProperty(m_Geometry, Index, &Viewer::CSegment2D::angle);
    }

    std::shared_ptr<CVenetianCellDescription> CVenetianCellDescription::getBackwardFlowCell() const
    {
        auto venetianGeometry{m_VenetianGeometry};
        venetianGeometry.SlatTiltAngle = -venetianGeometry.SlatTiltAngle;
        size_t m_NumOfSlatSegments = m_Top.segments().size();

        std::shared_ptr<CVenetianCellDescription> aBackwardCell =
          std::make_shared<CVenetianCellDescription>(venetianGeometry, m_NumOfSlatSegments);

        if(!m_ProfileAngles.empty())
        {
            aBackwardCell->preCalculateForProfileAngles(
              FenestrationCommon::Side::Front, m_ProfileAngles.at(FenestrationCommon::Side::Back));
            aBackwardCell->preCalculateForProfileAngles(
              FenestrationCommon::Side::Back, m_ProfileAngles.at(FenestrationCommon::Side::Front));
        }
        return aBackwardCell;
    }

    FenestrationCommon::SquareMatrix CVenetianCellDescription::viewFactors()
    {
        return m_Geometry.viewFactors();
    }

    namespace Helper
    {
        std::vector<double> scaleForSegmentThatIsHit(const std::vector<BeamSegmentView> & v_f)
        {
            std::vector<double> result(v_f.size());

            std::transform(std::begin(v_f),
                           std::end(v_f),
                           std::begin(result),
                           [](BeamSegmentView vf) { return vf.viewFactor * vf.percentViewed; });

            return result;
        }

        std::vector<double> visibleSegmentPercentage(const std::vector<BeamSegmentView> & v_f)
        {
            std::vector<double> result(v_f.size());

            std::transform(std::begin(v_f),
                           std::end(v_f),
                           std::begin(result),
                           [](BeamSegmentView vf) { return vf.percentViewed; });

            return result;
        }
    }   // namespace Helper

    std::vector<double>
      CVenetianCellDescription::scaledViewFactors(FenestrationCommon::Side t_Side,
                                                  const CBeamDirection & t_Direction)
    {
        // clang-format off
        return Helper::scaleForSegmentThatIsHit(
                                          beamViewFactorsToBeamSegmentViews(
                                              numberOfSegments(),
                                              t_Side,
                                              t_Direction,
                                              beamViewFactors(t_Side, t_Direction))
                                          );
        // clang-format on
    }

    std::vector<double>
      CVenetianCellDescription::visibleSegmentFraction(FenestrationCommon::Side t_Side,
                                                       const CBeamDirection & t_Direction)
    {
        // clang-format off
        return Helper::visibleSegmentPercentage(
                                          beamViewFactorsToBeamSegmentViews(
                                              numberOfSegments(),
                                              t_Side,
                                              t_Direction,
                                              beamViewFactors(t_Side, t_Direction))
                                          );
        // clang-format on
    }

    FenestrationCommon::SquareMatrix
      CVenetianCellDescription::viewFactors(FenestrationCommon::Side t_Side,
                                            const CBeamDirection & t_Direction)
    {
        const auto scaled_vf{scaledViewFactors(t_Side, t_Direction)};

        auto diffuseVF{m_Geometry.viewFactors()};
        for(int i = 0; i < scaled_vf.size(); ++i)
        {
            diffuseVF(0, i) = scaled_vf[i];
        }
        return diffuseVF;
    }

    std::vector<Viewer::BeamViewFactor>
      CVenetianCellDescription::beamViewFactors(const double t_ProfileAngle,
                                                const FenestrationCommon::Side t_Side)
    {
        return m_BeamGeometry.beamViewFactors(-t_ProfileAngle, t_Side);
    }

    std::vector<Viewer::BeamViewFactor>
      CVenetianCellDescription::beamViewFactors(FenestrationCommon::Side t_Side,
                                                const CBeamDirection & t_Direction)
    {
        return beamViewFactors(t_Side == FenestrationCommon::Side::Front
                                 ? t_Direction.profileAngle()
                                 : -t_Direction.profileAngle(),
                               t_Side);
    }

    double CVenetianCellDescription::T_dir_dir(const FenestrationCommon::Side t_Side,
                                               const CBeamDirection & t_Direction)
    {
        return m_BeamGeometry.directToDirect(-t_Direction.profileAngle(), t_Side);
    }

    double CVenetianCellDescription::R_dir_dir(const FenestrationCommon::Side,
                                               const CBeamDirection &)
    {
        return 0;
    }

    FenestrationCommon::VenetianGeometry CVenetianCellDescription::getVenetianGeometry() const
    {
        return m_VenetianGeometry;
    }

    size_t CVenetianCellDescription::numOfSegments() const
    {
        return m_NumOfSegments;
    }

    void CVenetianCellDescription::preCalculateForProfileAngles(
      FenestrationCommon::Side side, const std::vector<double> & t_ProfileAngles)
    {
        m_ProfileAngles[side] = t_ProfileAngles;
        m_BeamGeometry.precalculateForProfileAngles(side, t_ProfileAngles);
    }

    std::vector<BeamSegmentView> CVenetianCellDescription::beamViewFactorsToBeamSegmentViews(
      const size_t numberOfSegments,
      FenestrationCommon::Side t_Side,
      const CBeamDirection & t_Direction,
      const std::vector<Viewer::BeamViewFactor> & t_BeamViewFactors)
    {
        std::vector<BeamSegmentView> B(numberOfSegments);
        size_t index = 0;
        for(const Viewer::BeamViewFactor & aVF : t_BeamViewFactors)
        {
            if(aVF.enclosureIndex == 0)
            {   // Top
                index = aVF.segmentIndex + 1;
            }
            else if(aVF.enclosureIndex == 1)
            {   // Bottom
                index = static_cast<size_t>(numberOfSegments / 2) + 1u + aVF.segmentIndex;
            }
            else
            {
                assert("Incorrect value for enclosure. Cannot have more than three enclosures.");
            }
            B[index].viewFactor = aVF.value;
            B[index].percentViewed = aVF.percentHit;
        }

        using FenestrationCommon::Side;
        const std::map<Side, size_t> sideIndex{
          {Side::Front, static_cast<size_t>(numberOfSegments / 2)}, {Side::Back, 0}};

        B[sideIndex.at(t_Side)].viewFactor = T_dir_dir(t_Side, t_Direction);
        B[sideIndex.at(t_Side)].percentViewed = T_dir_dir(t_Side, t_Direction);

        return B;
    }

    double CVenetianCellDescription::getCellSpacing() const
    {
        if(m_Top.segments().size() == 0 || m_Bottom.segments().size() == 0)
        {
            return 0;
        }

        return m_Top.segments().front().startPoint().y()
               - m_Bottom.segments().back().endPoint().y();
    }

}   // namespace SingleLayerOptics
