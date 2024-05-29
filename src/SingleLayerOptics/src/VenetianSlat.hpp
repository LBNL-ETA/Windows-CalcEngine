#pragma once

#include <memory>

#include <WCECommon.hpp>
#include <WCEViewer.hpp>

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
        CVenetianSlat(const FenestrationCommon::VenetianGeometry & t_Geometry,
                      size_t t_NumOfSegments,
                      SegmentsDirection t_Direction);

        // Returns the geometry segments that are slats made of
        [[nodiscard]] Viewer::CGeometry2D geometry() const;

        [[nodiscard]] FenestrationCommon::VenetianGeometry getVenetianGeometry() const;

    private:
        Viewer::CGeometry2D
          buildSlat(const FenestrationCommon::VenetianGeometry & t_Geometry,
                    size_t t_NumOfSegments,
                    SegmentsDirection t_Direction);

        FenestrationCommon::VenetianGeometry m_VenetianGeometry;

        Viewer::CGeometry2D m_Geometry;
    };

}   // namespace SingleLayerOptics
