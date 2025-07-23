#include <gtest/gtest.h>
#include <cmath>
#include <WCETarcog.hpp>

namespace Tarcog::ISO15099 {

    TEST(ShadingModifierCalculator, ComputesCorrectRatiosFromTarcogDebugOutput)
    {
        // Inputs extracted from TARCOG debug output for the interior shading case
        constexpr double T_env = 294.149994;               // Indoor air temperature (K)
        constexpr double T_shade_to_env = 287.049177;      // Theta 4
        constexpr double T_glass_to_shade = 259.102097;    // Theta 2
        constexpr double hc_env = 2.370916;                // hcin

        constexpr double R_shade_to_env = 389.727005;      // Rb 2
        constexpr double R_glass_to_shade = 395.133494;    // Rb 1
        constexpr double tau_shade = 0.39599434;           // tir

        // Compute actual results
        const double hc_ratio = computeHcRatio(hc_env, T_env, T_shade_to_env, T_glass_to_shade);
        const double emissivity_ratio = computeEmissivityRatio(
            R_shade_to_env, R_glass_to_shade, T_glass_to_shade, tau_shade
        );


        constexpr double expected_hc_ratio = 0.480355;
        constexpr double expected_emissivity_ratio = 2.137499;
        // Compare with known values using tolerances
        EXPECT_NEAR(hc_ratio, expected_hc_ratio, 1e-6);
        EXPECT_NEAR(emissivity_ratio, expected_emissivity_ratio, 1e-6);
    }

}  // namespace Tarcog::ISO15099
