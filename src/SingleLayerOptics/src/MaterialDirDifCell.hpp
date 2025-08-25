#pragma once

#include <memory>
#include <vector>

#include "BaseCell.hpp"
#include "DirectionalDiffuseCell.hpp"

namespace FenestrationCommon
{
    enum class Side;
}

namespace SingleLayerOptics
{
    class ICellDescription;
    class CBeamDirection;
    class CMaterial;

    class CMaterialDirectionalDiffuseCell : public CDirectionalDiffuseCell
    {
    public:
        CMaterialDirectionalDiffuseCell(const std::shared_ptr<CMaterial> & t_MaterialProperties,
                                        const std::shared_ptr<ICellDescription> & t_Cell);

        double T_dir_dir(FenestrationCommon::Side t_Side,
                         const CBeamDirection & t_Direction) override;
        double R_dir_dir(FenestrationCommon::Side t_Side,
                         const CBeamDirection & t_Direction) override;

        std::vector<double> T_dir_dir_band(FenestrationCommon::Side t_Side,
                                           const CBeamDirection & t_Direction) override;

        double T_dir_dir_at_wavelength(FenestrationCommon::Side t_Side,
                                       const CBeamDirection & t_Direction,
                                       size_t wavelengthIndex) override;

        std::vector<double> R_dir_dir_band(FenestrationCommon::Side t_Side,
                                           const CBeamDirection & t_Direction) override;

        double R_dir_dir_at_wavelength(FenestrationCommon::Side t_Side,
                                       const CBeamDirection & t_Direction,
                                       size_t wavelengthIndex) override;

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
    };

}   // namespace SingleLayerOptics