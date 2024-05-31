#pragma once

namespace SingleLayerOptics
{
    class CVenetianCellDescription;
    class CBeamDirection;
    class CMaterial;

    // Simple structure to hold backward and forward irradiances
    struct SegmentIrradiance
    {
        double E_f{0.0};
        double E_b{0.0};
    };

    ////////////////////////////////////////////////////////////////////
    // SlatSegments
    ////////////////////////////////////////////////////////////////////

    //! Holds mappings for the slats. Used for mapping between view factors and energy matrix
    //! calculation. For more information about please refer to the Figure called "Segments of the
    //! enclosure and their indexes" in the "Venetian Technical Documentation" document.
    //!
    //! @numberOfSegments Number of segments in the cell. The number of segments do not include the
    //! entire enclosure but only the opening for the incoming beam plus one set of slats. If for
    //! example venetian slat is divided into five segments, this number will be equal to six
    //! (including that extra segment for the incoming beam).
    //! @backSideMeshIndex In single slat enclosure the segments are numbered in the clockwise
    //! direction. In case of single slat is divided into five segments, the total number of
    //! segments in the enclosure will be equal to twelve (top segments, bottom segments and two
    //! openings). This vector will contain indexes for the back side of the slat segments. In the
    //! described this vector will contain the numbers (1, 2, 3, 4, 5).
    //! @frontSideMeshIndex In single slat enclosure the segments are numbered in the clockwise
    //! direction. In case of single slat is divided into five segments, the total number of
    //! segments in the enclosure will be equal to twelve (top segments, bottom segments and two
    //! openings). This vector will contain indexes for the front side of the slat segments. In
    //! the described this vector will contain the numbers (11, 10, 9, 8, 7).
    //! @slatsViewFactorsMatrix View factors matrix is valid for any incoming direction, it depends
    //! on the geometry and will be calculated only once and stored into slatsViewFactorsMatrix
    //! field. Please refer to the "Venetian Technical Documentation" document (Chapter called
    //! Dif-Dif portion) for more information.
    class SlatSegmentsMesh
    {
    public:
        explicit SlatSegmentsMesh(
          CVenetianCellDescription & cell, double Tf, double Tb, double Rf, double Rb);
        SlatSegmentsMesh() = default;
        size_t numberOfSegments{0u};
        std::vector<size_t> backSideMeshIndex;
        std::vector<size_t> frontSideMeshIndex;
        FenestrationCommon::SquareMatrix slatsViewFactorsMatrix;

    private:
        std::vector<size_t> formFrontSegmentsNumbering(size_t numberOfSegments);
        std::vector<size_t> formBackSegmentsNumbering(size_t numberOfSegments);

        //! View factors matrix is valid for any incoming direction, it depends on the geometry and
        //! will be calculated only once and stored into slatsViewFactorsMatrix field
        FenestrationCommon::SquareMatrix
          formViewFactorsMatrix(FenestrationCommon::SquareMatrix && viewFactors,
                                double Tf,
                                double Tb,
                                double Rf,
                                double Rb);
    };

    // Keeping intermediate results for backward and forward directions.
    class CVenetianCellEnergy
    {
    public:
        CVenetianCellEnergy();
        CVenetianCellEnergy(const std::shared_ptr<CVenetianCellDescription> & t_Cell,
                            double Tf,
                            double Tb,
                            double Rf,
                            double Rb);

        double T_dir_dir(const CBeamDirection & t_Direction);
        double T_dir_dif(const CBeamDirection & t_Direction);
        double R_dir_dif(const CBeamDirection & t_Direction);

        double T_dir_dir(const CBeamDirection & t_IncomingDirection,
                         const CBeamDirection & t_OutgoingDirection);
        double R_dir_dir(const CBeamDirection & t_IncomingDirection,
                         const CBeamDirection & t_OutgoingDirection);

        double T_dif_dif();
        double R_dif_dif();

    private:
        //! Keeps information about beam view factor and percentage view. Incoming beam value is
        //! normalized to one.
        //! @viewFactor Fraction of the incoming beam that slat is being hit with
        //! @percentViewed Incoming beam will not always fully hit the slat. This is the percentage
        //! of the slat that actually is being hit by the beam.
        struct BeamSegmentView
        {
            BeamSegmentView() : viewFactor(0), percentViewed(0)
            {}
            double viewFactor;
            double percentViewed;
        };

        // Irradiances for given incoming direction
        std::vector<SegmentIrradiance>
          slatIrradiances(const CBeamDirection & t_IncomingDirection,
                          const SlatSegmentsMesh & slats,
                          const FenestrationCommon::SquareMatrix & energy);

        // Radiances for given incoming direction
        std::vector<double> slatRadiances(const CBeamDirection & t_IncomingDirection,
                                          const SlatSegmentsMesh & slats,
                                          const FenestrationCommon::SquareMatrix & energy);

        // Creates diffuse to diffuse std::vector. Right hand side of the equation
        std::vector<double> diffuseVector(const SlatSegmentsMesh & slats,
                                          FenestrationCommon::SquareMatrix && viewFactors);

        //! Create beam view factors for given incoming direction and side. For details on what beam
        //! view factors are see BeamSegmentView structure.
        //! @param t_Direction Incoming direction of the beam.
        //! @param t_Side Side of the cell.
        //! @return Vector of beam view factors for each segment.
        std::vector<BeamSegmentView> beamVector(const CBeamDirection & t_Direction,
                                                FenestrationCommon::Side t_Side);

        std::shared_ptr<CVenetianCellDescription> m_Cell;
        double m_Tf;
        double m_Tb;
        double m_Rf;
        double m_Rb;

        SlatSegmentsMesh m_SlatSegments;

        std::map<CBeamDirection, std::vector<SegmentIrradiance>> m_SlatIrradiances;
        std::map<CBeamDirection, std::vector<double>> m_SlatRadiances;
    };

    class CVenetianEnergy
    {
    public:
        CVenetianEnergy();
        CVenetianEnergy(const CMaterial & t_Material,
                        const std::shared_ptr<CVenetianCellDescription> & t_ForwardFlowGeometry,
                        const std::shared_ptr<CVenetianCellDescription> & t_BackwardFlowGeometry);

        CVenetianEnergy(double Tf,
                        double Tb,
                        double Rf,
                        double Rb,
                        const std::shared_ptr<CVenetianCellDescription> & t_ForwardFlowGeometry,
                        const std::shared_ptr<CVenetianCellDescription> & t_BackwardFlowGeometry);

        [[nodiscard]] CVenetianCellEnergy & getCell(FenestrationCommon::Side t_Side);

    private:
        // construction of forward and backward cells from both constructors have identical part of
        // the code
        void createForwardAndBackward(
          double Tf,
          double Tb,
          double Rf,
          double Rb,
          const std::shared_ptr<CVenetianCellDescription> & t_ForwardFlowGeometry,
          const std::shared_ptr<CVenetianCellDescription> & t_BackwardFlowGeometry);

        std::map<FenestrationCommon::Side, CVenetianCellEnergy> m_CellEnergy;
    };
}   // namespace SingleLayerOptics