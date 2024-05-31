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

    double CVenetianCellDescription::segmentLength(const size_t Index) const
    {
        const auto aSegments = m_Geometry.segments();
        if(Index > aSegments.size())
        {
            throw std::runtime_error("Incorrect index for venetian segment.");
        }
        const auto aSegment = aSegments[Index];
        return aSegment.length();
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

}   // namespace SingleLayerOptics
