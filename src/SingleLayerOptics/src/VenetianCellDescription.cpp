#include <cassert>
#include <memory>
#include <stdexcept>

#include <WCECommon.hpp>
#include <WCEViewer.hpp>

#include "VenetianCellDescription.hpp"
#include "BeamDirection.hpp"

namespace SingleLayerOptics
{
    namespace Helper
    {
        FenestrationCommon::Venetian::Geometry
          bottomGeometry(const FenestrationCommon::Venetian::Geometry & t_Geometry)
        {
            auto bottomGeometry{t_Geometry};
            bottomGeometry.SlatSpacing = 0.0;
            return bottomGeometry;
        }
    }   // namespace Helper

    CVenetianCellDescription::CVenetianCellDescription(
      const FenestrationCommon::Venetian::Geometry & t_Geometry, size_t t_NumOfSlatSegments) :
        m_VenetianGeometry(FenestrationCommon::Venetian::adjustSlatTiltAngle(t_Geometry)),
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
        const Viewer::CSegment2D & segment(const Viewer::CGeometry2D & geometry, size_t index)
        {
            const auto & segments{geometry.segments()};
            if(index >= segments.size())
            {
                throw std::runtime_error("Incorrect index for venetian segment.");
            }
            return segments[index];
        }

        template<typename Func>
        double getSegmentProperty(const Viewer::CGeometry2D & geometry, size_t index, Func func)
        {
            return (Helper::segment(geometry, index).*func)();
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
      CVenetianCellDescription::scaledBeamViewFactors(FenestrationCommon::Side t_Side,
                                                      BSDFDirection t_BeamDirection,
                                                      const CBeamDirection & t_Direction)
    {
        // clang-format off
        return Helper::scaleForSegmentThatIsHit(
                                          beamViewFactorsToBeamSegmentViews(
                                              numberOfSegments(),
                                              t_Side,
                                              t_Direction,
                                              cellBeamViewFactors(t_Side, t_BeamDirection, t_Direction))
                                          );
        // clang-format on
    }

    std::vector<double>
      CVenetianCellDescription::visibleBeamSegmentFraction(FenestrationCommon::Side t_Side,
                                                           BSDFDirection t_BeamDirection,
                                                           const CBeamDirection & t_Direction)
    {
        // clang-format off
        auto bvf{cellBeamViewFactors(t_Side, t_BeamDirection, t_Direction)};

        return Helper::visibleSegmentPercentage(
                                          beamViewFactorsToBeamSegmentViews(
                                              numberOfSegments(),
                                              t_Side,
                                              t_Direction,
                                              bvf)
                                          );
        // clang-format on
    }

    std::vector<double> CVenetianCellDescription::visibleBeamSegmentFractionSlatsOnly(
      FenestrationCommon::Side t_Side,
      BSDFDirection t_BeamDirection,
      const CBeamDirection & t_Direction)
    {
        auto result = CVenetianCellDescription::visibleBeamSegmentFraction(
          t_Side, t_BeamDirection, t_Direction);

        // Ensure the result vector has the expected size
        if(result.size() != 2 * m_NumOfSegments + 2)
        {
            throw std::runtime_error("Unexpected size of the result vector");
        }

        // Erase the first value
        result.erase(result.begin());

        // Erase the value after the first numberOfSegments values
        result.erase(result.begin() + m_NumOfSegments);

        return result;
    }

    FenestrationCommon::SquareMatrix
      CVenetianCellDescription::viewFactors(FenestrationCommon::Side t_Side,
                                            BSDFDirection t_BeamDirection,
                                            const CBeamDirection & t_Direction)
    {
        const auto scaled_vf{scaledBeamViewFactors(t_Side, t_BeamDirection, t_Direction)};

        auto diffuseVF{m_Geometry.viewFactors()};
        for(size_t i = 0u; i < scaled_vf.size(); ++i)
        {
            diffuseVF(0, i) = scaled_vf[i];
        }
        return diffuseVF;
    }

    std::vector<Viewer::BeamViewFactor> CVenetianCellDescription::cellBeamViewFactors(
      double t_ProfileAngle, FenestrationCommon::Side t_Side, const BSDFDirection t_Direction)
    {
        // Define a map of conditions for reverting the angle
        static const std::map<std::pair<BSDFDirection, FenestrationCommon::Side>, bool> revertMap = {
          {{BSDFDirection::Incoming, FenestrationCommon::Side::Front}, true},
          {{BSDFDirection::Outgoing, FenestrationCommon::Side::Back}, true},
          {{BSDFDirection::Incoming, FenestrationCommon::Side::Back}, false},
          {{BSDFDirection::Outgoing, FenestrationCommon::Side::Front}, false}
        };

        // Check if the current (t_Direction, t_Side) combination should revert the angle
        const bool shouldRevertAngle = revertMap.at({t_Direction, t_Side});
        const auto profileAngle = shouldRevertAngle ? -t_ProfileAngle : t_ProfileAngle;

        return m_BeamGeometry.beamViewFactors(profileAngle, t_Side);
    }

    std::vector<Viewer::BeamViewFactor>
      CVenetianCellDescription::cellBeamViewFactors(FenestrationCommon::Side t_Side,
                                                    BSDFDirection t_BeamDirection,
                                                    const CBeamDirection & t_Direction)
    {
        return cellBeamViewFactors(t_Direction.profileAngle(), t_Side, t_BeamDirection);
    }

    double CVenetianCellDescription::Beam_dir_dir(const FenestrationCommon::Side t_Side,
                                               const CBeamDirection & t_Direction)
    {
        return m_BeamGeometry.directToDirect(-t_Direction.profileAngle(), t_Side);
    }

    FenestrationCommon::Venetian::Geometry CVenetianCellDescription::getVenetianGeometry() const
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

        B[sideIndex.at(t_Side)].viewFactor = Beam_dir_dir(t_Side, t_Direction);
        B[sideIndex.at(t_Side)].percentViewed = Beam_dir_dir(t_Side, t_Direction);

        return B;
    }

    double CVenetianCellDescription::getCellSpacing() const
    {
        if(m_Top.segments().empty() || m_Bottom.segments().empty())
        {
            return 0;
        }

        return m_Top.segments().front().startPoint().y()
               - m_Bottom.segments().back().endPoint().y();
    }

    const Viewer::CGeometry2D & CVenetianCellDescription::getSlats(SlatPosition position) const
    {
        return position == SlatPosition::Top ? m_Top : m_Bottom;
    }

    std::vector<Viewer::CViewSegment2D> CVenetianCellDescription::getSlats() const
    {
        const auto upperSlats = getSlats(SlatPosition::Top);
        const auto lowerSlats = getSlats(SlatPosition::Bottom);

        std::vector<Viewer::CViewSegment2D> allSlats;
        allSlats.reserve(upperSlats.segments().size() + lowerSlats.segments().size());

        allSlats.insert(allSlats.end(), upperSlats.segments().begin(), upperSlats.segments().end());
        allSlats.insert(allSlats.end(), lowerSlats.segments().begin(), lowerSlats.segments().end());

        return allSlats;
    }

}   // namespace SingleLayerOptics
