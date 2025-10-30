#pragma once

#include <variant>

#include <WCECommon.hpp>

namespace EffectiveLayers
{
    //! \brief Structure to hold data about shading device opennes as it is defined from user point
    //! of view
    struct ShadeOpenness
    {
        //! \var Dl
        //! \brief Left side openness of the shading layer [m]
        double Dl{0.0};

        //! \var Dr
        //! \brief Right side openness of the shading layer [m]
        double Dr{0.0};

        //! \var Dtop
        //! \brief Top side openness of the shading layer [m]
        double Dtop{0.0};

        //! \var Dbot
        //! \brief Bottom side openness of the shading layer [m]
        double Dbot{0.0};
    };

    //! \brief Effective permeabilityFactor of shading layer that is necessary for thermal
    //! calculations.
    struct EffectiveMultipliers
    {
        //! \var Mfront
        //! \brief Effective front thermal openness multiplier used for in between gap (or
        //! environment calculations) [-]
        double Mfront;

        //! \var Mleft
        //! \brief Multiplier of the left side openness where the air is flowing between the gaps
        //! [-]
        double Mleft;

        //! \var Mright
        //! \brief Multiplier of the right side openness where the air is flowing between the gaps
        //! [-]
        double Mright;

        //! \var Mtop
        //! \brief Multiplier of the top side openness where the air is flowing between the gaps [-]
        double Mtop;

        //! \var Mbot
        //! \brief Multiplier of the bottom side openness where the air is flowing between the gaps
        //! [-]
        double Mbot;

        //! \var PermeabilityFactor
        //! \brief Geometrical openness used to calculate equivalent layer conductivity
        double PermeabilityFactor;
    };

    bool isClosed(const EffectiveMultipliers & effectiveOpenness);

    struct Coefficients
    {
        double C1{0.0};
        double C2{0.0};
        double C3{0.0};
        double C4{0.0};
    };

    // This is the output for the effective layer properties
    struct EffectiveLayerProperties
    {
        double thickness{0.0};
        EffectiveMultipliers openness{0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        Coefficients coeffs{0.0, 0.0, 0.0, 0.0};
    };

    [[nodiscard]] EffectiveLayerProperties makeCommonValues(double thickness,
                                                            double permeabilityFactor,
                                                            const ShadeOpenness & openness = {
                                                              0, 0, 0, 0}) noexcept;

    [[nodiscard]] EffectiveLayerProperties
      makeHorizontalVenetianValues(double thickness,
                                   const FenestrationCommon::Venetian::Geometry & geometry,
                                   const ShadeOpenness & openness = {0, 0, 0, 0}) noexcept;

    [[nodiscard]] EffectiveLayerProperties
      makeVerticalVenetianValues(double thickness,
                                 const FenestrationCommon::Venetian::Geometry & geometry,
                                 const ShadeOpenness & openness = {0, 0, 0, 0}) noexcept;

    [[nodiscard]] EffectiveLayerProperties
      makePerforatedValues(double thickness,
                           const FenestrationCommon::Perforated::Geometry & geometry,
                           const ShadeOpenness & openness = {0, 0, 0, 0}) noexcept;

    [[nodiscard]] EffectiveLayerProperties
      makeWovenValues(double thickness,
                      const FenestrationCommon::Woven::Geometry & geometry,
                      const ShadeOpenness & openness = {0, 0, 0, 0}) noexcept;

    [[nodiscard]] EffectiveLayerProperties
      makeLouveredShutterValues(const FenestrationCommon::LouveredShutter::Geometry & geometry,
                                const ShadeOpenness & openness = {0, 0, 0, 0}) noexcept;

    [[nodiscard]] EffectiveLayerProperties
      makeUserDefinedValues(double thickness,
                            double permeabilityFactor,
                            double effectiveFrontThermalOpennessArea,
                            const ShadeOpenness & openness = {0, 0, 0, 0}) noexcept;

}   // namespace EffectiveLayers
