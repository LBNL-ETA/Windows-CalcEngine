#pragma once

namespace FenestrationCommon::LouveredShutter
{
    //! \brief Structure that holds geometry of louvered shutter.
    struct Geometry
    {
        double SlatWidth{0.0};
        double SlatThickness{0.0};
        double SlatAngle{0.0};
        double SlatSpacing{0.0};
    };
}   // namespace FenestrationCommon