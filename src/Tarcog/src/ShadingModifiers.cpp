#include <cmath>

#include <WCECommon.hpp>

#include "ShadingModifiers.hpp"

namespace Tarcog::ISO15099 {

    double
      computeHcRatio(double hc_env, double T_env, double T_shade_to_env, double T_glass_to_shade)
    {
        const double numerator = std::abs(T_env - T_shade_to_env);
        const double denominator = std::abs(T_env - T_glass_to_shade);

        if(denominator == 0.0)
        {
            return 1.0;   // Avoid division by zero — could also log or assert
        }

        return hc_env * numerator / denominator;
    }

    double computeEmissivityRatio(double R_shade_to_env,
                                  double R_glass_to_shade,
                                  double T_glass_to_shade,
                                  double tau_shade)
    {
        const double numerator = R_shade_to_env + R_glass_to_shade * tau_shade;
        const double denominator = ConstantsData::STEFANBOLTZMANN * std::pow(T_glass_to_shade, 4);

        if(denominator == 0.0)
        {
            return std::numeric_limits<double>::infinity();
        }

        return numerator / denominator;
    }

}   // namespace Tarcog::ISO15099
