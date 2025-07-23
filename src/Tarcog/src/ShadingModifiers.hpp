//! @brief Modifiers are calculated values used for IGUs with interior or exterior shading devices.
//!
//! These modifiers allow an IGU with a shade to behave thermally like an IGU without a shade,
//! by adjusting surface properties such as emissivity and convective heat transfer coefficients (Hc).
//!
//! For example, in an IGU with two glass panes and a single shade (either interior or exterior),
//! the modifiers provide ratio values that adjust the surface coefficients of the glass layers.
//! This adjustment ensures that the overall heat flow through the IGU matches what it would be
//! if the shade were physically present.
//!
//! These modifiers are intended for use in tools like THERM, which cannot import shading devices
//! as explicit layers. In such cases, the shading layer is removed, and the remaining IGU layers
//! are modified using these coefficients to replicate the shade's thermal effect.


#pragma once

#include <map>

#include "EnvironmentConfigurations.hpp"

namespace Tarcog::ISO15099
{
    struct ShadingModifier
    {
        double emissivityRatio{1.0};
        double hcRatio{1.0};
    };

    using ShadingModifiers = std::map<Environment, ShadingModifier>;

    /// Computes the adjusted convective heat transfer coefficient for an IGU surface
    /// after removing an adjacent shading layer.
    ///
    /// This function rescales the original convective coefficient so that the convective
    /// heat flux remains the same even when a shading layer is removed. It adjusts the
    /// value based on the ratio of temperature differences between:
    ///   - the environment and the shading surface, and
    ///   - the environment and the glass surface that originally faced the shade.
    ///
    /// Formula:
    ///   hc_new = hc_env * |(T_env - T_shade_to_env) / (T_env - T_glass_to_shade)|
    ///
    /// If the denominator is zero (i.e., the glass surface temperature equals the
    /// environment temperature), the function returns 1.0 to avoid division by zero.
    ///
    /// @param hc_env Convective coefficient between environment and shading surface (W/m²·K)
    /// @param T_env Air temperature of the environment (K)
    /// @param T_shade_to_env Temperature of the shading surface facing the environment (K)
    /// @param T_glass_to_shade Temperature of the glass surface that faced the shade (K)
    /// @return Adjusted convective coefficient for the glass surface (W/m²·K)
    [[nodiscard]] double computeHcRatio(
        double hc_env,
        double T_env,
        double T_shade_to_env,
        double T_glass_to_shade
    );

    /// Computes the adjusted surface emissivity for a glass layer after removing an adjacent shading device.
    ///
    /// This function ensures that the radiative heat transfer remains the same after
    /// removing the shading layer, by modifying the glass surface's emissivity.
    ///
    /// Formula:
    ///   ε_new = (R_shade_to_env + R_glass_to_shade * τ_shade) / (σ * T_glass_to_shade^4)
    ///
    /// @param R_shade_to_env Radiosity of the shading surface facing the environment (W/m²)
    /// @param R_glass_to_shade Radiosity of the glass surface facing the shade (W/m²)
    /// @param T_glass_to_shade Temperature of the glass surface that faced the shade (K)
    /// @param tau_shade IR transmittance of the shading layer (dimensionless, between 0 and 1)
    /// @return Adjusted emissivity value to assign to the glass surface (dimensionless)
    double computeEmissivityRatio(double R_shade_to_env,
                                  double R_glass_to_shade,
                                  double T_glass_to_shade,
                                  double tau_shade);

}   // namespace Tarcog::ISO15099