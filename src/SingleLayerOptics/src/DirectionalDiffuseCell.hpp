#pragma once

#include <memory>
#include <vector>

#include "BaseCell.hpp"

namespace FenestrationCommon
{
    enum class Side;
}

namespace SingleLayerOptics
{
    class ICellDescription;
    class CBeamDirection;
    class CMaterial;

    class CDirectionalDiffuseCell : public virtual CBaseCell
    {
    public:
        CDirectionalDiffuseCell(const std::shared_ptr<CMaterial> & t_MaterialProperties,
                                const std::shared_ptr<ICellDescription> & t_Cell,
                                double rotation = 0);

        // dir_dif and dir_dif_band functions are calculating portion of incoming beam that bounced
        // off material It does not include dir_dir portion of the beam that goes through cell (that
        // is calculated with dir_dir functions) (Simon)
        virtual double T_dir_dif(FenestrationCommon::Side t_Side,
                                 const CBeamDirection & t_IncomingDirection,
                                 const CBeamDirection & t_OutgoingDirection);

        virtual double R_dir_dif(FenestrationCommon::Side t_Side,
                                 const CBeamDirection & t_IncomingDirection,
                                 const CBeamDirection & t_OutgoingDirection);

        virtual std::vector<double> T_dir_dif_band(FenestrationCommon::Side t_Side,
                                                   const CBeamDirection & t_IncomingDirection,
                                                   const CBeamDirection & t_OutgoingDirection);

        virtual double T_dir_dif_by_wavelength(FenestrationCommon::Side t_Side,
                                               const CBeamDirection & t_IncomingDirection,
                                               const CBeamDirection & t_OutgoingDirection,
                                               size_t wavelengthIndex);

        virtual std::vector<double> R_dir_dif_band(FenestrationCommon::Side t_Side,
                                                   const CBeamDirection & t_IncomingDirection,
                                                   const CBeamDirection & t_OutgoingDirection);

        virtual double R_dir_dif_by_wavelength(FenestrationCommon::Side t_Side,
                                               const CBeamDirection & t_IncomingDirection,
                                               const CBeamDirection & t_OutgoingDirection,
                                               size_t wavelengthIndex);
    };

}   // namespace SingleLayerOptics
