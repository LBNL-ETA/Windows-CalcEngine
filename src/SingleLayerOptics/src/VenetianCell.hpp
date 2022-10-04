#pragma once

#include <memory>
#include <vector>
#include <map>

#include <WCECommon.hpp>

#include "UniformDiffuseCell.hpp"
#include "DirectionalDiffuseCell.hpp"

namespace FenestrationCommon
{
    class SquareMatrix;
    enum class Side;

}   // namespace FenestrationCommon

namespace Viewer
{
    struct BeamViewFactor;
}

namespace SingleLayerOptics
{
    class ICellDescription;
    class CVenetianCellDescription;
    class CBeamDirection;

    class CVenetianBase : public CUniformDiffuseCell, public CDirectionalDiffuseCell
    {
    public:
        CVenetianBase(const std::shared_ptr<CMaterial> & t_MaterialProperties,
                      const std::shared_ptr<ICellDescription> & t_Cell,
                      double rotation = 0);

    protected:
        [[nodiscard]] std::shared_ptr<CVenetianCellDescription> getCellAsVenetian() const;
    };

    // Simple structure to hold backward and forward irradiances
    struct SegmentIrradiance
    {
    public:
        SegmentIrradiance() : E_f(0), E_b(0){};
        double E_f;
        double E_b;
    };

    class CVenetianSlatEnergies
    {
    public:
        CVenetianSlatEnergies(const CBeamDirection & t_BeamDirection,
                              const std::vector<SegmentIrradiance> & t_SlatIrradiances,
                              const std::vector<double> & t_SlatRadiances);

        const SegmentIrradiance & irradiances(const size_t index) const;
        double radiances(const size_t index) const;
        std::shared_ptr<const CBeamDirection> direction() const;
        size_t size() const;

    private:
        // Keep slat energies for certain beam incoming directions
        std::vector<SegmentIrradiance> m_SlatIrradiances;
        std::vector<double> m_SlatRadiances;
        // Direction for which energies are calculated
        std::shared_ptr<CBeamDirection> m_CalcDirection;
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
        // Keeps information about beam view factor and percentage view
        struct BeamSegmentView
        {
            BeamSegmentView() : viewFactor(0), percentViewed(0)
            {}
            double viewFactor;
            double percentViewed;
        };

        // Holds mappings for the slats. Used for mapping between view factors and energy matrix.
        struct SlatSegments
        {
            explicit SlatSegments(CVenetianCellDescription & cell);
            SlatSegments() = default;
            std::vector<size_t> b;
            std::vector<size_t> f;
        };

        // calculate slat irradiances and radiances based on incoming beam
        CVenetianSlatEnergies calculateSlatEnergiesFromBeam(const CBeamDirection & t_Direction,
                                                            CVenetianCellDescription & cell,
                                                            const SlatSegments & slats,
                                                            const FenestrationCommon::SquareMatrix & energy);

        // Irradiances for given incoming direction
        std::vector<SegmentIrradiance>
          slatIrradiances(const CBeamDirection & t_IncomingDirection,
                          CVenetianCellDescription & cell,
                          const SlatSegments & slats,
                          const FenestrationCommon::SquareMatrix & energy);

        // Radiances for given incoming direction
        std::vector<double> slatRadiances(std::vector<SegmentIrradiance> & t_Irradiances,
                                          const SlatSegments & slats);

        // Creates diffuse to diffuse std::vector. Right hand side of the equation
        std::vector<double> diffuseVector(CVenetianCellDescription & cell,
                                          const SlatSegments & slats);

        // Create beam to diffuse std::vector. Right hand side of the equation
        std::vector<BeamSegmentView> beamVector(const CBeamDirection & t_Direction,
                                                FenestrationCommon::Side t_Side);

        std::shared_ptr<CVenetianCellDescription> m_Cell;
        double m_Tf;
        double m_Tb;
        double m_Rf;
        double m_Rb;

        SlatSegments m_SlatSegments;
        FenestrationCommon::SquareMatrix m_Energy;

        // Energy matrix is valid for any incoming direction. Depends on geometry and will be
        // calculated only once and stored into m_Energy field
        FenestrationCommon::SquareMatrix formEnergyMatrix(CVenetianCellDescription & cell,
                                                          const SlatSegments & slats);
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

        [[nodiscard]] CVenetianCellEnergy & getCell(const FenestrationCommon::Side t_Side);

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

    class CVenetianCell : public CVenetianBase
    {
    public:
        CVenetianCell(const std::shared_ptr<CMaterial> & t_MaterialProperties,
                      const std::shared_ptr<ICellDescription> & t_Cell,
                      double rotation = 0);

        void setSourceData(FenestrationCommon::CSeries & t_SourceData) override;

        void setBandWavelengths(const std::vector<double> & wavelengths) override;

        double T_dir_dir(FenestrationCommon::Side t_Side,
                         const CBeamDirection & t_Direction) override;

        std::vector<double> T_dir_dir_band(FenestrationCommon::Side t_Side,
                                           const CBeamDirection & t_Direction) override;

        double T_dir_dir_at_wavelength(FenestrationCommon::Side t_Side,
                                       const CBeamDirection & t_Direction,
                                       size_t wavelengthIndex) override;

        /////////////////////////////////////////////////////////////////////////////////////////////
        // Uniform diffuse components
        /////////////////////////////////////////////////////////////////////////////////////////////
        double T_dir_dif(FenestrationCommon::Side t_Side,
                         const CBeamDirection & t_Direction) override;
        std::vector<double> T_dir_dif_band(FenestrationCommon::Side t_Side,
                                           const CBeamDirection & t_Direction) override;
        double T_dir_dif_at_wavelength(FenestrationCommon::Side t_Side,
                                       const CBeamDirection & t_Direction,
                                       size_t wavelengthIndex) override;

        double R_dir_dif(FenestrationCommon::Side t_Side,
                         const CBeamDirection & t_Direction) override;
        std::vector<double> R_dir_dif_band(FenestrationCommon::Side t_Side,
                                           const CBeamDirection & t_Direction) override;
        double R_dir_dif_at_wavelength(FenestrationCommon::Side t_Side,
                                       const CBeamDirection & t_Direction,
                                       size_t wavelengthIndex) override;

        /////////////////////////////////////////////////////////////////////////////////////////////
        // Directional diffuse components
        /////////////////////////////////////////////////////////////////////////////////////////////
        double T_dir_dif(FenestrationCommon::Side t_Side,
                         const CBeamDirection & t_IncomingDirection,
                         const CBeamDirection & t_OutgoingDirection) override;

        double R_dir_dif(FenestrationCommon::Side t_Side,
                         const CBeamDirection & t_IncomingDirection,
                         const CBeamDirection & t_OutgoingDirection) override;

        std::vector<double> T_dir_dif_band(FenestrationCommon::Side t_Side,
                                           const CBeamDirection & t_IncomingDirection,
                                           const CBeamDirection & t_OutgoingDirection) override;
        double T_dir_dif_by_wavelength(FenestrationCommon::Side t_Side,
                                       const CBeamDirection & t_IncomingDirection,
                                       const CBeamDirection & t_OutgoingDirection,
                                       size_t wavelengthIndex) override;

        std::vector<double> R_dir_dif_band(FenestrationCommon::Side t_Side,
                                           const CBeamDirection & t_IncomingDirection,
                                           const CBeamDirection & t_OutgoingDirection) override;
        double R_dir_dif_by_wavelength(FenestrationCommon::Side t_Side,
                                       const CBeamDirection & t_IncomingDirection,
                                       const CBeamDirection & t_OutgoingDirection,
                                       size_t wavelengthIndex) override;

        // Functions specific only for Venetian cell. Diffuse to diffuse component based only on
        // view factors
        double T_dif_dif(FenestrationCommon::Side t_Side);
        double R_dif_dif(FenestrationCommon::Side t_Side);

    private:
        void generateVenetianEnergy();
        // Energy calculations for whole band
        CVenetianEnergy m_Energy;

        // Energy calculations for material range (wavelengths)
        std::vector<CVenetianEnergy> m_EnergiesBand;

        std::shared_ptr<CVenetianCellDescription> m_BackwardFlowCellDescription;
    };

}   // namespace SingleLayerOptics
