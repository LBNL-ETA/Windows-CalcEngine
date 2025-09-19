#include <algorithm>

#include "SurfaceProps.hpp"

namespace Tarcog::ISO15099
{
    ThermochromicSurfaceProps::ThermochromicSurfaceProps(
      const std::vector<FenestrationCommon::TableValue> & emissivity_by_T,
      const std::vector<FenestrationCommon::TableValue> & transmittance_by_T) :
        emissivity_curve(emissivity_by_T), transmittance_curve(transmittance_by_T)
    {}

    template<auto ConstantMember, auto CurveMember>
    double eval_prop(const SurfaceProps & props, double temperature)
    {
        struct Visitor
        {
            double operator()(const ConstantSurfaceProps & constant) const
            {
                return constant.*ConstantMember;
            }
            double operator()(const ThermochromicSurfaceProps & thermo) const
            {
                return (thermo.*CurveMember).value(temperature);
            }
            double temperature;
        };
        return std::visit(Visitor{temperature}, props);
    }

    double emissivity(const SurfaceProps & props, double temperature)
    {
        return eval_prop<&ConstantSurfaceProps::emissivity,
                         &ThermochromicSurfaceProps::emissivity_curve>(props, temperature);
    }

    double transmittance(const SurfaceProps & props, double temperature)
    {
        return eval_prop<&ConstantSurfaceProps::transmittance,
                         &ThermochromicSurfaceProps::transmittance_curve>(props, temperature);
    }

}   // namespace Tarcog::ISO15099
