#ifndef SPECULARCELLDESCRIPTION_H
#define SPECULARCELLDESCRIPTION_H

#include "CellDescription.hpp"

namespace FenestrationCommon
{
    enum class Side;
    enum class MaterialType;

}   // namespace FenestrationCommon

namespace SingleLayerOptics
{
    class CSpecularCellDescription : public ICellDescription
    {
    public:
        virtual ~CSpecularCellDescription() = default;
        CSpecularCellDescription();

        double Beam_dir_dir(const FenestrationCommon::Side t_Side,
                            const CBeamDirection & t_Direction) override;
        double Rspecular(const FenestrationCommon::Side t_Side, const CBeamDirection & t_Direction);
    };

}   // namespace SingleLayerOptics

#endif
