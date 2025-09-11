#include "FlatCellDescription.hpp"

using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    double CFlatCellDescription::Beam_dir_dir(const Side, const CBeamDirection &)
    {
        return 0;
    }

}   // namespace SingleLayerOptics
