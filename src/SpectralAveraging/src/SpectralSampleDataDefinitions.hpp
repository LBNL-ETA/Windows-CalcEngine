#pragma once

#include <array>

#include <WCECommon.hpp>

namespace SpectralAveraging
{
    /// Optical measurement definitions

    enum class MeasurementType
    {
        Direct,
        Diffuse
    };

    constexpr std::array allMeasurements{MeasurementType::Direct, MeasurementType::Diffuse};

    using MeasurementKey =
      std::tuple<FenestrationCommon::PropertySurface, FenestrationCommon::Side, MeasurementType>;

    inline auto
      key(FenestrationCommon::PropertySurface p, FenestrationCommon::Side s, MeasurementType m)
    {
        return std::make_tuple(p, s, m);
    }

    /// Helper function to iterate through all possible combination since it is used often
    template<typename Func>
    void forEach_PropertySurface_Side_MeasurementType(Func && func)
    {
        for(const auto & prop : FenestrationCommon::allProperties())
        {
            for(const auto & side : FenestrationCommon::allSides())
            {
                for(const auto & scatter : allMeasurements)
                {
                    func(prop, side, scatter);
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////
    /// Optical scattering definitions
    ///////////////////////////////////////////////////////////////

    enum class ScatteringType
    {
        Direct = 0,
        Diffuse,
        Total
    };

    constexpr std::array allScatteringTypes{
        ScatteringType::Direct, ScatteringType::Diffuse, ScatteringType::Total};

    constexpr std::array scatteringTypeString{"Direct", "Diffuse", "Total"};

    constexpr std::string_view scatterName(ScatteringType scatter)
    {
        return scatteringTypeString[static_cast<int>(scatter)];
    }

    using ScatteringKey =
      std::tuple<FenestrationCommon::Property, FenestrationCommon::Side, ScatteringType>;

    inline auto key(FenestrationCommon::Property p, FenestrationCommon::Side s, ScatteringType m)
    {
        return std::make_tuple(p, s, m);
    }

    /// Helper function to iterate through all possible combination since it is used often
    template<typename Func>
    void forEach_Property_Side_Scatter(Func && func)
    {
        for(const auto & prop : FenestrationCommon::allProperties())
        {
            for(const auto & side : FenestrationCommon::allSides())
            {
                for(const auto & scatter : allScatteringTypes)
                {
                    func(prop, side, scatter);
                }
            }
        }
    }

    /// General definition for property and side

    inline auto key(FenestrationCommon::Property p, FenestrationCommon::Side s)
    {
        return std::make_pair(p, s);
    }
}
