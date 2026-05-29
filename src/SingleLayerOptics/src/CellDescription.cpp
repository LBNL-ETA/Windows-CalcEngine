#include "CellDescription.hpp"
#include "BeamDirection.hpp"

namespace SingleLayerOptics
{
    double Beam_dir_dir(CellDescription & description,
                        const FenestrationCommon::Side t_Side,
                        const CBeamDirection & t_Direction)
    {
        return std::visit([&](auto & cell) { return cell.Beam_dir_dir(t_Side, t_Direction); },
                          description);
    }
}   // namespace SingleLayerOptics
