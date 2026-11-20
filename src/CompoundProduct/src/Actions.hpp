#pragma once

#include <optional>
#include <string>
#include <vector>

#include "CompoundProductTypes.hpp"

namespace CompoundProduct
{
    // An action calculates the measured properties of a new product from the measured properties
    // of existing products. Every action deconstructs the given products, replaces one component
    // and constructs the result. The diagnostics of all steps are returned with the result, in
    // the order of the steps. The source of a diagnostic is the name of the input whose
    // deconstruction raised it, written as in the call: "substrate", "coatingSource.measured".
    // For a laminate the panes and the interlayers are numbered from the front, starting with
    // zero: "pane[1].measured", "interlayer[0].onPaneInFront.measured".
    //
    // All functions throw std::runtime_error if the inputs are not given at the same wavelengths.

    struct ActionOptions
    {
        //! Reflectance that is used when an uncoated pane or a substrate is deconstructed.
        ReflectanceChoice substrateReflectance{ReflectanceChoice::Average};
        Tolerances tolerances;
    };

    // Measured products as they are given to the actions. thickness is the thickness at which
    // the product was measured. It is used only when an action changes the thickness.

    struct UncoatedPane
    {
        ExternalProperties measured;
        double thickness{0};
    };

    //! \brief Pane with a coating that faces the air.
    //!
    //! substrate is the uncoated glass of the pane at the same thickness.
    //!
    //! A pane with an applied film is described in the same way. The film is a coating for the
    //! calculation.
    struct CoatedPane
    {
        ExternalProperties measured;
        ExternalProperties substrate;
        FenestrationCommon::Side coatedSide{FenestrationCommon::Side::Back};
        double thickness{0};
    };

    //! \brief Measured laminate of three layers that gives the properties of a coating between
    //! glass and interlayer: a coated pane, an interlayer and an uncoated pane.
    //!
    //! coatedPaneSubstrate is the uncoated glass of the coated pane of this laminate.
    //! coatedPanePosition tells whether the coated pane is the front or the back pane of the
    //! measured laminate.
    struct ReferenceLaminate
    {
        ExternalProperties measured;
        ExternalProperties coatedPaneSubstrate;
        Bulk interlayer;
        ExternalProperties uncoatedPane;
        FenestrationCommon::Side coatedPanePosition{FenestrationCommon::Side::Front};
    };

    struct Interlayer
    {
        FenestrationCommon::CSeries internalTransmittance;
        double thickness{0};
    };

    //! \brief Uncoated pane of the same glass at a different thickness.
    //!
    //! newThickness must be given in the unit of the thickness of the pane.
    [[nodiscard]] WithDiagnostics<ExternalProperties>
      changeUncoatedPaneThickness(const UncoatedPane & pane,
                                  double newThickness,
                                  const ActionOptions & options = {});

    //! \brief Coated pane with the same coating and the same glass at a different thickness.
    //!
    //! newThickness must be given in the unit of the thickness of the pane.
    [[nodiscard]] WithDiagnostics<ExternalProperties>
      changeCoatedPaneThickness(const CoatedPane & pane,
                                double newThickness,
                                const ActionOptions & options = {});

    //! \brief Uncoated substrate with the coating of another pane on one of its sides.
    //!
    //! The coating may be placed on a different side than it has on coatingSource. Its
    //! reflectance towards the glass stays towards the glass.
    [[nodiscard]] WithDiagnostics<ExternalProperties>
      applyCoating(const ExternalProperties & substrate,
                   const CoatedPane & coatingSource,
                   FenestrationCommon::Side coatedSide,
                   const ActionOptions & options = {});

    //! \brief Coated pane that carries the coating of coatedPane on a different substrate.
    //!
    //! substrate is the uncoated glass of coatedPane and has its thickness. newSubstrate is the
    //! uncoated glass of the result. The coating stays on coatedSide.
    [[nodiscard]] WithDiagnostics<ExternalProperties>
      changeSubstrate(const ExternalProperties & coatedPane,
                      const ExternalProperties & substrate,
                      const ExternalProperties & newSubstrate,
                      FenestrationCommon::Side coatedSide,
                      const ActionOptions & options = {});

    //! \brief Uncoated substrate with one coating on its front and another on its back side.
    [[nodiscard]] WithDiagnostics<ExternalProperties>
      coatBothSides(const ExternalProperties & substrate,
                    const CoatedPane & frontCoatingSource,
                    const CoatedPane & backCoatingSource,
                    const ActionOptions & options = {});

    //! \brief Internal transmittance of an interlayer from a measured laminate of two uncoated
    //! panes and from the measured panes.
    //!
    //! The result is valid for the thickness that the interlayer has in the laminate.
    [[nodiscard]] WithDiagnostics<Bulk> makeInterlayer(const ExternalProperties & laminate,
                                                       const ExternalProperties & frontPane,
                                                       const ExternalProperties & backPane,
                                                       const ActionOptions & options = {});

    //! \brief The same interlayer at a different thickness.
    //!
    //! newThickness must be given in the unit of the thickness of the interlayer.
    [[nodiscard]] Interlayer withThickness(const Interlayer & interlayer, double newThickness);

    //! \brief Coatings that touch one interlayer of a laminate.
    //!
    //! onPaneInFront is a coating on the back surface of the pane in front of the interlayer.
    //! onPaneBehind is a coating on the front surface of the pane behind the interlayer. Each
    //! coating is given by a reference laminate. An empty member means an uncoated surface.
    struct EmbeddedCoatings
    {
        std::optional<ReferenceLaminate> onPaneInFront;
        std::optional<ReferenceLaminate> onPaneBehind;
    };

    //! \brief Laminate that is built one pane at a time, from the front to the back.
    //!
    //!     Laminate(frontPane).add(interlayer, backPane).add(interlayer, thirdPane).spectra()
    //!
    //! It is assumed that there is no reflection between an uncoated glass surface and an
    //! interlayer. A CoatedPane brings a coating that faces the air, so it can only be the
    //! first pane with the coating on its front side or the last pane with the coating on its
    //! back side. A coating that touches an interlayer is given in EmbeddedCoatings, and its
    //! pane is given as an UncoatedPane.
    class Laminate
    {
    public:
        explicit Laminate(const UncoatedPane & frontPane, const ActionOptions & options = {});
        explicit Laminate(const CoatedPane & frontPane, const ActionOptions & options = {});

        //! \brief This laminate with one more interlayer and one more pane behind it.
        //!
        //! Throws std::runtime_error if a coating that was measured in the air would touch the
        //! interlayer.
        [[nodiscard]] Laminate add(const Interlayer & interlayer,
                                   const UncoatedPane & pane,
                                   const EmbeddedCoatings & coatings = {}) const;
        [[nodiscard]] Laminate add(const Interlayer & interlayer,
                                   const CoatedPane & pane,
                                   const EmbeddedCoatings & coatings = {}) const;

        //! \brief Measured properties that the laminate would have, with the diagnostics of
        //! all its panes and coatings.
        [[nodiscard]] WithDiagnostics<ExternalProperties> spectra() const;

    private:
        //! Components of one pane. A coating in it faces the air.
        struct Pane
        {
            Monolithic glass;
            std::optional<Interface> coatingOnFront;
            std::optional<Interface> coatingOnBack;
            std::vector<Diagnostic> diagnostics;
        };

        [[nodiscard]] static Pane
          paneOf(const UncoatedPane & pane, const std::string & name, const ActionOptions & options);
        [[nodiscard]] static Pane
          paneOf(const CoatedPane & pane, const std::string & name, const ActionOptions & options);

        Laminate(const Pane & frontPane, const ActionOptions & options);

        [[nodiscard]] std::string nextPaneName() const;
        [[nodiscard]] Laminate
          added(const Interlayer & interlayer, const Pane & pane, const EmbeddedCoatings & coatings) const;

        //! All components except the last interface, which depends on what follows the last pane.
        Stack m_Stack;
        Monolithic m_LastGlass;
        std::optional<Interface> m_CoatingOnLastBack;
        std::vector<Diagnostic> m_Diagnostics;
        ActionOptions m_Options;
        size_t m_NumberOfPanes{1};
    };
}   // namespace CompoundProduct
