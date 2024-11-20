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

    Viewer::CGeometry2D buildViewerSlat(const FenestrationCommon::Venetian::Geometry & t_Geometry,
                                        size_t t_NumOfSegments,
                                        SegmentsDirection t_Direction);
}   // namespace SingleLayerOptics
