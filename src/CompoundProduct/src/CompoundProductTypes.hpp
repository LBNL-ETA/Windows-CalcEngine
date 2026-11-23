#pragma once

#include <string>
#include <vector>

#include <WCECommon.hpp>

namespace CompoundProduct
{
    //! \brief Measured properties of a whole product at normal incidence.
    struct ExternalProperties
    {
        FenestrationCommon::CSeries transmittance;
        FenestrationCommon::CSeries reflectanceFront;
        FenestrationCommon::CSeries reflectanceBack;
    };

    //! \brief Internal properties of one interface.
    //!
    //! reflectanceFront is seen by light that travels from the front of the product to the back,
    //! reflectanceBack by light that travels in the opposite direction.
    struct Interface
    {
        FenestrationCommon::CSeries transmittance;
        FenestrationCommon::CSeries reflectanceFront;
        FenestrationCommon::CSeries reflectanceBack;
    };

    //! \brief Bulk material between two interfaces. Transmittance is for one pass through it.
    struct Bulk
    {
        FenestrationCommon::CSeries internalTransmittance;
    };

    //! \brief Bulk layers separated by interfaces, ordered from the front to the back.
    //!
    //! Number of interfaces is always the number of bulk layers plus one.
    struct Stack
    {
        std::vector<Interface> interfaces;
        std::vector<Bulk> bulks;
    };

    enum class Warning
    {
        OpaqueLayer,
        OpaqueSubstrate,
        NoData,
        UnphysicalClamped,
        InsensitiveCoatingSubstrateSide,
        InsensitiveCoatingInterlayerSide,
        InsensitiveInterlayer
    };

    //! source is the name of the input whose deconstruction raised the warning. It is set by
    //! the actions and it is empty in the result of a single deconstruction.
    struct Diagnostic
    {
        double wavelength{0};
        Warning warning{Warning::NoData};
        std::string source{};
    };

    //! \brief Limits used to decide when a measured value does not carry enough information.
    struct Tolerances
    {
        //! Measured values below this limit are treated as zero.
        double limitOfDetection{0.001};
        //! Internal transmittance below which a coating reflectance behind it is not reliable.
        double sensitivityThreshold{0.328};
        //! Internal transmittance of the panes below which interlayer result is not reliable.
        double interlayerSensitivityThreshold{0.34};
        //! Results outside of the range from zero to one are moved to the closest limit.
        bool clampToPhysical{true};
    };

    //! \brief Result of a calculation together with warnings raised at single wavelengths.
    template<typename Value>
    struct WithDiagnostics
    {
        Value value;
        std::vector<Diagnostic> diagnostics;
    };

    enum class ReflectanceChoice
    {
        Front,
        Back,
        Average
    };
}   // namespace CompoundProduct
