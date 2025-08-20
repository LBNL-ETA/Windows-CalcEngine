#pragma once

namespace FenestrationCommon
{
    enum class Side;
}

namespace SingleLayerOptics
{
    class CBeamDirection;

    // Interface for describing a single optical cell within a window layer.
    // Multiple cells are combined to form the complete window layer.
    // Each cell must provide methods to calculate direct-to-direct transmittance and reflectance
    // for a specified incoming direction and side. These calculations are essential for determining
    // the overall optical properties of the window system.
    class ICellDescription
    {
    public:
        virtual ~ICellDescription() = default;
        ICellDescription() = default;

        virtual double T_dir_dir(const FenestrationCommon::Side t_Side,
                                 const CBeamDirection & t_Direction) = 0;
        virtual double R_dir_dir(const FenestrationCommon::Side t_Side,
                                 const CBeamDirection & t_Direction) = 0;
    };
}   // namespace SingleLayerOptics
