#pragma once

#include <cstddef>
#include <optional>
#include <set>

#include "Point2D.hpp"
#include "ViewerConstants.hpp"

namespace Viewer
{
    // A single radiation surface segment that participates in an enclosure balance. Segments
    // are grouped into enclosures by enclosureId; segments with different ids never exchange
    // radiation. The emissivity is carried through for the downstream radiosity solve and is
    // not used by the view-factor computation itself.
    struct RadiationSegment
    {
        CPoint2D startPoint;
        CPoint2D endPoint;
        double emissivity{0.9};
        std::size_t enclosureId{0};
    };

    // A surface that can block the view between two radiation segments but is not itself a
    // radiation surface (the legacy "blocking surface"). Radiation segments also act as
    // blockers; this type is for additional obstacles.
    struct BlockingSegment
    {
        CPoint2D startPoint;
        CPoint2D endPoint;
    };

    // Tuning for computeEnclosureViewFactors. The defaults reproduce the legacy behavior:
    // closed enclosures, no smoothing, and the legacy subdivision count.
    struct ViewFactorOptions
    {
        // Apply least-squares view-factor smoothing to closed enclosures (the GUI checkbox).
        bool leastSquaresSmoothing{false};

        // Enclosure ids that are open to an environment (auto-enclosure). Their per-row deficit is
        // the view factor to that environment, so they are excluded from smoothing. Closed by
        // default (empty set).
        std::set<std::size_t> openEnclosureIds{};

        // Number of sub-segments used when a pair is partially blocked.
        std::size_t subdivision{static_cast<std::size_t>(ViewerConstants::NUM_OF_SEGMENTS)};

        // Spatial-grid density; empty means auto-pick from the blocker count.
        std::optional<std::size_t> gridCellsPerAxis{};

        // Parallelize the pair loop; false forces a deterministic serial run.
        bool multithread{true};
    };
}   // namespace Viewer
