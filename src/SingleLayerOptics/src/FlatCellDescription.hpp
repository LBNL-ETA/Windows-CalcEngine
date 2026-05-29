#pragma once

namespace FenestrationCommon
{
    enum class Side;
}

namespace SingleLayerOptics
{
    class CBeamDirection;

    // Cell description that needs to be used for perfect diffusers. Specular components are
    // set to zero
    class CFlatCellDescription
    {
    public:
        CFlatCellDescription() = default;

        double Beam_dir_dir(FenestrationCommon::Side t_Side, const CBeamDirection & t_Direction);
    };

}   // namespace SingleLayerOptics
