#pragma once

#include <memory>
#include <vector>

#include <WCECommon.hpp>
#include <WCEViewer.hpp>

#include "CellDescription.hpp"
#include "VenetianSlat.hpp"
#include "VenetianSegments.hpp"

namespace SingleLayerOptics
{
    class CVenetianCellDescription : public ICellDescription
    {
    public:
        virtual ~CVenetianCellDescription() = default;

        CVenetianCellDescription(const FenestrationCommon::VenetianGeometry & t_Geometry,
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
                                                     const CBeamDirection & t_Direction);

        // view factor of the beam entering the cell with profile angle
        std::vector<Viewer::BeamViewFactor> beamViewFactors(double t_ProfileAngle,
                                                            FenestrationCommon::Side t_Side);

        std::vector<Viewer::BeamViewFactor> beamViewFactors(FenestrationCommon::Side t_Side,
                                                            const CBeamDirection & t_Direction);

        //! Calculates corrected view factors for the given incoming beam direction
        std::vector<double> scaledBeamViewFactors(FenestrationCommon::Side t_Side,
                                                  const CBeamDirection & t_Direction);

        std::vector<double> visibleBeamSegmentFraction(FenestrationCommon::Side t_Side,
                                                       const CBeamDirection & t_Direction);

        // Direct to direct component of the ray
        double T_dir_dir(FenestrationCommon::Side t_Side,
                         const CBeamDirection & t_Direction) override;
        double R_dir_dir(FenestrationCommon::Side t_Side,
                         const CBeamDirection & t_Direction) override;

        [[nodiscard]] FenestrationCommon::VenetianGeometry getVenetianGeometry() const;
        [[nodiscard]] size_t numOfSegments() const;

        void preCalculateForProfileAngles(FenestrationCommon::Side side,
                                          const std::vector<double> & t_ProfileAngles);

    private:
        std::vector<BeamSegmentView> beamViewFactorsToBeamSegmentViews(
          size_t numberOfSegments,
          FenestrationCommon::Side t_Side,
          const CBeamDirection & t_Direction,
          const std::vector<Viewer::BeamViewFactor> & t_BeamViewFactors);

        double getCellSpacing() const;

        FenestrationCommon::VenetianGeometry m_VenetianGeometry;
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
