#include <algorithm>
#include <iterator>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <variant>

#include "Actions.hpp"
#include "Construction.hpp"
#include "Deconstruction.hpp"

using FenestrationCommon::Side;

namespace CompoundProduct
{
    namespace
    {
        //! Names of the two measurements of a coated pane, as they are reported in diagnostics.
        struct CoatedPaneNames
        {
            std::string measured;
            std::string substrate;
        };

        [[nodiscard]] CoatedPaneNames namesOf(const std::string & coatedPane)
        {
            return {.measured = coatedPane + ".measured", .substrate = coatedPane + ".substrate"};
        }

        //! Diagnostics of one step, marked with the name of the input that the step used.
        [[nodiscard]] std::vector<Diagnostic> from(const std::string & source,
                                                   const std::vector<Diagnostic> & diagnostics)
        {
            std::vector<Diagnostic> result;
            std::ranges::transform(diagnostics,
                                   std::back_inserter(result),
                                   [&](const Diagnostic & diagnostic) {
                                       return Diagnostic{.wavelength = diagnostic.wavelength,
                                                         .warning = diagnostic.warning,
                                                         .source = source};
                                   });
            return result;
        }

        [[nodiscard]] std::vector<Diagnostic>
          allDiagnostics(const std::vector<std::vector<Diagnostic>> & steps)
        {
            std::vector<Diagnostic> result;
            std::ranges::copy(steps | std::views::join, std::back_inserter(result));
            return result;
        }

        //! Internal properties of an uncoated pane that the caller gave under the given name.
        [[nodiscard]] WithDiagnostics<Monolithic> glassOf(const ExternalProperties & uncoatedPane,
                                                          const std::string & name,
                                                          const ActionOptions & options)
        {
            const auto glass{
              uncoatedPane.deconstructMonolithic(options.substrateReflectance, options.tolerances)};
            return {.value = glass.value, .diagnostics = from(name, glass.diagnostics)};
        }

        [[nodiscard]] Bulk bulkOf(const Monolithic & pane)
        {
            return {.internalTransmittance = pane.internalTransmittance};
        }

        [[nodiscard]] Bulk bulkOf(const Interlayer & interlayer)
        {
            return {.internalTransmittance = interlayer.internalTransmittance};
        }

        //! Pane with the given glass and with the coating on one of its sides.
        [[nodiscard]] ExternalProperties coatedPaneOf(const Interface & glassSurface,
                                                      const Bulk & glass,
                                                      const Interface & coating,
                                                      const Side coatedSide)
        {
            if(coatedSide == Side::Front)
            {
                return Assembly(coating).add(glass, glassSurface).externalProperties();
            }
            return Assembly(glassSurface).add(glass, coating).externalProperties();
        }

        //! Pane with the given glass and with the same bare surface on both sides.
        [[nodiscard]] ExternalProperties uncoatedPaneOf(const Interface & glassSurface,
                                                        const Bulk & glass)
        {
            return Assembly(glassSurface).add(glass, glassSurface).externalProperties();
        }

        //! Coating that faces the air, from the measurement of its pane and from the glass of
        //! that pane. The result is oriented for targetSide: its reflectance towards the glass
        //! stays towards the glass.
        [[nodiscard]] WithDiagnostics<Interface> coatingOf(const CoatedPane & source,
                                                           const Monolithic & sourceGlass,
                                                           const Side targetSide,
                                                           const std::string & measuredName,
                                                           const ActionOptions & options)
        {
            const auto coating{source.measured.deconstructCoating(
              sourceGlass, source.coatedSide, options.tolerances)};
            return {.value = source.coatedSide == targetSide ? coating.value
                                                             : coating.value.flipped(),
                    .diagnostics = from(measuredName, coating.diagnostics)};
        }

        //! The same, with the deconstruction of the substrate of the source pane.
        [[nodiscard]] WithDiagnostics<Interface> coatingOf(const CoatedPane & source,
                                                           const Side targetSide,
                                                           const CoatedPaneNames & names,
                                                           const ActionOptions & options)
        {
            const auto glass{glassOf(source.substrate, names.substrate, options)};
            const auto coating{coatingOf(source, glass.value, targetSide, names.measured, options)};
            return {.value = coating.value,
                    .diagnostics = allDiagnostics({glass.diagnostics, coating.diagnostics})};
        }

        //! Uncoated substrate, given under substrateName, with the coating of the source pane.
        [[nodiscard]] WithDiagnostics<ExternalProperties>
          coatedSubstrate(const ExternalProperties & substrate,
                          const std::string & substrateName,
                          const CoatedPane & coatingSource,
                          const CoatedPaneNames & sourceNames,
                          const Side coatedSide,
                          const ActionOptions & options)
        {
            const auto coating{coatingOf(coatingSource, coatedSide, sourceNames, options)};
            const auto glass{glassOf(substrate, substrateName, options)};
            return {.value = coatedPaneOf(uncoatedInterface(glass.value),
                                          bulkOf(glass.value),
                                          coating.value,
                                          coatedSide),
                    .diagnostics = allDiagnostics({coating.diagnostics, glass.diagnostics})};
        }
    }   // namespace

    WithDiagnostics<UncoatedPane> UncoatedPane::atThickness(const double newThickness,
                                                            const ActionOptions & options) const
    {
        const auto glass{glassOf(measured, "measured", options)};
        const auto resized{withThickness(bulkOf(glass.value), thickness, newThickness)};
        return {.value = {.measured = uncoatedPaneOf(uncoatedInterface(glass.value), resized),
                          .thickness = newThickness},
                .diagnostics = glass.diagnostics};
    }

    WithDiagnostics<CoatedPane> UncoatedPane::withCoating(const CoatedPane & coatingSource,
                                                          const Side coatedSide,
                                                          const ActionOptions & options) const
    {
        const auto coated{coatedSubstrate(
          measured, "measured", coatingSource, namesOf("coatingSource"), coatedSide, options)};
        return {.value = {.measured = coated.value,
                          .substrate = measured,
                          .coatedSide = coatedSide,
                          .thickness = thickness},
                .diagnostics = coated.diagnostics};
    }

    WithDiagnostics<CoatedPane> CoatedPane::atThickness(const double newThickness,
                                                        const ActionOptions & options) const
    {
        const auto glass{glassOf(substrate, "substrate", options)};
        const auto coating{coatingOf(*this, glass.value, coatedSide, "measured", options)};
        const auto surface{uncoatedInterface(glass.value)};
        const auto resized{withThickness(bulkOf(glass.value), thickness, newThickness)};
        return {.value = {.measured =
                            coatedPaneOf(surface, resized, coating.value, coatedSide),
                          .substrate = uncoatedPaneOf(surface, resized),
                          .coatedSide = coatedSide,
                          .thickness = newThickness},
                .diagnostics = allDiagnostics({glass.diagnostics, coating.diagnostics})};
    }

    WithDiagnostics<CoatedPane> CoatedPane::onSubstrate(const UncoatedPane & newSubstrate,
                                                        const ActionOptions & options) const
    {
        const CoatedPaneNames names{.measured = "measured", .substrate = "substrate"};
        const auto coated{coatedSubstrate(
          newSubstrate.measured, "newSubstrate.measured", *this, names, coatedSide, options)};
        return {.value = {.measured = coated.value,
                          .substrate = newSubstrate.measured,
                          .coatedSide = coatedSide,
                          .thickness = newSubstrate.thickness},
                .diagnostics = coated.diagnostics};
    }

    WithDiagnostics<ExternalProperties> coatBothSides(const ExternalProperties & substrate,
                                                      const CoatedPane & frontCoatingSource,
                                                      const CoatedPane & backCoatingSource,
                                                      const ActionOptions & options)
    {
        const auto front{
          coatingOf(frontCoatingSource, Side::Front, namesOf("frontCoatingSource"), options)};
        const auto back{
          coatingOf(backCoatingSource, Side::Back, namesOf("backCoatingSource"), options)};
        const auto glass{glassOf(substrate, "substrate", options)};
        const auto coated{Assembly(front.value).add(bulkOf(glass.value), back.value)};
        return {.value = coated.externalProperties(),
                .diagnostics = allDiagnostics(
                  {front.diagnostics, back.diagnostics, glass.diagnostics})};
    }

    WithDiagnostics<Interlayer> makeInterlayer(const ExternalProperties & laminate,
                                               const ExternalProperties & frontPane,
                                               const ExternalProperties & backPane,
                                               const double interlayerThickness,
                                               const ActionOptions & options)
    {
        const auto front{glassOf(frontPane, "frontPane", options)};
        const auto back{glassOf(backPane, "backPane", options)};
        const auto interlayer{
          laminate.deconstructInterlayer(front.value, back.value, options.tolerances)};
        return {.value = {.internalTransmittance = interlayer.value.internalTransmittance,
                          .thickness = interlayerThickness},
                .diagnostics = allDiagnostics({front.diagnostics,
                                               back.diagnostics,
                                               from("laminate", interlayer.diagnostics)})};
    }

    Interlayer Interlayer::atThickness(const double newThickness) const
    {
        return {.internalTransmittance =
                  withThickness(bulkOf(*this), thickness, newThickness).internalTransmittance,
                .thickness = newThickness};
    }

    namespace
    {
        void requireNoCoatingFromAir(const std::optional<Interface> & coating)
        {
            if(coating.has_value())
            {
                throw std::runtime_error(
                  "Compound product: a coating of a coated pane was measured in the air and "
                  "cannot touch an interlayer. Give the pane as an uncoated pane and give the "
                  "coating as an embedded coating.");
            }
        }
    }   // namespace

    Laminate::Pane Laminate::paneOf(const UncoatedPane & pane,
                                    const std::string & name,
                                    const ActionOptions & options)
    {
        const auto glass{glassOf(pane.measured, name + ".measured", options)};
        return {.glass = glass.value,
                .coatingOnFront = std::nullopt,
                .coatingOnBack = std::nullopt,
                .diagnostics = glass.diagnostics};
    }

    Laminate::Pane Laminate::paneOf(const CoatedPane & pane,
                                    const std::string & name,
                                    const ActionOptions & options)
    {
        const auto glass{glassOf(pane.substrate, name + ".substrate", options)};
        const auto coating{
          coatingOf(pane, glass.value, pane.coatedSide, name + ".measured", options)};
        const auto onFront{pane.coatedSide == Side::Front};
        return {.glass = glass.value,
                .coatingOnFront = onFront ? std::optional{coating.value} : std::nullopt,
                .coatingOnBack = onFront ? std::nullopt : std::optional{coating.value},
                .diagnostics = allDiagnostics({glass.diagnostics, coating.diagnostics})};
    }

    Laminate::Laminate(const Pane & frontPane, const ActionOptions & options) :
        m_InFrontOfLastGlass{
          frontPane.coatingOnFront.value_or(uncoatedInterface(frontPane.glass))},
        m_LastGlass{frontPane.glass},
        m_CoatingOnLastBack{frontPane.coatingOnBack},
        m_Diagnostics{frontPane.diagnostics},
        m_Options{options}
    {}

    Laminate::Laminate(const UncoatedPane & frontPane, const ActionOptions & options) :
        Laminate(paneOf(frontPane, "pane[0]", options), options)
    {}

    Laminate::Laminate(const CoatedPane & frontPane, const ActionOptions & options) :
        Laminate(paneOf(frontPane, "pane[0]", options), options)
    {}

    std::string Laminate::nextPaneName() const
    {
        return "pane[" + std::to_string(m_NumberOfPanes) + "]";
    }

    Laminate Laminate::added(const Interlayer & interlayer,
                             const Pane & pane,
                             const EmbeddedCoatings & coatings) const
    {
        requireNoCoatingFromAir(m_CoatingOnLastBack);
        requireNoCoatingFromAir(pane.coatingOnFront);
        const auto noReflection{
          nonReflectingInterface(m_LastGlass.surfaceReflectance.getXArray())};
        auto result{*this};
        result.m_InFrontOfLastGlass =
          m_InFrontOfLastGlass
            .add(bulkOf(m_LastGlass), coatings.onPaneInFront.value_or(noReflection))
            .add(bulkOf(interlayer), coatings.onPaneBehind.value_or(noReflection));
        result.m_LastGlass = pane.glass;
        result.m_CoatingOnLastBack = pane.coatingOnBack;
        result.m_Diagnostics = allDiagnostics({m_Diagnostics, pane.diagnostics});
        result.m_NumberOfPanes = m_NumberOfPanes + 1;
        return result;
    }

    Laminate Laminate::add(const Interlayer & interlayer,
                           const UncoatedPane & pane,
                           const EmbeddedCoatings & coatings) const
    {
        return added(interlayer, paneOf(pane, nextPaneName(), m_Options), coatings);
    }

    Laminate Laminate::add(const Interlayer & interlayer,
                           const CoatedPane & pane,
                           const EmbeddedCoatings & coatings) const
    {
        return added(interlayer, paneOf(pane, nextPaneName(), m_Options), coatings);
    }

    WithDiagnostics<ExternalProperties> Laminate::spectra() const
    {
        const auto backSurface{m_CoatingOnLastBack.value_or(uncoatedInterface(m_LastGlass))};
        const auto whole{m_InFrontOfLastGlass.add(bulkOf(m_LastGlass), backSurface)};
        return {.value = whole.externalProperties(), .diagnostics = m_Diagnostics};
    }
}   // namespace CompoundProduct
