#include <cassert>
#include <memory>
#include <stdexcept>

#include "VenetianCellDescription.hpp"
#include "BeamDirection.hpp"
#include "WCECommon.hpp"

namespace SingleLayerOptics
{
    CVenetianCellDescription::CVenetianCellDescription(const double t_SlatWidth,
                                                       const double t_SlatSpacing,
                                                       const double t_SlatTiltAngle,
                                                       const double t_CurvatureRadius,
                                                       const size_t t_NumOfSlatSegments) :
        m_SlatWidth(t_SlatWidth),
        m_SlatSpacing(t_SlatSpacing),
        m_SlatTiltAngle(t_SlatTiltAngle),
        m_CurvatureRadius(t_CurvatureRadius),
        m_NumOfSegments(t_NumOfSlatSegments),
        m_Top(t_SlatWidth,
              t_SlatSpacing,
              t_SlatTiltAngle,
              t_CurvatureRadius,
              t_NumOfSlatSegments,
              SegmentsDirection::Positive),
        m_Bottom(t_SlatWidth,
                 0,
                 t_SlatTiltAngle,
                 t_CurvatureRadius,
                 t_NumOfSlatSegments,
                 SegmentsDirection::Negative)
    {
        Viewer::CViewSegment2D exteriorSegment(m_Bottom.geometry().lastPoint(),
                                               m_Top.geometry().firstPoint());

        Viewer::CViewSegment2D interiorSegment(m_Top.geometry().lastPoint(),
                                               m_Bottom.geometry().firstPoint());

        m_Geometry.appendSegment(exteriorSegment);
        m_Geometry.appendGeometry2D(m_Top.geometry());
        m_Geometry.appendSegment(interiorSegment);
        m_Geometry.appendGeometry2D(m_Bottom.geometry());

        m_BeamGeometry.appendGeometry2D(m_Top.geometry());
        m_BeamGeometry.appendGeometry2D(m_Bottom.geometry());
    }

    size_t CVenetianCellDescription::numberOfSegments() const
    {
        // Two additional segments are for interior and exterior openness
        return 2 + m_Top.numberOfSegments() + m_Bottom.numberOfSegments();
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
        double slatWidth = m_Top.slatWidth();
        double slatSpacing = m_Top.slatSpacing();
        double slatTiltAngle = -m_Top.slatTiltAngle();
        double curvatureRadius = m_Top.curvatureRadius();
        size_t m_NumOfSlatSegments = m_Top.numberOfSegments();

        std::shared_ptr<CVenetianCellDescription> aBackwardCell =
          std::make_shared<CVenetianCellDescription>(
            slatWidth, slatSpacing, slatTiltAngle, curvatureRadius, m_NumOfSlatSegments);

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

    double CVenetianCellDescription::T_dir_dir(const FenestrationCommon::Side t_Side,
                                               const CBeamDirection & t_Direction)
    {
        const double aProfileAngle = t_Direction.profileAngle();
        return m_BeamGeometry.directToDirect(-aProfileAngle, t_Side);
    }

    double CVenetianCellDescription::R_dir_dir(const FenestrationCommon::Side,
                                               const CBeamDirection &)
    {
        return 0;
    }

    double CVenetianCellDescription::slatWidth() const
    {
        return m_SlatWidth;
    }

    double CVenetianCellDescription::slatSpacing() const
    {
        return m_SlatSpacing;
    }

    double CVenetianCellDescription::slatTiltAngle() const
    {
        return m_SlatTiltAngle;
    }

    double CVenetianCellDescription::curvatureRadius() const
    {
        return m_CurvatureRadius;
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
