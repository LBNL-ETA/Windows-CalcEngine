#pragma once

#include "CompoundProductTypes.hpp"

namespace CompoundProduct
{
    // All functions throw std::runtime_error if the inputs are not given at the same wavelengths.
    // Problems at single wavelengths (opaque material, results that are not physical) do not
    // throw. They are reported in the diagnostics of the result.

    //! \brief Internal properties of a monolithic product from its measured properties.
    [[nodiscard]] WithDiagnostics<Monolithic>
      deconstructMonolithic(const ExternalProperties & measured,
                            ReflectanceChoice choice = ReflectanceChoice::Average,
                            const Tolerances & tolerances = {});

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
