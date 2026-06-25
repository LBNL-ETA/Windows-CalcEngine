#pragma once

#include <cstddef>
#include <vector>

#include "Point2D.hpp"
#include "Segment2D.hpp"
#include "ViewSegment2D.hpp"   // Shadowing / PointPosition enums

// Pure free-function form of the 2D view-factor geometry, lifted out of CViewSegment2D so the
// new enclosure engine can use it without the CViewSegment2D : CSegment2D inheritance. Each
// function is a faithful transcription of the corresponding CViewSegment2D method; the
// ViewFactorMath parity tests lock them together.
namespace Viewer::detail
{
    // Position of a point relative to a segment's outward view direction
    // (CViewSegment2D::position).
    [[nodiscard]] PointPosition pointPosition(const CSegment2D & segment, const CPoint2D & point);

    // Mutual self-shadowing classification between two segments
    // (CViewSegment2D::selfShadowing).
    [[nodiscard]] Shadowing selfShadow(const CSegment2D & segmentA, const CSegment2D & segmentB);

    // Cross-string (Hottel) view-factor coefficient; divide by twice the segment length to get
    // the view factor (CViewSegment2D::viewFactorCoefficient).
    [[nodiscard]] double crossStringCoefficient(const CSegment2D & segmentA,
                                                const CSegment2D & segmentB);

    // Splits a segment into count equal sub-segments (CViewSegment2D::subSegments). Throws if
    // count is zero.
    [[nodiscard]] std::vector<CSegment2D> subdivide(const CSegment2D & segment, std::size_t count);
}   // namespace Viewer::detail
