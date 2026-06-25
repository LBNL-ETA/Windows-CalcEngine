#pragma once

#include <vector>

#include <WCECommon.hpp>

#include "EnclosureViewFactorInput.hpp"

namespace Viewer
{
    // Result of computeEnclosureViewFactors. The matrix is block-diagonal over enclosures,
    // sized to all radiation segments and indexed in input order; cross-enclosure entries are
    // exactly zero. rowSumError holds the per-row (1 - row sum) residual before closure, kept
    // for diagnostics and tests.
    struct EnclosureViewFactorResult
    {
        FenestrationCommon::SquareMatrix viewFactors;
        std::vector<double> rowSumError;
    };

    // Computes radiation view factors for one or more closed enclosures. Pure: it owns no
    // state and caches nothing. Segments are matched into enclosures by RadiationSegment's
    // enclosureId, and the result is indexed in the order the segments are supplied.
    [[nodiscard]] EnclosureViewFactorResult
    computeEnclosureViewFactors(const std::vector<RadiationSegment> & segments,
                                const std::vector<BlockingSegment> & blockers,
                                const ViewFactorOptions & options = {});
}   // namespace Viewer
