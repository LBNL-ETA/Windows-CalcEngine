#pragma once

#include <memory>

#include <WCEViewer.hpp>

namespace Viewer
{
    class CSegment2D;

}   // namespace Viewer

namespace SingleLayerOptics
{
    enum class SegmentsDirection
    {
        Positive,
        Negative
    };

    class CVenetianSlat
    {
    public:
        CVenetianSlat(double t_SlatWidth,
                      double t_SlatSpacing,
                      double t_SlatTiltAngle,
                      double t_CurvatureRadius,
                      size_t t_NumOfSegments,
                      SegmentsDirection t_Direction);

        // Returns the geometry segments that are slats made of
        [[nodiscard]] Viewer::CGeometry2D geometry() const;

        [[nodiscard]] double slatWidth() const;
        [[nodiscard]] double slatSpacing() const;
        [[nodiscard]] double slatTiltAngle() const;
        [[nodiscard]] double curvatureRadius() const;
        [[nodiscard]] size_t numberOfSegments() const;

    private:
        void buildSlat();

        double m_SlatWidth;
        double m_SlatSpacing;
        double m_SlatTiltAngle;
        double m_CurvatureRadius;
        size_t m_NumOfSlatSegments;
        SegmentsDirection m_Direction;

        Viewer::CGeometry2D m_Geometry;
    };

}   // namespace SingleLayerOptics
