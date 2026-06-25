#pragma once

#include <cstddef>
#include <set>
#include <vector>

#include <WCECommon.hpp>


namespace Viewer
{
    // Least-squares view-factor smoothing - a port of conrad's VIEWR (ism == 1). For each CLOSED
    // enclosure it adjusts the view factors so every row sums to one, while preserving symmetry
    // (reciprocity) and the zero diagonal and perturbing the values as little as possible. Open
    // (auto) enclosures listed in openEnclosureIds are left untouched: their row deficit is the
    // view factor to the environment and must not be smoothed away. Returns a new matrix; the
    // input is unchanged.
    [[nodiscard]] FenestrationCommon::SquareMatrix
    smoothViewFactors(const FenestrationCommon::SquareMatrix & viewFactors,
                      const std::vector<double> & areas,
                      const std::vector<std::size_t> & enclosureIds,
                      const std::set<std::size_t> & openEnclosureIds);
}   // namespace Viewer
