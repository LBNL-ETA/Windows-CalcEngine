#pragma once

namespace FenestrationCommon::Venetian
{
    //! \brief Structure that holds geometry of venetian blind slats. To be used across different
    //! calculation modules
    struct Geometry
    {
        double SlatWidth{0.0};
        double SlatSpacing{0.0};
        double SlatTiltAngle{0.0};
        double CurvatureRadius{0.0};
    };

    Geometry adjustSlatTiltAngle(const Geometry & t_Geometry);

    //! \brief Calculates geometrical thickness of venetian blind slats
    [[nodiscard]] double thickness(const Geometry & t_Geometry);
}   // namespace FenestrationCommon