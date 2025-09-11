#pragma once

#include "EnumValues.hpp"

namespace FenestrationCommon
{
    template<typename T>
    int sgn(T val)
    {
        return (T(0) < val) - (val < T(0));
    }

    //////////////////////////////////////////////////////////////////////////
    /// Side
    //////////////////////////////////////////////////////////////////////////

    enum class Side
    {
        Front,
        Back
    };

    template<>
    struct EnumBounds<Side>
    {
        static constexpr auto first = Side::Front;
        static constexpr auto last = Side::Back;
    };

    constexpr auto allSides()
    {
        return enumValues<Side>();
    }

    //////////////////////////////////////////////////////////////////////////
    /// Property
    //////////////////////////////////////////////////////////////////////////

    enum class Property
    {
        T,
        R,
        Abs
    };

    template<>
    struct EnumBounds<Property>
    {
        static constexpr auto first = Property::T;
        static constexpr auto last = Property::Abs;
    };

    constexpr auto allProperties()
    {
        return enumValues<Property>();
    }

    inline Side oppositeSide(const Side t_Side)
    {
        auto aSide = Side::Front;
        if(t_Side == Side::Front)
        {
            aSide = Side::Back;
        }
        return aSide;
    }

    inline Side getSide(const Side side, const bool flipped)
    {
        return flipped ? oppositeSide(side) : side;
    }

    //////////////////////////////////////////////////////////////////////////
    /// WavelengthRange
    //////////////////////////////////////////////////////////////////////////

    enum class WavelengthRange
    {
        Solar,
        Visible,
        IR
    };

    struct Limits
    {
        Limits(double min, double max) : minimum(min), maximum(max)
        {}
        double minimum;
        double maximum;
    };

    template<>
    struct EnumBounds<WavelengthRange>
    {
        static constexpr auto first = WavelengthRange::Solar;
        static constexpr auto last = WavelengthRange::IR;
    };

    constexpr auto allWavelengthRanges()
    {
        return enumValues<WavelengthRange>();
    }

    //////////////////////////////////////////////////////////////////////////
    /// PropertySimple
    //////////////////////////////////////////////////////////////////////////

    // Short version of enum class Property is necessary because in optical routines it is quite
    // often the case when calculations are performed only over transmittance and reflectance. It is
    // also often the case when Transmittance and Reflectance have different structure from
    // absorptances.
    enum class PropertySurface
    {
        T,
        R
    };

    template<>
    struct EnumBounds<PropertySurface>
    {
        static constexpr auto first = PropertySurface::T;
        static constexpr auto last = PropertySurface::R;
    };

    constexpr auto allPropertySimple()
    {
        return enumValues<PropertySurface>();
    }

    inline PropertySurface toPropertySimple(const Property prop)
    {
        auto result{PropertySurface::T};
        if(prop == Property::R)
        {
            result = PropertySurface::R;
        }
        return result;
    }

    inline Property toProperty(const PropertySurface prop)
    {
        auto result{Property::T};
        if(prop == PropertySurface::R)
        {
            result = Property::R;
        }
        return result;
    }

    enum class Scattering
    {
        DirectDirect,
        DirectDiffuse,
        DiffuseDiffuse,
        DirectHemispherical
    };

    template<>
    struct EnumBounds<Scattering>
    {
        static constexpr auto first = Scattering::DirectDirect;
        static constexpr auto last = Scattering::DirectHemispherical;
    };

    constexpr auto allScattering()
    {
        return enumValues<PropertySurface>();
    }

    //////////////////////////////////////////////////////////////////////////
    /// ScatteringSimple
    //////////////////////////////////////////////////////////////////////////

    enum class ScatteringSimple
    {
        Direct,
        Diffuse
    };

    template<>
    struct EnumBounds<ScatteringSimple>
    {
        static constexpr auto first = ScatteringSimple::Direct;
        static constexpr auto last = ScatteringSimple::Diffuse;
    };

    constexpr auto allScatteringSimple()
    {
        return enumValues<ScatteringSimple>();
    }

    //////////////////////////////////////////////////////////////////////////
    /// EnergyFlow
    //////////////////////////////////////////////////////////////////////////

    enum class EnergyFlow
    {
        Forward,
        Backward
    };

    template<>
    struct EnumBounds<EnergyFlow>
    {
        static constexpr auto first = EnergyFlow::Forward;
        static constexpr auto last = EnergyFlow::Backward;
    };

    constexpr auto allEnergyFlow()
    {
        return enumValues<EnergyFlow>();
    }

    inline EnergyFlow getFlowFromSide(const Side t_Side)
    {
        auto aResult = EnergyFlow::Forward;
        if(t_Side == Side::Back)
        {
            aResult = EnergyFlow::Backward;
        }

        return aResult;
    }

    inline Side getSideFromFlow(const EnergyFlow t_EnergyFlow)
    {
        auto aResult = Side::Front;
        if(t_EnergyFlow == EnergyFlow::Backward)
        {
            aResult = Side::Back;
        }

        return aResult;
    }
}   // namespace FenestrationCommon
