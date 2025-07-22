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
}