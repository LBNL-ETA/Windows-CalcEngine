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

    //! \brief One warning at one wavelength.
    struct Diagnostic
    {
        double wavelength{0};
        Warning warning{Warning::NoData};
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
        FenestrationCommon::CSeries tau;
    };

    //! \brief Internal properties of one interface.
    //!
    //! tf is the transmittance and rf the reflectance for light that travels from the front of
    //! the product to the back. tb and rb are the same for light that travels in the opposite
    //! direction. Small letters are used for internal properties.
    struct Interface
    {
        FenestrationCommon::CSeries tf;
        FenestrationCommon::CSeries tb;
        FenestrationCommon::CSeries rf;
        FenestrationCommon::CSeries rb;

        //! \brief The same interface seen from its other side.
        [[nodiscard]] Interface flipped() const;
    };

    struct Monolithic;
    class Assembly;

    //! \brief Properties of a whole product, or of a part of it, at normal incidence.
    //!
    //! Tf is the transmittance and Rf the reflectance for light that enters at the front. Tb and
    //! Rb are the same for light that enters at the back. A product that was measured with one
    //! transmittance has the same spectrum in Tf and Tb. Capital letters are used for external
    //! properties.
    //!
    //! The functions that start with "deconstruct" calculate internal properties from these
    //! external properties. Everything else in the product must be known. They throw
    //! std::runtime_error if the inputs are not given at the same wavelengths. Problems at single
    //! wavelengths (opaque material, results that are not physical) do not throw. They are
    //! reported in the diagnostics of the result.
    struct ExternalProperties
    {
        FenestrationCommon::CSeries Tf;
        FenestrationCommon::CSeries Tb;
        FenestrationCommon::CSeries Rf;
        FenestrationCommon::CSeries Rb;

        //! \brief The same product seen from its back side.
        [[nodiscard]] ExternalProperties flipped() const;

        ///////////////////////////////////////////////////////////////////////////
        /// General functions. They are the equations of the model. The unknown may be at any
        /// place of the product. The functions of the cases below call them.
        ///
        /// A known part is given as an Assembly. It can be one interface, several known
        /// components, or a part that is known only by its external properties. It is given as
        /// it is seen in the product: its front is towards the front of the product. The
        /// arguments are in the order in which the parts and the bulk layers lie in the product,
        /// from the front to the back.
        ///////////////////////////////////////////////////////////////////////////

        //! \brief External properties of the part behind a known front part.
        //!
        //! The known front part and the bulk layer behind it are removed from the product. The
        //! part that remains is returned as a whole. It may be one interface or several layers.
        [[nodiscard]] WithDiagnostics<ExternalProperties>
          deconstructPartBehind(const Assembly & frontPart,
                                const Bulk & bulk,
                                const Tolerances & tolerances = {}) const;

        //! \brief External properties of the part in front of a known back part.
        //!
        //! The bulk layer in front of the known back part and that part are removed from the
        //! product.
        [[nodiscard]] WithDiagnostics<ExternalProperties>
          deconstructPartInFrontOf(const Bulk & bulk,
                                   const Assembly & backPart,
                                   const Tolerances & tolerances = {}) const;

        //! \brief External properties of the part that has a known part on each side.
        //!
        //! frontBulk is the bulk layer between the front part and the part in the middle.
        //! backBulk is the bulk layer between the part in the middle and the back part. Both
        //! known parts are removed. The part in the middle is returned as a whole. It may be one
        //! interface or several layers.
        [[nodiscard]] WithDiagnostics<ExternalProperties>
          deconstructPartInTheMiddle(const Assembly & frontPart,
                                     const Bulk & frontBulk,
                                     const Bulk & backBulk,
                                     const Assembly & backPart,
                                     const Tolerances & tolerances = {}) const;

        //! \brief Internal transmittance of a bulk layer that has a known part on each side.
        //!
        //! Only the two transmittances of the product are used. Each of them gives one equation
        //! for the same unknown, and the mean of the two equations is solved.
        [[nodiscard]] WithDiagnostics<Bulk>
          deconstructBulk(const Assembly & frontPart,
                          const Assembly & backPart,
                          const Tolerances & tolerances = {}) const;

        //! \brief Internal properties of a part that consists of two equal interfaces with a
        //! bulk layer between them.
        //!
        //! The interfaces do not absorb. An uncoated pane is such a part, and it is the first
        //! case of the sources. This is the only deconstruction that needs no known part. A
        //! calculation that starts from measurements alone starts with it.
        [[nodiscard]] WithDiagnostics<Monolithic>
          deconstructMonolithic(ReflectanceChoice choice = ReflectanceChoice::Average,
                                const Tolerances & tolerances = {}) const;

        ///////////////////////////////////////////////////////////////////////////
        /// Functions of the cases of the sources (LaminateEquations, pylam, Optics 5.1). Each
        /// of them takes the products of one case, calls one general function and applies the
        /// rules of Optics 5.1 for that case.
        ///////////////////////////////////////////////////////////////////////////

        //! \brief Internal properties of the coating of a coated pane.
        //!
        //! Result is oriented in the same way as the coating in the pane. For coating on the back
        //! side, rf is the reflectance seen from the substrate.
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
        //! rf of the result is the reflectance seen from the front pane, rb is the reflectance
        //! seen from the interlayer.
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
        FenestrationCommon::CSeries tau;
    };
}   // namespace CompoundProduct
