#pragma once

#include <memory>
#include <vector>
#include <map>

#include <WCECommon.hpp>

#include "VenetianSegments.hpp"
#include "UniformDiffuseCell.hpp"
#include "DirectionalDiffuseCell.hpp"
#include "BeamDirection.hpp"

namespace FenestrationCommon
{
    class SquareMatrix;
    enum class Side;

}   // namespace FenestrationCommon

namespace SingleLayerOptics
{
    class ICellDescription;
    class CVenetianCellDescription;

    class CVenetianBase : public CUniformDiffuseCell, public CDirectionalDiffuseCell
    {
    public:
        CVenetianBase(const std::shared_ptr<CMaterial> & t_MaterialProperties,
                      const std::shared_ptr<ICellDescription> & t_Cell,
                      double rotation = 0);

    protected:
        [[nodiscard]] std::shared_ptr<CVenetianCellDescription> getCellAsVenetian() const;
    };

    class CVenetianCell : public CVenetianBase
    {
    public:
        CVenetianCell(const std::shared_ptr<CMaterial> & t_MaterialProperties,
                      const std::shared_ptr<ICellDescription> & t_Cell,
                      double rotation = 0);

        void setSourceData(const FenestrationCommon::CSeries & t_SourceData) override;

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
