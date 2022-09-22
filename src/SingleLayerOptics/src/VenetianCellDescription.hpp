#pragma once

#include <memory>
#include <vector>

#include <WCEViewer.hpp>

#include "CellDescription.hpp"
#include "VenetianSlat.hpp"

namespace FenestrationCommon
{
    class SquareMatrix;
    enum class Side;

}   // namespace FenestrationCommon

namespace SingleLayerOptics
{
    class CVenetianSlat;

    class CVenetianCellDescription : public ICellDescription
    {
    public:
        CVenetianCellDescription(double t_SlatWidth,
                                 double t_SlatSpacing,
                                 double t_SlatTiltAngle,
                                 double t_CurvatureRadius,
                                 size_t t_NumOfSlatSegments);

        // Makes exact copy of cell description
        std::shared_ptr<CVenetianCellDescription> makeBackwardCell() const;
        size_t numberOfSegments() const;
        double segmentLength(const size_t Index) const;

        // View factors of enclosure slats
        FenestrationCommon::SquareMatrix viewFactors();

        // view factor of the beam entering the cell with profile angle
        std::vector<Viewer::BeamViewFactor>
          beamViewFactors(double t_ProfileAngle, const FenestrationCommon::Side t_Side);

        // Direct to direct component of the ray
        double T_dir_dir(FenestrationCommon::Side t_Side,
                         const CBeamDirection & t_Direction) override;
        double R_dir_dir(FenestrationCommon::Side t_Side,
                         const CBeamDirection & t_Direction) override;

        [[nodiscard]] double slatWidth() const;
        [[nodiscard]] double slatSpacing() const;
        [[nodiscard]] double slatTiltAngle() const;
        [[nodiscard]] double curvatureRadius() const;
        [[nodiscard]] size_t numOfSegments() const;

    private:
        double m_SlatWidth;
        double m_SlatSpacing;
        double m_SlatTiltAngle;
        double m_CurvatureRadius;
        size_t m_NumOfSegments;

        // Top and bottom slats of venetian cell
        CVenetianSlat m_Top;
        CVenetianSlat m_Bottom;

        // Complete enclosure from venetian cell
        Viewer::CGeometry2D m_Geometry;

        // Geometry to handle direct to direct beam component
        Viewer::CGeometry2DBeam m_BeamGeometry;
    };

}   // namespace SingleLayerOptics
