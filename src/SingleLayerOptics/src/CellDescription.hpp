#pragma once

#include <variant>

#include "SpecularCellDescription.hpp"
#include "FlatCellDescription.hpp"
#include "VenetianCellDescription.hpp"
#include "PerforatedCellDescription.hpp"
#include "WovenCellDescription.hpp"

namespace FenestrationCommon
{
    enum class Side;
}

namespace SingleLayerOptics
{
    class CBeamDirection;

    // A cell description is one of a closed set of geometries. Previously these were subclasses of
    // an ICellDescription interface held through a shared_ptr; they are now value types collected in
    // a variant so cells can store the description directly (no inheritance, no heap allocation).
    using CellDescription = std::variant<CSpecularCellDescription,
                                         CFlatCellDescription,
                                         CVenetianCellDescription,
                                         CCircularCellDescription,
                                         CRectangularCellDescription,
                                         CWovenCellDescription>;

    // Direct-to-direct beam fraction through the cell geometry. Dispatches over the active
    // alternative, replacing the former ICellDescription::Beam_dir_dir virtual call.
    double Beam_dir_dir(CellDescription & description,
                        FenestrationCommon::Side t_Side,
                        const CBeamDirection & t_Direction);
}   // namespace SingleLayerOptics
