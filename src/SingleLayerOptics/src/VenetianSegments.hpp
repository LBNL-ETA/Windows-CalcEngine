#pragma once

namespace Viewer
{
    struct BeamViewFactor;
    class CGeometry2D;
}   // namespace Viewer

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

    struct SegmentIndexes
    {
        std::vector<size_t> backSideMeshIndex;
        std::vector<size_t> frontSideMeshIndex;
    };

    struct LayerProperties
    {
        double Tf{0.0};
        double Rf{0.0};
        double Tb{0.0};
        double Rb{0.0};
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
    //! @slatsRadiancesMatrix Slat radiances matrix is valid for diffuse to diffuse, it depends
    //! on the geometry and will be calculated only once and stored into slatsViewFactorsMatrix
    //! field. Please refer to the "Venetian Technical Documentation" document (Chapter called
    //! Dif-Dif portion) for more information.
    class SlatSegmentsMesh
    {
    public:
        explicit SlatSegmentsMesh(CVenetianCellDescription & cell);
        SlatSegmentsMesh() = default;

        size_t numberOfSegments{0u};
        SegmentIndexes surfaceIndexes;

    private:
        std::vector<size_t> formFrontSegmentsNumbering(size_t numberOfSegments);
        std::vector<size_t> formBackSegmentsNumbering(size_t numberOfSegments);
    };

    // Creates diffuse to diffuse std::vector. Right hand side of the equation
    std::vector<double> diffuseRadiosities(size_t numberOfSegments,
                                           const SegmentIndexes & indexes,
                                           const FenestrationCommon::SquareMatrix & viewFactors);

    //! Function to calculate view factors that are used in beam to diffuse calculations
    std::vector<double> beamToDiffuseViewFactors(FenestrationCommon::Side t_Side,
                                                 const CBeamDirection & t_IncomingDirection,
                                                 CVenetianCellDescription & cell,
                                                 const SlatSegmentsMesh & mesh);

    // Irradiances for given incoming direction
    std::vector<SegmentIrradiance>
      slatIrradiances(const std::vector<double> & beamViewFactors,
                      const FenestrationCommon::SquareMatrix & radianceMatrix,
                      const SlatSegmentsMesh & mesh);

    std::vector<double>
      directUniformSlatRadiances(const std::vector<SegmentIrradiance> & vector,
                                 const FenestrationCommon::SquareMatrix & radiancesMatrix,
                                 const LayerProperties & properties);

    // Radiances for given incoming direction
    std::vector<double>
      directToDiffuseSlatRadiances(const std::vector<SegmentIrradiance> & slatIrradiances,
                                   const SlatSegmentsMesh & slats,
                                   const LayerProperties & layerProperties);

    // Keeping intermediate results for backward and forward directions.
    class CVenetianCellEnergy
    {
    public:
        CVenetianCellEnergy();
        CVenetianCellEnergy(const std::shared_ptr<CVenetianCellDescription> & t_Cell,
                            const LayerProperties & layerProperties);

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
        //! View factors matrix is valid for any incoming direction, it depends on the geometry and
        //! will be calculated only once and stored into slatsViewFactorsMatrix field
        FenestrationCommon::SquareMatrix
          formIrradianceMatrix(const FenestrationCommon::SquareMatrix & viewFactors,
                               const LayerProperties & properties);

        //! Function to calculate direct to direct slat irradiances needed for directional
        //! calculations
        //! @param t_IncomingDirection Incoming direction of the beam
        //! @return std::vector of irradiances for each slat segment (Note that this does not
        //! include inside and outside segments, meaning if number of slats mesh is 5 then size of
        //! this vector will be 5)
        std::vector<SegmentIrradiance>
          directToDirectSlatIrradiances(const CBeamDirection & t_IncomingDirection);

        std::shared_ptr<CVenetianCellDescription> m_Cell;
        LayerProperties m_LayerProperties;

        SlatSegmentsMesh m_SlatSegmentsMesh;

        //! This matrix is precalculated for the diffuse to diffuse part of the calculation.
        //! It is used to calculate the radiances for the slats and it is precalculated because it
        //! saves time (around 10% faster for the unit tests run).
        FenestrationCommon::SquareMatrix slatsDiffuseRadiancesMatrix;

        std::map<CBeamDirection, std::vector<SegmentIrradiance>> m_DirectToDiffuseSlatIrradiances;
        std::map<CBeamDirection, std::vector<double>> m_DirectToDiffuseSlatRadiances;

        std::map<CBeamDirection, std::vector<SegmentIrradiance>> m_DirectToDirectSlatIrradiances;
        std::map<CBeamDirection, std::vector<double>> m_DirectToDirectSlatRadiances;
    };

    class CVenetianEnergy
    {
    public:
        CVenetianEnergy();
        CVenetianEnergy(const CMaterial & t_Material,
                        const std::shared_ptr<CVenetianCellDescription> & t_ForwardFlowGeometry,
                        const std::shared_ptr<CVenetianCellDescription> & t_BackwardFlowGeometry);

        CVenetianEnergy(const LayerProperties & layerProperties,
                        const std::shared_ptr<CVenetianCellDescription> & t_ForwardFlowGeometry,
                        const std::shared_ptr<CVenetianCellDescription> & t_BackwardFlowGeometry);

        [[nodiscard]] CVenetianCellEnergy & getCell(FenestrationCommon::Side t_Side);

    private:
        // construction of forward and backward cells from both constructors have identical part of
        // the code
        void createForwardAndBackward(
          const LayerProperties & layerProperties,
          const std::shared_ptr<CVenetianCellDescription> & t_ForwardFlowGeometry,
          const std::shared_ptr<CVenetianCellDescription> & t_BackwardFlowGeometry);

        std::map<FenestrationCommon::Side, CVenetianCellEnergy> m_CellEnergy;
    };
}   // namespace SingleLayerOptics