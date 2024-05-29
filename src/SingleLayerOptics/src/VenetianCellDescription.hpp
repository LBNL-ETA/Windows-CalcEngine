#pragma once

#include <memory>
#include <vector>

#include <WCECommon.hpp>
#include <WCEViewer.hpp>

#include "CellDescription.hpp"
#include "VenetianSlat.hpp"

namespace SingleLayerOptics
{
    class CVenetianSlat;

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

        // View factors of enclosure slats
        FenestrationCommon::SquareMatrix viewFactors();

        // view factor of the beam entering the cell with profile angle
        std::vector<Viewer::BeamViewFactor> beamViewFactors(double t_ProfileAngle,
                                                            FenestrationCommon::Side t_Side);

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
        FenestrationCommon::VenetianGeometry m_VenetianGeometry;
        size_t m_NumOfSegments;

        // Top and bottom slats of venetian cell
        CVenetianSlat m_Top;
        CVenetianSlat m_Bottom;

        // Complete enclosure from venetian cell
        Viewer::CGeometry2D m_Geometry;

        // Geometry to handle direct to direct beam component
        Viewer::CGeometry2DBeam m_BeamGeometry;

        //! Storing profile angles for backward cell calculations
        std::map<FenestrationCommon::Side, std::vector<double>> m_ProfileAngles;
    };

}   // namespace SingleLayerOptics
