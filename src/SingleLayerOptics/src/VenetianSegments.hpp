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

    // Holds mappings for the slats. Used for mapping between view factors and energy matrix.
    class SlatSegments
    {
    public:
        explicit SlatSegments(
          CVenetianCellDescription & cell, double Tf, double Tb, double Rf, double Rb);
        SlatSegments() = default;
        size_t numberOfSegments{0u};
        std::vector<size_t> b;
        std::vector<size_t> f;
        FenestrationCommon::SquareMatrix slatsEnergy;

    private:
        std::vector<size_t> formFrontSegmentsNumbering(size_t numberOfSegments);
        std::vector<size_t> formBackSegmentsNumbering(size_t numberOfSegments);

        // Energy matrix is valid for any incoming direction. Depends on geometry and will be
        // calculated only once and stored into m_Energy field
        FenestrationCommon::SquareMatrix
          formEnergyMatrix(FenestrationCommon::SquareMatrix && viewFactors,
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
        //! Keeps information about beam view factor and percentage view. Incoming beam value is normalized to one.
        //! @viewFactor Fraction of the incoming beam that slat is being hit with
        //! @percentViewed Incoming beam will not always fully hit the slat. This is the percentage of the slat that
        //! actually is being hit by the beam.
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
                          const SlatSegments & slats,
                          const FenestrationCommon::SquareMatrix & energy);

        // Radiances for given incoming direction
        std::vector<double> slatRadiances(const CBeamDirection & t_IncomingDirection,
                                          const SlatSegments & slats,
                                          const FenestrationCommon::SquareMatrix & energy);

        // Creates diffuse to diffuse std::vector. Right hand side of the equation
        std::vector<double> diffuseVector(const SlatSegments & slats,
                                          FenestrationCommon::SquareMatrix && viewFactors);

        //! Create beam view factors for given incoming direction and side. For details on what beam view factors are
        //! see BeamSegmentView structure.
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

        SlatSegments m_SlatSegments;

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
}