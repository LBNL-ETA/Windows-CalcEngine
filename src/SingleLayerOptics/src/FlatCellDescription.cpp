#include "FlatCellDescription.hpp"

using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    CFlatCellDescription::CFlatCellDescription() : ICellDescription()
    {}

    double CFlatCellDescription::Beam_dir_dir(const Side, const CBeamDirection &)
    {
        return 0;
    }

    double CFlatCellDescription::BeamR_dir_dir(const Side, const CBeamDirection &)
    {
        return 0;
    }

}   // namespace SingleLayerOptics
