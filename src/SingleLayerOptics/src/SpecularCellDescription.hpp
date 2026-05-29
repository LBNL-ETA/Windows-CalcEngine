#ifndef SPECULARCELLDESCRIPTION_H
#define SPECULARCELLDESCRIPTION_H

namespace FenestrationCommon
{
    enum class Side;
    enum class MaterialType;

}   // namespace FenestrationCommon

namespace SingleLayerOptics
{
    class CBeamDirection;

    class CSpecularCellDescription
    {
    public:
        CSpecularCellDescription() = default;

        double Beam_dir_dir(FenestrationCommon::Side t_Side, const CBeamDirection & t_Direction);
        double Rspecular(FenestrationCommon::Side t_Side, const CBeamDirection & t_Direction);
    };

}   // namespace SingleLayerOptics

#endif
