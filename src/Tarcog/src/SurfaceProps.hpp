#pragma once
#include <variant>
#include <vector>
#include <WCECommon.hpp>

namespace Tarcog::ISO15099
{

    struct ConstantSurfaceProps
    {
        double emissivity;      // constant over T
        double transmittance;   // constant over T
    };

    struct ThermochromicSurfaceProps
    {
        FenestrationCommon::SPChipInterpolation2D emissivity_curve;
        FenestrationCommon::SPChipInterpolation2D transmittance_curve;

        ThermochromicSurfaceProps(
          const std::vector<FenestrationCommon::TableValue> & emissivity_by_T,
          const std::vector<FenestrationCommon::TableValue> & transmittance_by_T);
    };

    using SurfaceProps = std::variant<ConstantSurfaceProps, ThermochromicSurfaceProps>;

    double emissivity(const SurfaceProps & p, double T);
    double transmittance(const SurfaceProps & p, double T);

}   // namespace Tarcog::ISO15099
