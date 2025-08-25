#pragma once

#include <memory>
#include <vector>

#include <WCECommon.hpp>
#include <WCEViewer.hpp>

#include "CellDescription.hpp"
#include "VenetianSlat.hpp"
#include "VenetianSegments.hpp"
#include "BSDFDirections.hpp"

namespace SingleLayerOptics
{
    enum class SlatPosition
    {
        Top = 0,
        Bottom = 1
    };

    class CVenetianCellDescription : public ICellDescription
    {
    public:
        virtual ~CVenetianCellDescription() = default;

        CVenetianCellDescription(const FenestrationCommon::Venetian::Geometry & t_Geometry,
                                 size_t t_NumOfSlatSegments);

        // Makes exact copy of cell description
        [[nodiscard]] std::shared_ptr<CVenetianCellDescription> getBackwardFlowCell() const;
        [[nodiscard]] size_t numberOfSegments() const;
        [[nodiscard]] double segmentLength(size_t Index) const;
        [[nodiscard]] double segmentAngle(size_t Index) const;

        // View factors of enclosure slats
        FenestrationCommon::SquareMatrix viewFactors();

        //! Calculates view factors for given profile angle
        FenestrationCommon::SquareMatrix viewFactors(FenestrationCommon::Side t_Side,
                                                     BSDFDirection t_BeamDirection,
                                                     const CBeamDirection & t_Direction);

        // view factor of the beam entering the cell with profile angle
        std::vector<Viewer::BeamViewFactor> cellBeamViewFactors(double t_ProfileAngle,
                                                                FenestrationCommon::Side t_Side,
                                                                BSDFDirection t_Direction);

        std::vector<Viewer::BeamViewFactor> cellBeamViewFactors(FenestrationCommon::Side t_Side,
                                                                BSDFDirection t_BeamDirection,
                                                                const CBeamDirection & t_Direction);

        //! Calculates corrected view factors for the given incoming beam direction
        std::vector<double> scaledBeamViewFactors(FenestrationCommon::Side t_Side,
                                                  BSDFDirection t_BeamDirection,
                                                  const CBeamDirection & t_Direction);

        //! Calculates segments that are visible for the beam including indoor and outdoor segments
        std::vector<double> visibleBeamSegmentFraction(FenestrationCommon::Side t_Side,
                                                       BSDFDirection t_BeamDirection,
                                                       const CBeamDirection & t_Direction);

        //! Calculates segments that are visible for the beam excluding indoor and outdoor segments
        std::vector<double> visibleBeamSegmentFractionSlatsOnly(FenestrationCommon::Side t_Side,
                                                                BSDFDirection t_BeamDirection,
                                                                const CBeamDirection & t_Direction);

        // Direct to direct component of the ray
        double Beam_dir_dir(FenestrationCommon::Side t_Side,
                         const CBeamDirection & t_Direction) override;

        [[nodiscard]] FenestrationCommon::Venetian::Geometry getVenetianGeometry() const;
        [[nodiscard]] size_t numOfSegments() const;

        void preCalculateForProfileAngles(FenestrationCommon::Side side,
                                          const std::vector<double> & t_ProfileAngles);

        [[nodiscard]] const Viewer::CGeometry2D & getSlats(SlatPosition position) const;

        //! Return all slats in CW direction (No indoor and outdoor just venetian slats)
        [[nodiscard]] std::vector<Viewer::CViewSegment2D> getSlats() const;

        [[nodiscard]] double getCellSpacing() const;

    private:
        std::vector<BeamSegmentView> beamViewFactorsToBeamSegmentViews(
          size_t numberOfSegments,
          FenestrationCommon::Side t_Side,
          const CBeamDirection & t_Direction,
          const std::vector<Viewer::BeamViewFactor> & t_BeamViewFactors);

        FenestrationCommon::Venetian::Geometry m_VenetianGeometry;
        size_t m_NumOfSegments;

        // Top and bottom slats of venetian cell
        Viewer::CGeometry2D m_Top;
        Viewer::CGeometry2D m_Bottom;

        // Complete enclosure from venetian cell
        Viewer::CGeometry2D m_Geometry;

        // Geometry to handle direct to direct beam component
        Viewer::CGeometry2DBeam m_BeamGeometry;

        //! Storing profile angles for backward cell calculations
        std::map<FenestrationCommon::Side, std::vector<double>> m_ProfileAngles;
    };

}   // namespace SingleLayerOptics
