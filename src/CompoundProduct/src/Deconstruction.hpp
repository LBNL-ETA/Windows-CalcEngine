#pragma once

#include "CompoundProductTypes.hpp"

namespace CompoundProduct
{
    // The deconstruction functions are members of ExternalProperties (CompoundProductTypes.hpp).

    //! \brief Surface of a monolithic towards the air, as an interface.
    [[nodiscard]] Interface uncoatedInterface(const Monolithic & substrate);
}   // namespace CompoundProduct
