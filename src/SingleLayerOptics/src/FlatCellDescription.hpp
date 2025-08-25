#ifndef PERFECTDIFFUSECELLDESRIPTION_H
#define PERFECTDIFFUSECELLDESRIPTION_H

#include "CellDescription.hpp"

namespace SingleLayerOptics
{
    // Cell descritpion that needs to be used for perfect diffusers. Specular components are
    // set to zero
    class CFlatCellDescription : public ICellDescription
    {
    public:
        virtual ~CFlatCellDescription() = default;
        CFlatCellDescription();

        double Beam_dir_dir(const FenestrationCommon::Side t_Side, const CBeamDirection & t_Direction);
        double BeamR_dir_dir(const FenestrationCommon::Side t_Side, const CBeamDirection & t_Direction);
    };

}   // namespace SingleLayerOptics

#endif
