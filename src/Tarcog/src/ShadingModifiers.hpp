//! @brief Modifiers are calculated values used for IGUs with interior or exterior shading devices.
//!
//! These modifiers allow an IGU with a shade to behave thermally like an IGU without a shade,
//! by adjusting surface properties such as emissivity and convective heat transfer coefficients
//! (Hc).
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
    /// The ventilated gap gain (qv) is subtracted or added depending on shade position.
    ///
    /// Formula:
    ///   hc_new = hc_env * (T_env - T_shade_to_env) / (T_env - T_glass_to_shade) ± qv / ΔT
    ///
    /// @param hc_env Convective coefficient between environment and shading surface (W/m²·K)
    /// @param T_env Air temperature of the environment (K)
    /// @param T_shade_to_env Temperature of the shading surface facing the environment (K)
    /// @param T_glass_to_shade Temperature of the glass surface that faced the shade (K)
    /// @param qv Ventilated adjacent gap heat gain or loss (W/m²)
    /// @return Adjusted convective coefficient for the glass surface (W/m²·K)
    [[nodiscard]] double computeHcRatio(
      double hc_env, double T_env, double T_shade_to_env, double T_glass_to_shade, double qv);

    /// Computes the adjusted surface emissivity for a glass layer after removing an adjacent
    /// shading device (either interior or exterior).
    ///
    /// This version accounts for whether the system had more than two layers. If the number of
    /// layers was greater than 2, the radiosity from the previous layer (Rb or Rf) is used;
    /// otherwise, the environmental irradiation is used directly (Gin or Gout).
    ///
    /// Formula (legacy FORTRAN style):
    /// For exterior shade:
    ///     temp = (nlayer > 2) ? Rf(3) : Gin
    ///     ε_new = (Rf(1) - tir(3) * temp - (1 - tir(4)) * Gout) / (σ * Tf(2)^4 - Gout)
    ///
    /// For interior shade:
    ///     temp = (nlayer > 2) ? Rb(nlayer - 2) : Gout
    ///     ε_new = (Rb(nlayer) - tir(2*nlayer - 3) * temp - (1 - tir(2*nlayer - 2)) * Gin)
    ///              / (σ * Tb(nlayer - 1)^4 - Gin)
    ///
    /// @param R_shade Surface radiosity of the shade (Rf(1) or Rb(nlayer)) [W/m²]
    /// @param R_prev  Radiosity from the adjacent layer behind the shade (Rf(3) or Rb(nlayer-2)),
    ///                or environment radiosity (Gin or Gout) [W/m²]
    /// @param T_glass Temperature of the glass surface that will replace the shade [K]
    /// @param tau_shade IR transmittance of the shade surface toward the glass (tir of the glass
    /// surface) [–]
    /// @param tau_opposite IR transmittance of the shade surface toward the environment [–]
    /// @param G_env Irradiation from the environment (Gin or Gout) [W/m²]
    /// @return Adjusted emissivity value for the glass surface [–]
    double computeEmissivityRatio(double glassEmissivity,
                                  double R_shade,
                                  double R_prev,
                                  double T_glass,
                                  double tau_shade,
                                  double tau_opposite,
                                  double G_env);

}   // namespace Tarcog::ISO15099
