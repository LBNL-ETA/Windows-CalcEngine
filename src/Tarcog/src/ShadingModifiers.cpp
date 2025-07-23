#include <cmath>

#include <WCECommon.hpp>

#include "ShadingModifiers.hpp"

namespace Tarcog::ISO15099
{

    double computeHcRatio(
      double hc_env, double T_env, double T_shade_to_env, double T_glass_to_shade, double qv)
    {
        const double delta_glass = T_glass_to_shade - T_env;

        if(std::abs(delta_glass) < 1e-8)
        {
            return 1.0;   // Avoid division by zero
        }

        const double hc_new = hc_env * (T_shade_to_env - T_env) / delta_glass - qv / delta_glass;

        return hc_new / hc_env;
    }


    double computeEmissivityRatio(double glassEmissivity,
                                  double R_shade,
                                  double R_prev,
                                  double T_glass,
                                  double tau_shade,
                                  double tau_opposite,
                                  double G_env)
    {
        using namespace FenestrationCommon;

        const double numerator = R_shade - tau_shade * R_prev - (1.0 - tau_opposite) * G_env;
        const double denominator = ConstantsData::STEFANBOLTZMANN * std::pow(T_glass, 4) - G_env;

        if(std::abs(denominator) < 1e-6)
        {
            return std::numeric_limits<double>::infinity();
        }

        return numerator / denominator / glassEmissivity;
    }

}   // namespace Tarcog::ISO15099
