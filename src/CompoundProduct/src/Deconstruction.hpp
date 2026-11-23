#pragma once

#include "CompoundProductTypes.hpp"

namespace CompoundProduct
{
    // All functions throw std::runtime_error if the inputs are not given at the same wavelengths.
    // Problems at single wavelengths (opaque material, results that are not physical) do not
    // throw. They are reported in the diagnostics of the result.

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

    //! \brief Surface of a monolithic towards the air, as an interface.
    [[nodiscard]] Interface uncoatedInterface(const Monolithic & substrate);

    //! \brief Internal properties of a monolithic product from its measured properties.
    //!
    //! This is the start of every calculation. It needs no other known component.
    [[nodiscard]] WithDiagnostics<Monolithic>
      deconstructMonolithic(const ExternalProperties & measured,
                            ReflectanceChoice choice = ReflectanceChoice::Average,
                            const Tolerances & tolerances = {});

    ///////////////////////////////////////////////////////////////////////////
    /// General form. The unknown component may be at any place of the measured product. Everything
    /// else in the product must be known.
    ///////////////////////////////////////////////////////////////////////////

    //! \brief Known section of a measured product on one side of the unknown interface.
    //!
    //! A part begins and ends with an interface. It can be one interface or any stack of layers.
    //! Its external properties are given as they are seen in the measured product: front of the
    //! part is towards the front of the product.
    struct KnownPart
    {
        ExternalProperties externalProperties;
        //! Bulk layer between this part and the unknown interface.
        Bulk bulkToUnknownInterface;
    };

    //! \brief Internal properties of an interface that has a known part on each side.
    [[nodiscard]] WithDiagnostics<Interface>
      deconstructInterface(const ExternalProperties & measured,
                           const KnownPart & frontPart,
                           const KnownPart & backPart,
                           const Tolerances & tolerances = {});

    //! \brief Internal properties of the last interface of a product. Everything in front of it
    //! is known.
    [[nodiscard]] WithDiagnostics<Interface>
      deconstructLastInterface(const ExternalProperties & measured,
                               const KnownPart & frontPart,
                               const Tolerances & tolerances = {});

    //! \brief Internal properties of the first interface of a product. Everything behind it is
    //! known.
    [[nodiscard]] WithDiagnostics<Interface>
      deconstructFirstInterface(const ExternalProperties & measured,
                                const KnownPart & backPart,
                                const Tolerances & tolerances = {});

    //! \brief Internal transmittance of a bulk layer that has a known part on each side.
    //!
    //! Only the measured transmittance is used.
    [[nodiscard]] WithDiagnostics<Bulk> deconstructBulk(const ExternalProperties & measured,
                                                      const ExternalProperties & frontPart,
                                                      const ExternalProperties & backPart,
                                                      const Tolerances & tolerances = {});

    ///////////////////////////////////////////////////////////////////////////
    /// Cases of the sources (LaminateEquations, pylam, Optics 5.1). Each of them is the general
    /// form for one type of product, with the warnings of Optics 5.1 for that product.
    ///////////////////////////////////////////////////////////////////////////

    //! \brief Internal properties of a coating from measured properties of a coated product.
    //!
    //! Result is oriented in the same way as the coating in the measured product. For coating on
    //! the back side, reflectanceFront is the reflectance seen from the substrate.
    [[nodiscard]] WithDiagnostics<Interface>
      deconstructCoating(const ExternalProperties & coated,
                         const Monolithic & substrate,
                         FenestrationCommon::Side coatedSide,
                         const Tolerances & tolerances = {});

    //! \brief Internal transmittance of the interlayer from measured properties of a laminate
    //! made of two uncoated panes.
    //!
    //! It is assumed that there is no reflection between the interlayer and the panes.
    [[nodiscard]] WithDiagnostics<Bulk> deconstructInterlayer(const ExternalProperties & laminate,
                                                            const Monolithic & front,
                                                            const Monolithic & back,
                                                            const Tolerances & tolerances = {});

    //! \brief Internal properties of a coating that is placed between the front pane and the
    //! interlayer of a laminate.
    //!
    //! reflectanceFront of the result is the reflectance seen from the front pane,
    //! reflectanceBack is the reflectance seen from the interlayer.
    [[nodiscard]] WithDiagnostics<Interface>
      deconstructEmbeddedCoating(const ExternalProperties & laminate,
                                 const Monolithic & front,
                                 const Monolithic & back,
                                 const Bulk & interlayer,
                                 const Tolerances & tolerances = {});

    //! \brief Internal properties of the interface between two thick layers that are in direct
    //! contact (ISO 9050, B.5.2).
    //!
    //! frontSurface is the interface between air and the front layer, backSurface the interface
    //! between the back layer and air. Both may absorb and may have different reflectances on
    //! their two sides.
    [[nodiscard]] WithDiagnostics<Interface>
      deconstructDoubleMonolithic(const ExternalProperties & measured,
                                  const Interface & frontSurface,
                                  const Bulk & frontBulk,
                                  const Interface & backSurface,
                                  const Bulk & backBulk,
                                  const Tolerances & tolerances = {});

    //! \brief The same for two layers that have uncoated surfaces to the air.
    [[nodiscard]] WithDiagnostics<Interface>
      deconstructDoubleMonolithic(const ExternalProperties & measured,
                                  const Monolithic & front,
                                  const Monolithic & back,
                                  const Tolerances & tolerances = {});
}   // namespace CompoundProduct
