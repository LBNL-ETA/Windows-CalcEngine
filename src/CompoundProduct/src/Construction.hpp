#pragma once

#include <vector>

#include "CompoundProductTypes.hpp"

namespace CompoundProduct
{
    //! \brief A product that is assembled from parts, from the front to the back.
    //!
    //! An assembly starts with one part. Every step adds one bulk layer and the part behind it,
    //! so that parts and bulk layers always alternate:
    //!
    //!     Assembly(frontPart).add(bulk, part).add(bulk, part).externalProperties()
    //!
    //! A part is given in one of three ways: as one Interface, as another Assembly, or as
    //! ExternalProperties of a part that is known only as a whole (a measured product, or the
    //! result of deconstructPartBehind and deconstructPartInFrontOf).
    //!
    //! An assembly is the only way to write a product. A product in which two interfaces touch,
    //! or which ends with a bulk layer, cannot be written.
    //!
    //! add and flipped do not change the assembly. They return a new one.
    class Assembly
    {
    public:
        explicit Assembly(const Interface & frontPart);
        explicit Assembly(const ExternalProperties & frontPart);

        [[nodiscard]] Assembly add(const Bulk & bulk, const Interface & part) const;
        [[nodiscard]] Assembly add(const Bulk & bulk, const ExternalProperties & part) const;
        [[nodiscard]] Assembly add(const Bulk & bulk, const Assembly & part) const;

        //! The same assembly seen from its back side.
        [[nodiscard]] Assembly flipped() const;

        //! Throws std::runtime_error if the parts are not given at the same wavelengths.
        [[nodiscard]] ExternalProperties externalProperties() const;

    private:
        //! Components from the front to the back. The number of interfaces is always the number
        //! of bulk layers plus one.
        std::vector<Interface> m_Interfaces;
        std::vector<Bulk> m_Bulks;
    };

    //! \brief Interface that does not reflect (for example glass to interlayer).
    [[nodiscard]] Interface nonReflectingInterface(const std::vector<double> & wavelengths);

    //! \brief Internal transmittance of the same material at different thickness.
    //!
    //! Both thicknesses must be given in the same unit.
    [[nodiscard]] Bulk withThickness(const Bulk & bulk, double oldThickness, double newThickness);

    //! \brief Sorted wavelengths that are present in at least one of the products.
    [[nodiscard]] std::vector<double>
      commonWavelengths(const std::vector<ExternalProperties> & products);
}   // namespace CompoundProduct
