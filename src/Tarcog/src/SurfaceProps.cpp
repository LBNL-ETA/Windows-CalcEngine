#include <algorithm>

#include "SurfaceProps.hpp"

namespace Tarcog::ISO15099
{
    ThermochromicSurfaceProps::ThermochromicSurfaceProps(
      const std::vector<FenestrationCommon::TableValue> & e,
      const std::vector<FenestrationCommon::TableValue> & t) :
        emissivity_curve(e), transmittance_curve(t)
    {}

    double emissivity(const SurfaceProps & p, double T)
    {
        return std::visit(
          [&](const auto & s) -> double {
              using S = std::decay_t<decltype(s)>;
              if constexpr(std::is_same_v<S, ConstantSurfaceProps>)
              {
                  return s.emissivity;
              }
              else
              {
                  return s.emissivity_curve.value(T);
              }
          },
          p);
    }

    double transmittance(const SurfaceProps & p, double T)
    {
        return std::visit(
          [&](const auto & s) -> double {
              using S = std::decay_t<decltype(s)>;
              if constexpr(std::is_same_v<S, ConstantSurfaceProps>)
              {
                  return s.transmittance;
              }
              else
              {
                  return s.transmittance_curve.value(T);
              }
          },
          p);
    }

}   // namespace Tarcog::ISO15099
