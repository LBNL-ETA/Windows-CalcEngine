#include "SpecularCellDescription.hpp"
#include "WCECommon.hpp"

using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    CSpecularCellDescription::CSpecularCellDescription()
    {}

    double CSpecularCellDescription::Beam_dir_dir(const Side, const CBeamDirection &)
    {
        return 0;
    }

    double CSpecularCellDescription::BeamR_dir_dir(const Side, const CBeamDirection &)
    {
        return 0;
    }

    double CSpecularCellDescription::Rspecular(const Side, const CBeamDirection &)
    {
        return 0;
    }

}   // namespace SingleLayerOptics
