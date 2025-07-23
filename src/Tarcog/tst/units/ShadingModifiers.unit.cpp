#include <gtest/gtest.h>
#include <cmath>
#include <WCETarcog.hpp>

namespace Tarcog::ISO15099
{

    TEST(ShadingModifierCalculator, ComputesCorrectRatiosFromTarcogDebugOutput)
    {
        // Inputs extracted from TARCOG debug output for the interior shading case
        constexpr double Gin = 424.458715;
        constexpr double Gout = 240.293669;
        constexpr double T_env = 294.149994;              // Indoor air temperature (K)
        constexpr double T_shade_to_env = 287.049177;     // Theta 4
        constexpr double T_glass_to_shade = 259.102097;   // Theta 2
        constexpr double hc_env = 2.370916;               // hcin

        constexpr double R_shade_to_env = 389.727005;     // Rb 2
        constexpr double R_glass_to_shade = 395.133494;   // Rb 1
        constexpr double tau_shade = 0.39599434;          // tir
        constexpr double tau_glass = 0.0;
        constexpr double glass_emiss = 3.879854455590248e-2;
        constexpr double qv = 47.62064;

        // Compute actual results
        const double hc_ratio = computeHcRatio(hc_env, T_env, T_shade_to_env, T_glass_to_shade, qv);
        const double emissivity_ratio = computeEmissivityRatio(
          glass_emiss, R_shade_to_env, Gout, T_glass_to_shade, tau_glass, tau_glass, Gin);


        constexpr double expected_hc_ratio = 0.775686;
        constexpr double expected_emissivity_ratio = 5.299198;
        // Compare with known values using tolerances
        EXPECT_NEAR(hc_ratio, expected_hc_ratio, 1e-6);
        EXPECT_NEAR(emissivity_ratio, expected_emissivity_ratio, 1e-6);
    }

}   // namespace Tarcog::ISO15099
