#pragma once

#include "CellDescription.hpp"

namespace SingleLayerOptics
{
    // Cell description that needs to be used for perfect diffusers. Specular components are
    // set to zero
    class CFlatCellDescription : public ICellDescription
    {
    public:
        ~CFlatCellDescription() override = default;
        CFlatCellDescription() = default;

        double Beam_dir_dir(const FenestrationCommon::Side t_Side,
                            const CBeamDirection & t_Direction) override;
    };

}   // namespace SingleLayerOptics
