#pragma once

namespace FenestrationCommon
{
    //! \brief Structure that holds geometry of venetian blind slats. To be used across different
    //! calculation modules
    struct VenetianGeometry
    {
        double SlatWidth{0.0};
        double SlatSpacing{0.0};
        double SlatTiltAngle{0.0};
        double CurvatureRadius{0.0};
    };

    VenetianGeometry adjustSlatTiltAngle(const VenetianGeometry & t_Geometry);
}   // namespace FenestrationCommon