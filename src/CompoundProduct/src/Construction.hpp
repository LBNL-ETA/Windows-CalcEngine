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
    //! result of deconstructPartBehind, deconstructPartInFrontOf and deconstructPartInTheMiddle).
    //!
    //! An assembly is the only way to write a product. A product in which two interfaces touch,
    //! or which ends with a bulk layer, cannot be written.
    //!
    //! The components of an assembly can be read with front and segments. A part that is known
    //! only as a whole is one interface in them, because nothing inside it is known.
    //!
    //! add and flipped do not change the assembly. They return a new one.
    class Assembly
    {
    public:
        //! \brief A bulk layer together with the interface that closes it at the back.
        struct Segment
        {
            Bulk bulk;
            Interface backInterface;
        };

        explicit Assembly(const Interface & frontPart);
        explicit Assembly(const ExternalProperties & frontPart);

        [[nodiscard]] Assembly add(const Bulk & bulk, const Interface & part) const;
        [[nodiscard]] Assembly add(const Bulk & bulk, const ExternalProperties & part) const;
        [[nodiscard]] Assembly add(const Bulk & bulk, const Assembly & part) const;

        //! The same assembly seen from its back side.
        [[nodiscard]] Assembly flipped() const;

        //! The first interface of the assembly.
        [[nodiscard]] const Interface & front() const;

        //! Everything behind the first interface, from the front to the back. An assembly of one
        //! interface has no segment.
        [[nodiscard]] const std::vector<Segment> & segments() const;

        //! Throws std::runtime_error if the parts are not given at the same wavelengths.
        [[nodiscard]] ExternalProperties externalProperties() const;

    private:
        Interface m_Front;
        std::vector<Segment> m_Segments;
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
