#pragma once

#include <vector>

#include "CompoundProductTypes.hpp"

namespace CompoundProduct
{
    //! \brief Calculates external properties of a stack of bulk layers and interfaces.
    //!
    //! Throws std::runtime_error if number of interfaces is not number of bulk layers plus one or
    //! if the series are not given at the same wavelengths.
    [[nodiscard]] ExternalProperties construct(const Stack & stack);

    //! \brief Interface that does not reflect (for example glass to interlayer).
    [[nodiscard]] Interface nonReflectingInterface(const std::vector<double> & wavelengths);

    [[nodiscard]] Interface flipped(const Interface & layerInterface);
    [[nodiscard]] Stack flipped(const Stack & stack);

    //! \brief Puts stack "back" behind stack "front".
    //!
    //! The last interface of the front stack and the first interface of the back stack are
    //! replaced by the interface "between".
    [[nodiscard]] Stack joined(const Stack & front, const Stack & back, const Interface & between);

    //! \brief Internal transmittance of the same material at different thickness.
    //!
    //! Both thicknesses must be given in the same unit.
    [[nodiscard]] Bulk withThickness(const Bulk & bulk, double oldThickness, double newThickness);

    //! \brief Sorted wavelengths that are present in at least one of the products.
    [[nodiscard]] std::vector<double>
      commonWavelengths(const std::vector<ExternalProperties> & products);
}   // namespace CompoundProduct
