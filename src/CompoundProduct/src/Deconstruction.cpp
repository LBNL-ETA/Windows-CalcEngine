#include <algorithm>
#include <cmath>
#include <iterator>
#include <optional>
#include <ranges>

#include "Construction.hpp"
#include "Deconstruction.hpp"
#include "SeriesUtilities.hpp"

using FenestrationCommon::CSeries;
using FenestrationCommon::Side;

namespace CompoundProduct
{
    namespace
    {
        ///////////////////////////////////////////////////////////////////////////
        /// Values at one wavelength
        ///////////////////////////////////////////////////////////////////////////

        //! External properties at one wavelength. All calculations use this one structure: for
        //! a measured product, for a part of it, and for one interface, which is the smallest
        //! part.
        struct ExternalPoint
        {
            double Tf{0};
            double Tb{0};
            double Rf{0};
            double Rb{0};
        };

        struct MonolithicPoint
        {
            double surfaceReflectance{0};
            double tau{0};
        };

        template<typename Point>
        struct PointResult
        {
            Point value;
            std::vector<Warning> warnings;
        };

        [[nodiscard]] ExternalPoint measuredAt(const ExternalProperties & measured,
                                               const size_t index)
        {
            return {.Tf = measured.Tf[index].value(),
                    .Tb = measured.Tb[index].value(),
                    .Rf = measured.Rf[index].value(),
                    .Rb = measured.Rb[index].value()};
        }

        [[nodiscard]] MonolithicPoint monolithicAt(const Monolithic & monolithic,
                                                   const size_t index)
        {
            return {.surfaceReflectance = monolithic.surfaceReflectance[index].value(),
                    .tau = monolithic.bulk.tau[index].value()};
        }

        ///////////////////////////////////////////////////////////////////////////
        /// Warnings and physical limits
        ///////////////////////////////////////////////////////////////////////////

        [[nodiscard]] std::vector<Warning> appended(const std::vector<Warning> & warnings,
                                                    const bool condition,
                                                    const Warning warning)
        {
            auto result{warnings};
            if(condition)
            {
                result.push_back(warning);
            }
            return result;
        }

        [[nodiscard]] PointResult<double> physical(const double value,
                                                   const std::vector<Warning> & warnings,
                                                   const Tolerances & tolerances)
        {
            if(!tolerances.clampToPhysical || Helper::isPhysical(value))
            {
                return {.value = value, .warnings = warnings};
            }
            return {.value = std::clamp(value, 0.0, 1.0),
                    .warnings = appended(warnings, true, Warning::UnphysicalClamped)};
        }

        [[nodiscard]] PointResult<ExternalPoint> physical(const ExternalPoint & point,
                                                           const std::vector<Warning> & warnings,
                                                           const Tolerances & tolerances)
        {
            const auto allPhysical{Helper::isPhysical(point.Tf)
                                   && Helper::isPhysical(point.Tb)
                                   && Helper::isPhysical(point.Rf)
                                   && Helper::isPhysical(point.Rb)};
            if(!tolerances.clampToPhysical || allPhysical)
            {
                return {.value = point, .warnings = warnings};
            }
            return {.value = {.Tf = std::clamp(point.Tf, 0.0, 1.0),
                              .Tb = std::clamp(point.Tb, 0.0, 1.0),
                              .Rf = std::clamp(point.Rf, 0.0, 1.0),
                              .Rb = std::clamp(point.Rb, 0.0, 1.0)},
                    .warnings = appended(warnings, true, Warning::UnphysicalClamped)};
        }

        //! True if both properties of the monolithic are zero, which means that there is no data.
        [[nodiscard]] bool noData(const MonolithicPoint & point, const Tolerances & tolerances)
        {
            return std::abs(point.surfaceReflectance) < tolerances.limitOfDetection
                   && std::abs(point.tau) < tolerances.limitOfDetection;
        }

        ///////////////////////////////////////////////////////////////////////////
        /// General form: a known part is removed from a measured product.
        ///
        /// A part is a section of a product that begins and ends with an interface. It has the
        /// same four external properties as a measured product, so it is an ExternalPoint.
        ///////////////////////////////////////////////////////////////////////////

        [[nodiscard]] ExternalPoint flippedPoint(const ExternalPoint & point)
        {
            return {.Tf = point.Tb,
                    .Tb = point.Tf,
                    .Rf = point.Rb,
                    .Rb = point.Rf};
        }

        //! Transmittance of a product that has one transmittance by nature (an uncoated pane, or
        //! a product whose unknown is a bulk layer): the mean of the two measured ones.
        [[nodiscard]] double meanTransmittance(const ExternalPoint & point)
        {
            return 0.5 * (point.Tf + point.Tb);
        }

        //! The part of "whole" that remains when the known part "front" and the bulk layer behind
        //! it are removed. bulk is the internal transmittance of that bulk layer.
        [[nodiscard]] ExternalPoint withoutFrontPart(const ExternalPoint & whole,
                                                     const ExternalPoint & front,
                                                     const double bulk)
        {
            const auto bulkSquared{bulk * bulk};
            const auto added{whole.Rf - front.Rf};
            const auto reflectanceFront{
              added
              / Helper::nonZero(bulkSquared
                                * (front.Tf * front.Tb
                                   + front.Rb * added))};
            const auto interReflection{
              Helper::nonZero(1 - front.Rb * reflectanceFront * bulkSquared)};
            const auto transmittanceFront{whole.Tf * interReflection
                                          / Helper::nonZero(front.Tf * bulk)};
            const auto transmittanceBack{whole.Tb * interReflection
                                         / Helper::nonZero(front.Tb * bulk)};
            return {.Tf = transmittanceFront,
                    .Tb = transmittanceBack,
                    .Rf = reflectanceFront,
                    .Rb = whole.Rb
                                       - transmittanceFront * transmittanceBack
                                           * front.Rb * bulkSquared
                                           / interReflection};
        }

        //! The part of "whole" that remains when the known part "back" and the bulk layer in
        //! front of it are removed.
        [[nodiscard]] ExternalPoint withoutBackPart(const ExternalPoint & whole,
                                                    const ExternalPoint & back,
                                                    const double bulk)
        {
            return flippedPoint(withoutFrontPart(flippedPoint(whole), flippedPoint(back), bulk));
        }

        //! Internal transmittance of the bulk layer between two known parts, from the
        //! transmittance of the whole product. Each direction of the light gives one equation
        //! for the same unknown. The mean of the two equations is solved.
        [[nodiscard]] double bulkBetween(const ExternalPoint & whole,
                                         const ExternalPoint & front,
                                         const ExternalPoint & back)
        {
            const auto transmittance{meanTransmittance(whole)};
            const auto linear{0.5
                              * (front.Tf * back.Tf
                                 + front.Tb * back.Tb)};
            const auto quadratic{transmittance * front.Rb * back.Rf};
            if(std::abs(quadratic) < Helper::SmallNumber)
            {
                return transmittance / Helper::nonZero(linear);
            }
            const auto discriminant{linear * linear + 4 * quadratic * transmittance};
            return (-linear + std::sqrt(discriminant)) / (2 * quadratic);
        }

        //! Known part together with the bulk layer between it and the unknown interface.
        struct KnownPartPoint
        {
            ExternalPoint part;
            double bulk{0};
        };

        //! The part that remains when the optional known parts in front of it and behind it
        //! are removed.
        [[nodiscard]] PointResult<ExternalPoint>
          remainingPartAt(const ExternalPoint & measured,
                                 const std::optional<KnownPartPoint> & front,
                                 const std::optional<KnownPartPoint> & back,
                                 const Tolerances & tol)
        {
            const auto opaque{[&](const std::optional<KnownPartPoint> & known) {
                return known.has_value() && std::abs(known->bulk) < tol.limitOfDetection;
            }};
            if(opaque(front) || opaque(back))
            {
                return {.value = {}, .warnings = {Warning::OpaqueLayer}};
            }
            const auto behindFront{
              front.has_value() ? withoutFrontPart(measured, front->part, front->bulk) : measured};
            const auto alone{
              back.has_value() ? withoutBackPart(behindFront, back->part, back->bulk)
                               : behindFront};
            return physical(alone, {}, tol);
        }

        [[nodiscard]] PointResult<double> deconstructBulkAt(const ExternalPoint & measured,
                                                            const ExternalPoint & front,
                                                            const ExternalPoint & back,
                                                            const Tolerances & tol)
        {
            if(std::abs(meanTransmittance(measured)) < tol.limitOfDetection)
            {
                return {.value = 0.0, .warnings = {Warning::OpaqueLayer}};
            }
            return physical(bulkBetween(measured, front, back), {}, tol);
        }

        ///////////////////////////////////////////////////////////////////////////
        /// Monolithic
        ///////////////////////////////////////////////////////////////////////////

        [[nodiscard]] double chosenReflectance(const ExternalPoint & measured,
                                               const ReflectanceChoice choice)
        {
            switch(choice)
            {
                case ReflectanceChoice::Front:
                    return measured.Rf;
                case ReflectanceChoice::Back:
                    return measured.Rb;
                case ReflectanceChoice::Average:
                default:
                    return 0.5 * (measured.Rf + measured.Rb);
            }
        }

        [[nodiscard]] double interfaceReflectance(const double transmittance,
                                                  const double reflectance)
        {
            const auto beta{transmittance * transmittance - reflectance * reflectance
                            + 2 * reflectance + 1};
            const auto discriminant{
              std::max(beta * beta - 4 * (2 - reflectance) * reflectance, 0.0)};
            return (beta - std::sqrt(discriminant)) / (2 * (2 - reflectance));
        }

        [[nodiscard]] double internalTransmittance(const double transmittance,
                                                   const double reflectance,
                                                   const double surfaceReflectance)
        {
            const auto denominator{surfaceReflectance * transmittance};
            if(std::abs(denominator) < Helper::SmallNumber)
            {
                // Surfaces do not reflect and all of the loss is in the bulk material
                const auto surfaceTransmittance{1.0 - surfaceReflectance};
                return transmittance / Helper::nonZero(surfaceTransmittance * surfaceTransmittance);
            }
            return (reflectance - surfaceReflectance) / denominator;
        }

        [[nodiscard]] PointResult<MonolithicPoint> deconstructMonolithicAt(
          const ExternalPoint & measured, const ReflectanceChoice choice, const Tolerances & tol)
        {
            const auto reflectance{chosenReflectance(measured, choice)};
            const auto transmittance{meanTransmittance(measured)};
            const auto opaque{std::abs(transmittance) < tol.limitOfDetection};
            if(opaque && std::abs(measured.Rf) < tol.limitOfDetection
               && std::abs(measured.Rb) < tol.limitOfDetection)
            {
                return {.value = {}, .warnings = {Warning::NoData}};
            }
            if(opaque)
            {
                return {.value = {.surfaceReflectance = reflectance, .tau = 0.0},
                        .warnings = {Warning::OpaqueLayer}};
            }
            const auto surface{interfaceReflectance(transmittance, reflectance)};
            const auto [tau, warnings]{
              physical(internalTransmittance(transmittance, reflectance, surface), {}, tol)};
            return {.value = {.surfaceReflectance = surface, .tau = tau},
                    .warnings = warnings};
        }

        ///////////////////////////////////////////////////////////////////////////
        /// Rules of the cases
        ///
        /// A case function calls a general function and then applies the rules of Optics 5.1 for
        /// its type of product at every wavelength. "general" is the result of the general
        /// function at one wavelength, calculated without the correction to the physical limits.
        ///////////////////////////////////////////////////////////////////////////

        //! Coating on the back side of a substrate. Rf of the result is the reflectance of the
        //! coating seen from the substrate, Rb is the one seen from the air.
        [[nodiscard]] PointResult<ExternalPoint> coatingRules(const ExternalPoint & general,
                                                              const ExternalPoint & coated,
                                                              const MonolithicPoint & substrate,
                                                              const Tolerances & tol)
        {
            if(noData(substrate, tol))
            {
                return {.value = {}, .warnings = {Warning::NoData}};
            }
            if(std::abs(substrate.tau) < tol.limitOfDetection)
            {
                // Nothing is known about the substrate side. Coating is assumed to be opaque.
                return {.value = {.Tf = 0.0, .Tb = 0.0, .Rf = 1.0, .Rb = coated.Rb},
                        .warnings = {Warning::OpaqueSubstrate}};
            }
            const auto insensitive{substrate.tau < tol.sensitivityThreshold};
            return physical(general,
                            appended({}, insensitive, Warning::InsensitiveCoatingSubstrateSide),
                            tol);
        }

        [[nodiscard]] PointResult<ExternalPoint>
          flippedResult(const PointResult<ExternalPoint> & result)
        {
            return {.value = flippedPoint(result.value), .warnings = result.warnings};
        }

        //! Coating on the front side of a substrate. The product is turned around, the rules for
        //! a coating on the back side are applied, and the result is turned back.
        [[nodiscard]] PointResult<ExternalPoint>
          frontCoatingRules(const ExternalPoint & general,
                            const ExternalPoint & coated,
                            const MonolithicPoint & substrate,
                            const Tolerances & tol)
        {
            return flippedResult(
              coatingRules(flippedPoint(general), flippedPoint(coated), substrate, tol));
        }

        //! Interlayer between two uncoated panes.
        [[nodiscard]] PointResult<double> interlayerRules(const double general,
                                                          const ExternalPoint & laminate,
                                                          const MonolithicPoint & front,
                                                          const MonolithicPoint & back,
                                                          const Tolerances & tol)
        {
            if(noData(front, tol) || noData(back, tol))
            {
                return {.value = 0.0, .warnings = {Warning::NoData}};
            }
            const auto panes{front.tau * back.tau};
            if(std::abs(meanTransmittance(laminate)) < tol.limitOfDetection
               || panes < Helper::SmallNumber)
            {
                return {.value = 0.0, .warnings = {Warning::InsensitiveInterlayer}};
            }
            const auto insensitive{std::sqrt(panes) < tol.interlayerSensitivityThreshold};
            return physical(
              general, appended({}, insensitive, Warning::InsensitiveInterlayer), tol);
        }

        //! Values that cannot be determined from the measurement are set to zero.
        [[nodiscard]] ExternalPoint withoutInsensitive(const ExternalPoint & coating,
                                                       const bool interlayerSide,
                                                       const bool substrateSide)
        {
            return {.Tf = interlayerSide ? 0.0 : coating.Tf,
                    .Tb = interlayerSide ? 0.0 : coating.Tb,
                    .Rf = substrateSide ? 0.0 : coating.Rf,
                    .Rb = interlayerSide ? 0.0 : coating.Rb};
        }

        //! Coating between the front pane and the interlayer of a laminate. Rf of the result is
        //! the reflectance seen from the front pane, Rb is the one seen from the interlayer.
        [[nodiscard]] PointResult<ExternalPoint>
          embeddedCoatingRules(const ExternalPoint & general,
                               const MonolithicPoint & front,
                               const MonolithicPoint & back,
                               const double interlayer,
                               const Tolerances & tol)
        {
            if(noData(front, tol) || noData(back, tol))
            {
                return {.value = {}, .warnings = {Warning::NoData}};
            }
            const auto opaque{
              [&](const double value) { return std::abs(value) < tol.limitOfDetection; }};
            if(opaque(front.tau) || opaque(interlayer)
               || opaque(back.tau))
            {
                return {.value = {}, .warnings = {Warning::OpaqueLayer}};
            }
            const auto interlayerSide{
              interlayer * back.tau < tol.sensitivityThreshold};
            const auto substrateSide{front.tau < tol.sensitivityThreshold};
            const auto warnings{
              appended(appended({}, interlayerSide, Warning::InsensitiveCoatingInterlayerSide),
                       substrateSide,
                       Warning::InsensitiveCoatingSubstrateSide)};
            return physical(
              withoutInsensitive(general, interlayerSide, substrateSide), warnings, tol);
        }

        //! Outer interface of a thick layer together with internal transmittance of the layer.
        struct ThickLayerPoint
        {
            ExternalPoint surface;
            double tau{0};
        };

        [[nodiscard]] ThickLayerPoint thickLayerAt(const Interface & surface,
                                                   const Bulk & bulk,
                                                   const size_t index)
        {
            return {.surface = {.Tf = surface.tf[index].value(),
                                .Tb = surface.tb[index].value(),
                                .Rf = surface.rf[index].value(),
                                .Rb = surface.rb[index].value()},
                    .tau = bulk.tau[index].value()};
        }

        [[nodiscard]] bool noData(const ThickLayerPoint & layer, const Tolerances & tolerances)
        {
            return std::abs(layer.surface.Tf) < tolerances.limitOfDetection
                   && std::abs(layer.surface.Tb) < tolerances.limitOfDetection
                   && std::abs(layer.tau) < tolerances.limitOfDetection;
        }

        //! Interface between two thick layers that are in direct contact.
        [[nodiscard]] PointResult<ExternalPoint>
          contactInterfaceRules(const ExternalPoint & general,
                                const ThickLayerPoint & front,
                                const ThickLayerPoint & back,
                                const Tolerances & tol)
        {
            if(noData(front, tol) || noData(back, tol))
            {
                return {.value = {}, .warnings = {Warning::NoData}};
            }
            if(std::abs(front.tau) < tol.limitOfDetection
               || std::abs(back.tau) < tol.limitOfDetection)
            {
                return {.value = {}, .warnings = {Warning::OpaqueLayer}};
            }
            return physical(general, {}, tol);
        }

        ///////////////////////////////////////////////////////////////////////////
        /// From values at single wavelengths to series
        ///////////////////////////////////////////////////////////////////////////

        //! Calculates result at every wavelength. atIndex returns PointResult for given index.
        template<typename Point, typename AtIndex>
        [[nodiscard]] std::vector<PointResult<Point>> evaluate(const size_t size, AtIndex atIndex)
        {
            std::vector<PointResult<Point>> points(size);
            std::ranges::transform(std::views::iota(size_t{0}, size), points.begin(), atIndex);
            return points;
        }

        template<typename Point>
        [[nodiscard]] std::vector<Diagnostic>
          diagnosticsOf(const std::vector<double> & wavelengths,
                        const std::vector<PointResult<Point>> & points)
        {
            std::vector<Diagnostic> result;
            for(const auto index : std::views::iota(size_t{0}, points.size()))
            {
                std::ranges::transform(points[index].warnings,
                                       std::back_inserter(result),
                                       [&](const Warning warning) {
                                           return Diagnostic{.wavelength = wavelengths[index],
                                                             .warning = warning};
                                       });
            }
            return result;
        }

        [[nodiscard]] WithDiagnostics<ExternalProperties>
          toExternalProperties(const std::vector<double> & wavelengths,
                               const std::vector<PointResult<ExternalPoint>> & points)
        {
            const auto series{[&](double ExternalPoint::* member) {
                return Helper::makeSeries(
                  wavelengths, [&](const size_t index) { return points[index].value.*member; });
            }};
            return {.value = {.Tf = series(&ExternalPoint::Tf),
                              .Tb = series(&ExternalPoint::Tb),
                              .Rf = series(&ExternalPoint::Rf),
                              .Rb = series(&ExternalPoint::Rb)},
                    .diagnostics = diagnosticsOf(wavelengths, points)};
        }

        [[nodiscard]] std::vector<const CSeries *> seriesOf(const ExternalProperties & measured)
        {
            return {&measured.Tf,
                    &measured.Tb,
                    &measured.Rf,
                    &measured.Rb};
        }

        [[nodiscard]] std::vector<const CSeries *>
          seriesOf(const ExternalProperties & measured,
                   const std::vector<const Monolithic *> & monolithics)
        {
            auto result{seriesOf(measured)};
            for(const auto * monolithic : monolithics)
            {
                result.push_back(&monolithic->surfaceReflectance);
                result.push_back(&monolithic->bulk.tau);
            }
            return result;
        }

        //! Known part of a product together with the bulk layer between it and the part that
        //! remains when it is removed.
        struct KnownPart
        {
            ExternalProperties externalProperties;
            Bulk bulkToRemainingPart;
        };

        [[nodiscard]] std::vector<const CSeries *> seriesOf(const KnownPart & known)
        {
            auto result{seriesOf(known.externalProperties)};
            result.push_back(&known.bulkToRemainingPart.tau);
            return result;
        }

        [[nodiscard]] std::optional<KnownPartPoint> knownPartAt(const KnownPart * known,
                                                                const size_t index)
        {
            if(known == nullptr)
            {
                return std::nullopt;
            }
            return KnownPartPoint{
              .part = measuredAt(known->externalProperties, index),
              .bulk = known->bulkToRemainingPart.tau[index].value()};
        }

        //! The part that remains when the known parts are removed from the measured product. A
        //! side without a known part is a null pointer.
        [[nodiscard]] WithDiagnostics<ExternalProperties>
          remainingPart(const ExternalProperties & measured,
                        const KnownPart * frontPart,
                        const KnownPart * backPart,
                        const Tolerances & tolerances)
        {
            auto allSeries{seriesOf(measured)};
            for(const auto * known : {frontPart, backPart})
            {
                if(known != nullptr)
                {
                    std::ranges::copy(seriesOf(*known), std::back_inserter(allSeries));
                }
            }
            Helper::requireSameWavelengths(allSeries);
            const auto wavelengths{measured.Tf.getXArray()};
            const auto points{
              evaluate<ExternalPoint>(wavelengths.size(), [&](const size_t index) {
                  return remainingPartAt(measuredAt(measured, index),
                                                knownPartAt(frontPart, index),
                                                knownPartAt(backPart, index),
                                                tolerances);
              })};
            return toExternalProperties(wavelengths, points);
        }

        //! A part that consists of one interface, given as an Interface. The four properties are
        //! the same. Only the type and the names of the members change.
        [[nodiscard]] WithDiagnostics<Interface>
          asInterface(const WithDiagnostics<ExternalProperties> & part)
        {
            return {.value = {.tf = part.value.Tf,
                              .tb = part.value.Tb,
                              .rf = part.value.Rf,
                              .rb = part.value.Rb},
                    .diagnostics = part.diagnostics};
        }
        [[nodiscard]] WithDiagnostics<Bulk>
          toBulk(const std::vector<double> & wavelengths,
                 const std::vector<PointResult<double>> & points,
                 const double thickness)
        {
            return {.value = {.tau = Helper::makeSeries(
                                wavelengths,
                                [&](const size_t index) { return points[index].value; }),
                              .thickness = thickness},
                    .diagnostics = diagnosticsOf(wavelengths, points)};
        }

        //! Applies the rules of a case to the result of a general function. rule gets the index
        //! of the wavelength and the general result at that wavelength.
        template<typename Rule>
        [[nodiscard]] WithDiagnostics<ExternalProperties>
          withRules(const ExternalProperties & general, Rule rule)
        {
            const auto wavelengths{general.Tf.getXArray()};
            const auto points{
              evaluate<ExternalPoint>(wavelengths.size(), [&](const size_t index) {
                  return rule(index, measuredAt(general, index));
              })};
            return toExternalProperties(wavelengths, points);
        }

        template<typename Rule>
        [[nodiscard]] WithDiagnostics<Bulk> withRules(const Bulk & general, Rule rule)
        {
            const auto & tau{general.tau};
            const auto wavelengths{tau.getXArray()};
            const auto points{evaluate<double>(wavelengths.size(), [&](const size_t index) {
                return rule(index, tau[index].value());
            })};
            return toBulk(wavelengths, points, general.thickness);
        }

        //! The general functions are called without the correction to the physical limits. A
        //! case function applies it after its own rules.
        [[nodiscard]] Tolerances withoutCorrection(const Tolerances & tolerances)
        {
            auto result{tolerances};
            result.clampToPhysical = false;
            return result;
        }

        //! Surface of a monolithic towards the air, as a known part.
        [[nodiscard]] Assembly bareSurfaceOf(const Monolithic & monolithic)
        {
            return Assembly(uncoatedInterface(monolithic));
        }

        //! Back pane of a laminate as a known part: an interface to the interlayer that does not
        //! reflect, the glass, and the bare surface towards the air.
        [[nodiscard]] Assembly paneBehindInterlayer(const Monolithic & pane)
        {
            return Assembly(nonReflectingInterface(pane.bulk.tau.getXArray()))
              .add(pane.bulk, uncoatedInterface(pane));
        }
    }   // namespace

    Interface uncoatedInterface(const Monolithic & substrate)
    {
        return {.tf = 1.0 - substrate.surfaceReflectance,
                .tb = 1.0 - substrate.surfaceReflectance,
                .rf = substrate.surfaceReflectance,
                .rb = substrate.surfaceReflectance};
    }

    WithDiagnostics<ExternalProperties>
      ExternalProperties::deconstructPartInTheMiddle(const Assembly & frontPart,
                                                     const Bulk & frontBulk,
                                                     const Bulk & backBulk,
                                                     const Assembly & backPart,
                                                     const Tolerances & tolerances) const
    {
        const KnownPart front{.externalProperties = frontPart.externalProperties(),
                              .bulkToRemainingPart = frontBulk};
        const KnownPart back{.externalProperties = backPart.externalProperties(),
                             .bulkToRemainingPart = backBulk};
        return remainingPart(*this, &front, &back, tolerances);
    }

    WithDiagnostics<ExternalProperties>
      ExternalProperties::deconstructPartBehind(const Assembly & frontPart,
                                                const Bulk & bulk,
                                                const Tolerances & tolerances) const
    {
        const KnownPart front{.externalProperties = frontPart.externalProperties(),
                              .bulkToRemainingPart = bulk};
        return remainingPart(*this, &front, nullptr, tolerances);
    }

    WithDiagnostics<ExternalProperties>
      ExternalProperties::deconstructPartInFrontOf(const Bulk & bulk,
                                                   const Assembly & backPart,
                                                   const Tolerances & tolerances) const
    {
        const KnownPart back{.externalProperties = backPart.externalProperties(),
                             .bulkToRemainingPart = bulk};
        return remainingPart(*this, nullptr, &back, tolerances);
    }

    WithDiagnostics<Bulk> ExternalProperties::deconstructBulk(const Assembly & frontPart,
                                                              const Assembly & backPart,
                                                              const double thickness,
                                                              const Tolerances & tolerances) const
    {
        const auto front{frontPart.externalProperties()};
        const auto back{backPart.externalProperties()};
        auto allSeries{seriesOf(*this)};
        std::ranges::copy(seriesOf(front), std::back_inserter(allSeries));
        std::ranges::copy(seriesOf(back), std::back_inserter(allSeries));
        Helper::requireSameWavelengths(allSeries);
        const auto wavelengths{Tf.getXArray()};
        const auto points{evaluate<double>(wavelengths.size(), [&](const size_t index) {
            return deconstructBulkAt(measuredAt(*this, index),
                                     measuredAt(front, index),
                                     measuredAt(back, index),
                                     tolerances);
        })};
        return toBulk(wavelengths, points, thickness);
    }

    WithDiagnostics<Monolithic>
      ExternalProperties::deconstructMonolithic(const double thickness,
                                                const ReflectanceChoice choice,
                                                const Tolerances & tolerances) const
    {
        Helper::requireSameWavelengths(seriesOf(*this));
        const auto wavelengths{Tf.getXArray()};
        const auto points{
          evaluate<MonolithicPoint>(wavelengths.size(), [&](const size_t index) {
              return deconstructMonolithicAt(measuredAt(*this, index), choice, tolerances);
          })};
        const auto series{[&](double MonolithicPoint::* member) {
            return Helper::makeSeries(
              wavelengths, [&](const size_t index) { return points[index].value.*member; });
        }};
        return {.value = {.surfaceReflectance = series(&MonolithicPoint::surfaceReflectance),
                          .bulk = {.tau = series(&MonolithicPoint::tau),
                                   .thickness = thickness}},
                .diagnostics = diagnosticsOf(wavelengths, points)};
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Cases of the sources. Each of them calls a general function and applies its rules.
    ///////////////////////////////////////////////////////////////////////////

    WithDiagnostics<Interface>
      ExternalProperties::deconstructCoating(const Monolithic & substrate,
                                             const Side coatedSide,
                                             const Tolerances & tolerances) const
    {
        Helper::requireSameWavelengths(seriesOf(*this, {&substrate}));
        const auto generalTolerances{withoutCorrection(tolerances)};
        if(coatedSide == Side::Front)
        {
            const auto coating{
              deconstructPartInFrontOf(substrate.bulk, bareSurfaceOf(substrate), generalTolerances)};
            return asInterface(
              withRules(coating.value, [&](const size_t index, const ExternalPoint & point) {
                  return frontCoatingRules(
                    point, measuredAt(*this, index), monolithicAt(substrate, index), tolerances);
              }));
        }
        const auto coating{
          deconstructPartBehind(bareSurfaceOf(substrate), substrate.bulk, generalTolerances)};
        return asInterface(
          withRules(coating.value, [&](const size_t index, const ExternalPoint & point) {
              return coatingRules(
                point, measuredAt(*this, index), monolithicAt(substrate, index), tolerances);
          }));
    }

    WithDiagnostics<Bulk>
      ExternalProperties::deconstructInterlayer(const Monolithic & front,
                                                const Monolithic & back,
                                                const double thickness,
                                                const Tolerances & tolerances) const
    {
        Helper::requireSameWavelengths(seriesOf(*this, {&front, &back}));
        const auto frontPane{paneBehindInterlayer(front).flipped()};
        const auto interlayer{deconstructBulk(
          frontPane, paneBehindInterlayer(back), thickness, withoutCorrection(tolerances))};
        return withRules(interlayer.value, [&](const size_t index, const double tau) {
            return interlayerRules(tau,
                                   measuredAt(*this, index),
                                   monolithicAt(front, index),
                                   monolithicAt(back, index),
                                   tolerances);
        });
    }

    WithDiagnostics<Interface>
      ExternalProperties::deconstructEmbeddedCoating(const Monolithic & front,
                                                     const Monolithic & back,
                                                     const Bulk & interlayer,
                                                     const Tolerances & tolerances) const
    {
        auto allSeries{seriesOf(*this, {&front, &back})};
        allSeries.push_back(&interlayer.tau);
        Helper::requireSameWavelengths(allSeries);
        const auto coating{deconstructPartInTheMiddle(bareSurfaceOf(front),
                                                      front.bulk,
                                                      interlayer,
                                                      paneBehindInterlayer(back),
                                                      withoutCorrection(tolerances))};
        return asInterface(
          withRules(coating.value, [&](const size_t index, const ExternalPoint & point) {
              return embeddedCoatingRules(point,
                                          monolithicAt(front, index),
                                          monolithicAt(back, index),
                                          interlayer.tau[index].value(),
                                          tolerances);
          }));
    }

    WithDiagnostics<Interface>
      ExternalProperties::deconstructDoubleMonolithic(const Interface & frontSurface,
                                                      const Bulk & frontBulk,
                                                      const Interface & backSurface,
                                                      const Bulk & backBulk,
                                                      const Tolerances & tolerances) const
    {
        const auto contact{deconstructPartInTheMiddle(Assembly(frontSurface),
                                                      frontBulk,
                                                      backBulk,
                                                      Assembly(backSurface),
                                                      withoutCorrection(tolerances))};
        return asInterface(
          withRules(contact.value, [&](const size_t index, const ExternalPoint & point) {
              return contactInterfaceRules(point,
                                           thickLayerAt(frontSurface, frontBulk, index),
                                           thickLayerAt(backSurface, backBulk, index),
                                           tolerances);
          }));
    }

    WithDiagnostics<Interface>
      ExternalProperties::deconstructDoubleMonolithic(const Monolithic & front,
                                                      const Monolithic & back,
                                                      const Tolerances & tolerances) const
    {
        return deconstructDoubleMonolithic(
          uncoatedInterface(front), front.bulk, uncoatedInterface(back), back.bulk, tolerances);
    }
}   // namespace CompoundProduct
