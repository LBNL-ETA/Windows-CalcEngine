#pragma once

#include <string>
#include <vector>

#include <WCECommon.hpp>

namespace CompoundProduct
{
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

    //! \brief Bulk material between two interfaces. Transmittance is for one pass through it.
    struct Bulk
    {
        FenestrationCommon::CSeries internalTransmittance;
    };

    //! \brief Internal properties of one interface.
    //!
    //! transmittanceFront and reflectanceFront are seen by light that travels from the front of
    //! the product to the back, transmittanceBack and reflectanceBack by light that travels in
    //! the opposite direction.
    struct Interface
    {
        FenestrationCommon::CSeries transmittanceFront;
        FenestrationCommon::CSeries transmittanceBack;
        FenestrationCommon::CSeries reflectanceFront;
        FenestrationCommon::CSeries reflectanceBack;

        //! \brief The same interface seen from its other side.
        [[nodiscard]] Interface flipped() const;
    };

    struct Monolithic;
    struct KnownPart;

    //! \brief Properties of a whole product, or of a part of it, at normal incidence.
    //!
    //! transmittanceFront is for light that enters at the front, transmittanceBack for light that
    //! enters at the back. A product that was measured with one transmittance has the same
    //! spectrum in both.
    //!
    //! The functions that start with "deconstruct" calculate internal properties from these
    //! external properties. Everything else in the product must be known. They throw
    //! std::runtime_error if the inputs are not given at the same wavelengths. Problems at single
    //! wavelengths (opaque material, results that are not physical) do not throw. They are
    //! reported in the diagnostics of the result.
    struct ExternalProperties
    {
        FenestrationCommon::CSeries transmittanceFront;
        FenestrationCommon::CSeries transmittanceBack;
        FenestrationCommon::CSeries reflectanceFront;
        FenestrationCommon::CSeries reflectanceBack;

        //! \brief The same product seen from its back side.
        [[nodiscard]] ExternalProperties flipped() const;

        ///////////////////////////////////////////////////////////////////////////
        /// General form. The unknown may be at any place of the product.
        ///////////////////////////////////////////////////////////////////////////

        //! \brief External properties of the part behind a known front part.
        //!
        //! The known front part and the bulk layer behind it are removed from the product. The
        //! part that remains is returned as a whole. It may be one interface or several layers.
        [[nodiscard]] WithDiagnostics<ExternalProperties>
          deconstructPartBehind(const KnownPart & frontPart,
                                const Tolerances & tolerances = {}) const;

        //! \brief External properties of the part in front of a known back part.
        //!
        //! The known back part and the bulk layer in front of it are removed from the product.
        [[nodiscard]] WithDiagnostics<ExternalProperties>
          deconstructPartInFrontOf(const KnownPart & backPart,
                                   const Tolerances & tolerances = {}) const;

        //! \brief Internal properties of an interface that has a known part on each side.
        //!
        //! Both known parts are removed. The interface is the part that remains.
        [[nodiscard]] WithDiagnostics<Interface>
          deconstructInterface(const KnownPart & frontPart,
                               const KnownPart & backPart,
                               const Tolerances & tolerances = {}) const;

        //! \brief Internal transmittance of a bulk layer that has a known part on each side.
        //!
        //! Only the two transmittances of the product are used. Each of them gives one equation
        //! for the same unknown, and the mean of the two equations is solved.
        [[nodiscard]] WithDiagnostics<Bulk>
          deconstructBulk(const ExternalProperties & frontPart,
                          const ExternalProperties & backPart,
                          const Tolerances & tolerances = {}) const;

        //! \brief Internal properties of an uncoated pane.
        //!
        //! This is the start of every calculation. It needs no other known component.
        [[nodiscard]] WithDiagnostics<Monolithic>
          deconstructMonolithic(ReflectanceChoice choice = ReflectanceChoice::Average,
                                const Tolerances & tolerances = {}) const;

        ///////////////////////////////////////////////////////////////////////////
        /// Cases of the sources (LaminateEquations, pylam, Optics 5.1). Each of them is the
        /// general form for one type of product, with the warnings of Optics 5.1 for it.
        ///////////////////////////////////////////////////////////////////////////

        //! \brief Internal properties of the coating of a coated pane.
        //!
        //! Result is oriented in the same way as the coating in the pane. For coating on the back
        //! side, reflectanceFront is the reflectance seen from the substrate.
        [[nodiscard]] WithDiagnostics<Interface>
          deconstructCoating(const Monolithic & substrate,
                             FenestrationCommon::Side coatedSide,
                             const Tolerances & tolerances = {}) const;

        //! \brief Internal transmittance of the interlayer of a laminate of two uncoated panes.
        //!
        //! It is assumed that there is no reflection between the interlayer and the panes.
        [[nodiscard]] WithDiagnostics<Bulk>
          deconstructInterlayer(const Monolithic & front,
                                const Monolithic & back,
                                const Tolerances & tolerances = {}) const;

        //! \brief Internal properties of a coating that is placed between the front pane and the
        //! interlayer of a laminate.
        //!
        //! reflectanceFront of the result is the reflectance seen from the front pane,
        //! reflectanceBack is the reflectance seen from the interlayer.
        [[nodiscard]] WithDiagnostics<Interface>
          deconstructEmbeddedCoating(const Monolithic & front,
                                     const Monolithic & back,
                                     const Bulk & interlayer,
                                     const Tolerances & tolerances = {}) const;

        //! \brief Internal properties of the interface between two thick layers that are in
        //! direct contact (ISO 9050, B.5.2).
        //!
        //! frontSurface is the interface between air and the front layer, backSurface the
        //! interface between the back layer and air. Both may absorb and may have different
        //! reflectances on their two sides.
        [[nodiscard]] WithDiagnostics<Interface>
          deconstructDoubleMonolithic(const Interface & frontSurface,
                                      const Bulk & frontBulk,
                                      const Interface & backSurface,
                                      const Bulk & backBulk,
                                      const Tolerances & tolerances = {}) const;

        //! \brief The same for two layers that have uncoated surfaces to the air.
        [[nodiscard]] WithDiagnostics<Interface>
          deconstructDoubleMonolithic(const Monolithic & front,
                                      const Monolithic & back,
                                      const Tolerances & tolerances = {}) const;
    };

    //! \brief Internal properties of an uncoated pane. Result of deconstructMonolithic.
    //!
    //! Both surfaces of the pane are bare surfaces towards the air. They are identical and do
    //! not absorb, so that the surface transmittance is one minus the surface reflectance. A
    //! pane with a coating is not a Monolithic.
    struct Monolithic
    {
        FenestrationCommon::CSeries surfaceReflectance;
        FenestrationCommon::CSeries internalTransmittance;
    };

    //! \brief Known section of a product that a deconstruction removes from it.
    //!
    //! A part begins and ends with an interface. It can be one interface or any sequence of layers.
    //! Its external properties are given as they are seen in the product: front of the part is
    //! towards the front of the product.
    struct KnownPart
    {
        ExternalProperties externalProperties;
        //! Bulk layer between this part and the part that remains after the removal.
        Bulk bulkToRemainingPart;
    };
}   // namespace CompoundProduct
