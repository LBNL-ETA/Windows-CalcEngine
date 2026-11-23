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

        struct MeasuredPoint
        {
            double transmittance{0};
            double reflectanceFront{0};
            double reflectanceBack{0};
        };

        struct MonolithicPoint
        {
            double surfaceReflectance{0};
            double internalTransmittance{0};
        };

        struct InterfacePoint
        {
            double transmittance{0};
            double reflectanceFront{0};
            double reflectanceBack{0};
        };

        template<typename Point>
        struct PointResult
        {
            Point value;
            std::vector<Warning> warnings;
        };

        [[nodiscard]] MeasuredPoint measuredAt(const ExternalProperties & measured,
                                               const size_t index)
        {
            return {.transmittance = measured.transmittance[index].value(),
                    .reflectanceFront = measured.reflectanceFront[index].value(),
                    .reflectanceBack = measured.reflectanceBack[index].value()};
        }

        [[nodiscard]] MonolithicPoint monolithicAt(const Monolithic & monolithic,
                                                   const size_t index)
        {
            return {.surfaceReflectance = monolithic.surfaceReflectance[index].value(),
                    .internalTransmittance = monolithic.internalTransmittance[index].value()};
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

        [[nodiscard]] PointResult<InterfacePoint> physical(const InterfacePoint & point,
                                                           const std::vector<Warning> & warnings,
                                                           const Tolerances & tolerances)
        {
            const auto allPhysical{Helper::isPhysical(point.transmittance)
                                   && Helper::isPhysical(point.reflectanceFront)
                                   && Helper::isPhysical(point.reflectanceBack)};
            if(!tolerances.clampToPhysical || allPhysical)
            {
                return {.value = point, .warnings = warnings};
            }
            return {.value = {.transmittance = std::clamp(point.transmittance, 0.0, 1.0),
                              .reflectanceFront = std::clamp(point.reflectanceFront, 0.0, 1.0),
                              .reflectanceBack = std::clamp(point.reflectanceBack, 0.0, 1.0)},
                    .warnings = appended(warnings, true, Warning::UnphysicalClamped)};
        }

        //! True if both properties of the monolithic are zero, which means that there is no data.
        [[nodiscard]] bool noData(const MonolithicPoint & point, const Tolerances & tolerances)
        {
            return std::abs(point.surfaceReflectance) < tolerances.limitOfDetection
                   && std::abs(point.internalTransmittance) < tolerances.limitOfDetection;
        }

        ///////////////////////////////////////////////////////////////////////////
        /// General form: a known part is removed from a measured product.
        ///
        /// A part is a section of a product that begins and ends with an interface. It has the
        /// same three external properties as a measured product, so it is a MeasuredPoint.
        ///////////////////////////////////////////////////////////////////////////

        [[nodiscard]] MeasuredPoint flippedPoint(const MeasuredPoint & point)
        {
            return {.transmittance = point.transmittance,
                    .reflectanceFront = point.reflectanceBack,
                    .reflectanceBack = point.reflectanceFront};
        }

        //! Surface of uncoated glass towards the air, as a part.
        [[nodiscard]] MeasuredPoint bareSurface(const double reflectance)
        {
            return {.transmittance = 1 - reflectance,
                    .reflectanceFront = reflectance,
                    .reflectanceBack = reflectance};
        }

        //! A part that consists of one interface.
        [[nodiscard]] InterfacePoint asInterface(const MeasuredPoint & part)
        {
            return {.transmittance = part.transmittance,
                    .reflectanceFront = part.reflectanceFront,
                    .reflectanceBack = part.reflectanceBack};
        }

        [[nodiscard]] MeasuredPoint asPart(const InterfacePoint & layerInterface)
        {
            return {.transmittance = layerInterface.transmittance,
                    .reflectanceFront = layerInterface.reflectanceFront,
                    .reflectanceBack = layerInterface.reflectanceBack};
        }

        //! The part of "whole" that remains when the known part "front" and the bulk layer behind
        //! it are removed. bulk is the internal transmittance of that bulk layer.
        [[nodiscard]] MeasuredPoint withoutFrontPart(const MeasuredPoint & whole,
                                                     const MeasuredPoint & front,
                                                     const double bulk)
        {
            const auto bulkSquared{bulk * bulk};
            const auto added{whole.reflectanceFront - front.reflectanceFront};
            const auto reflectanceFront{
              added
              / Helper::nonZero(bulkSquared
                                * (front.transmittance * front.transmittance
                                   + front.reflectanceBack * added))};
            const auto interReflection{
              Helper::nonZero(1 - front.reflectanceBack * reflectanceFront * bulkSquared)};
            const auto transmittance{whole.transmittance * interReflection
                                     / Helper::nonZero(front.transmittance * bulk)};
            const auto reflectanceBack{whole.reflectanceBack
                                       - transmittance * transmittance * front.reflectanceBack
                                           * bulkSquared / interReflection};
            return {.transmittance = transmittance,
                    .reflectanceFront = reflectanceFront,
                    .reflectanceBack = reflectanceBack};
        }

        //! The part of "whole" that remains when the known part "back" and the bulk layer in
        //! front of it are removed.
        [[nodiscard]] MeasuredPoint withoutBackPart(const MeasuredPoint & whole,
                                                    const MeasuredPoint & back,
                                                    const double bulk)
        {
            return flippedPoint(withoutFrontPart(flippedPoint(whole), flippedPoint(back), bulk));
        }

        //! Internal transmittance of the bulk layer between two known parts, from the
        //! transmittance of the whole product.
        [[nodiscard]] double bulkBetween(const double transmittance,
                                         const MeasuredPoint & front,
                                         const MeasuredPoint & back)
        {
            const auto linear{front.transmittance * back.transmittance};
            const auto quadratic{transmittance * front.reflectanceBack * back.reflectanceFront};
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
            MeasuredPoint part;
            double bulk{0};
        };

        //! Interface with optional known parts in front of it and behind it.
        [[nodiscard]] PointResult<InterfacePoint>
          deconstructInterfaceAt(const MeasuredPoint & measured,
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
            return physical(asInterface(alone), {}, tol);
        }

        [[nodiscard]] PointResult<double> deconstructBulkAt(const MeasuredPoint & measured,
                                                            const MeasuredPoint & front,
                                                            const MeasuredPoint & back,
                                                            const Tolerances & tol)
        {
            if(std::abs(measured.transmittance) < tol.limitOfDetection)
            {
                return {.value = 0.0, .warnings = {Warning::OpaqueLayer}};
            }
            return physical(bulkBetween(measured.transmittance, front, back), {}, tol);
        }

        ///////////////////////////////////////////////////////////////////////////
        /// Monolithic
        ///////////////////////////////////////////////////////////////////////////

        [[nodiscard]] double chosenReflectance(const MeasuredPoint & measured,
                                               const ReflectanceChoice choice)
        {
            switch(choice)
            {
                case ReflectanceChoice::Front:
                    return measured.reflectanceFront;
                case ReflectanceChoice::Back:
                    return measured.reflectanceBack;
                case ReflectanceChoice::Average:
                default:
                    return 0.5 * (measured.reflectanceFront + measured.reflectanceBack);
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
          const MeasuredPoint & measured, const ReflectanceChoice choice, const Tolerances & tol)
        {
            const auto reflectance{chosenReflectance(measured, choice)};
            const auto opaque{std::abs(measured.transmittance) < tol.limitOfDetection};
            if(opaque && std::abs(measured.reflectanceFront) < tol.limitOfDetection
               && std::abs(measured.reflectanceBack) < tol.limitOfDetection)
            {
                return {.value = {}, .warnings = {Warning::NoData}};
            }
            if(opaque)
            {
                return {.value = {.surfaceReflectance = reflectance, .internalTransmittance = 0.0},
                        .warnings = {Warning::OpaqueLayer}};
            }
            const auto surface{interfaceReflectance(measured.transmittance, reflectance)};
            const auto [tau, warnings]{physical(
              internalTransmittance(measured.transmittance, reflectance, surface), {}, tol)};
            return {.value = {.surfaceReflectance = surface, .internalTransmittance = tau},
                    .warnings = warnings};
        }

        ///////////////////////////////////////////////////////////////////////////
        /// Coating on a substrate
        ///////////////////////////////////////////////////////////////////////////

        //! Coating behind the substrate. reflectanceFront of the result is the coating reflectance
        //! seen from the substrate, reflectanceBack is the one seen from the air.
        [[nodiscard]] InterfacePoint coatingBehindSubstrate(const MeasuredPoint & coated,
                                                            const MonolithicPoint & substrate)
        {
            return asInterface(withoutFrontPart(
              coated, bareSurface(substrate.surfaceReflectance), substrate.internalTransmittance));
        }

        //! Deconstruction of the coating for product that has the coating on the back side.
        [[nodiscard]] PointResult<InterfacePoint> deconstructCoatingAt(
          const MeasuredPoint & coated, const MonolithicPoint & substrate, const Tolerances & tol)
        {
            if(noData(substrate, tol))
            {
                return {.value = {}, .warnings = {Warning::NoData}};
            }
            if(std::abs(substrate.internalTransmittance) < tol.limitOfDetection)
            {
                // Nothing is known about the substrate side. Coating is assumed to be opaque.
                return {.value = {.transmittance = 0.0,
                                  .reflectanceFront = 1.0,
                                  .reflectanceBack = coated.reflectanceBack},
                        .warnings = {Warning::OpaqueSubstrate}};
            }
            const auto insensitive{substrate.internalTransmittance < tol.sensitivityThreshold};
            return physical(coatingBehindSubstrate(coated, substrate),
                            appended({}, insensitive, Warning::InsensitiveCoatingSubstrateSide),
                            tol);
        }

        [[nodiscard]] PointResult<InterfacePoint>
          flippedResult(const PointResult<InterfacePoint> & result)
        {
            return {.value = {.transmittance = result.value.transmittance,
                              .reflectanceFront = result.value.reflectanceBack,
                              .reflectanceBack = result.value.reflectanceFront},
                    .warnings = result.warnings};
        }

        ///////////////////////////////////////////////////////////////////////////
        /// Interlayer
        ///////////////////////////////////////////////////////////////////////////

        //! Front pane of a laminate as a part: bare surface, glass and an interface to the
        //! interlayer that does not reflect.
        [[nodiscard]] MeasuredPoint paneInFrontOfInterlayer(const MonolithicPoint & pane)
        {
            const auto tau{pane.internalTransmittance};
            return {.transmittance = (1 - pane.surfaceReflectance) * tau,
                    .reflectanceFront = pane.surfaceReflectance,
                    .reflectanceBack = pane.surfaceReflectance * tau * tau};
        }

        //! Interlayer is the bulk layer between the two panes.
        [[nodiscard]] double solveInterlayer(const double transmittance,
                                             const MonolithicPoint & front,
                                             const MonolithicPoint & back)
        {
            return bulkBetween(transmittance,
                               paneInFrontOfInterlayer(front),
                               flippedPoint(paneInFrontOfInterlayer(back)));
        }

        [[nodiscard]] PointResult<double> deconstructInterlayerAt(const MeasuredPoint & laminate,
                                                                  const MonolithicPoint & front,
                                                                  const MonolithicPoint & back,
                                                                  const Tolerances & tol)
        {
            if(noData(front, tol) || noData(back, tol))
            {
                return {.value = 0.0, .warnings = {Warning::NoData}};
            }
            const auto panes{front.internalTransmittance * back.internalTransmittance};
            if(std::abs(laminate.transmittance) < tol.limitOfDetection
               || panes < Helper::SmallNumber)
            {
                return {.value = 0.0, .warnings = {Warning::InsensitiveInterlayer}};
            }
            const auto insensitive{std::sqrt(panes) < tol.interlayerSensitivityThreshold};
            return physical(solveInterlayer(laminate.transmittance, front, back),
                            appended({}, insensitive, Warning::InsensitiveInterlayer),
                            tol);
        }

        ///////////////////////////////////////////////////////////////////////////
        /// Coating inside of a laminate
        ///////////////////////////////////////////////////////////////////////////

        //! Coating between the front pane and the interlayer. The front part is the bare surface
        //! of the front pane. The back part is the bare surface of the back pane. The interlayer
        //! and the back pane are one bulk layer (pathTransmittance), because the interface
        //! between them does not reflect.
        [[nodiscard]] InterfacePoint coatingBehindFrontPane(const MeasuredPoint & laminate,
                                                            const MonolithicPoint & front,
                                                            const double backSurface,
                                                            const double pathTransmittance)
        {
            const auto behindFrontPane{withoutFrontPart(
              laminate, bareSurface(front.surfaceReflectance), front.internalTransmittance)};
            return asInterface(
              withoutBackPart(behindFrontPane, bareSurface(backSurface), pathTransmittance));
        }

        //! Values that cannot be determined from the measurement are set to zero.
        [[nodiscard]] InterfacePoint withoutInsensitive(const InterfacePoint & coating,
                                                        const bool interlayerSide,
                                                        const bool substrateSide)
        {
            return {.transmittance = interlayerSide ? 0.0 : coating.transmittance,
                    .reflectanceFront = substrateSide ? 0.0 : coating.reflectanceFront,
                    .reflectanceBack = interlayerSide ? 0.0 : coating.reflectanceBack};
        }

        [[nodiscard]] PointResult<InterfacePoint> deconstructEmbeddedCoatingAt(
          const MeasuredPoint & laminate,
          const MonolithicPoint & front,
          const MonolithicPoint & back,
          const double interlayer,
          const Tolerances & tol)
        {
            if(noData(front, tol) || noData(back, tol))
            {
                return {.value = {}, .warnings = {Warning::NoData}};
            }
            const auto path{Helper::nonZero(interlayer * back.internalTransmittance)};
            const auto coating{
              coatingBehindFrontPane(laminate, front, back.surfaceReflectance, path)};
            const auto interlayerSide{path < tol.sensitivityThreshold};
            const auto substrateSide{front.internalTransmittance < tol.sensitivityThreshold};
            const auto warnings{
              appended(appended({}, interlayerSide, Warning::InsensitiveCoatingInterlayerSide),
                       substrateSide,
                       Warning::InsensitiveCoatingSubstrateSide)};
            return physical(
              withoutInsensitive(coating, interlayerSide, substrateSide), warnings, tol);
        }

        ///////////////////////////////////////////////////////////////////////////
        /// Two monolithic layers in contact (ISO 9050, B.5.2)
        ///////////////////////////////////////////////////////////////////////////

        //! Outer interface of a thick layer together with internal transmittance of the layer.
        struct ThickLayerPoint
        {
            InterfacePoint surface;
            double internalTransmittance{0};
        };

        //! Interface between the two layers. The front part is the outer surface of the front
        //! layer, the back part is the outer surface of the back layer. The closed expressions of
        //! ISO 9050 B.24 to B.26 are these two removals written as one.
        [[nodiscard]] InterfacePoint contactInterface(const MeasuredPoint & measured,
                                                      const ThickLayerPoint & front,
                                                      const ThickLayerPoint & back)
        {
            const auto behindFrontLayer{
              withoutFrontPart(measured, asPart(front.surface), front.internalTransmittance)};
            return asInterface(withoutBackPart(
              behindFrontLayer, asPart(back.surface), back.internalTransmittance));
        }

        [[nodiscard]] bool noData(const ThickLayerPoint & layer, const Tolerances & tolerances)
        {
            return std::abs(layer.surface.transmittance) < tolerances.limitOfDetection
                   && std::abs(layer.internalTransmittance) < tolerances.limitOfDetection;
        }

        [[nodiscard]] PointResult<InterfacePoint> deconstructDoubleMonolithicAt(
          const MeasuredPoint & measured,
          const ThickLayerPoint & front,
          const ThickLayerPoint & back,
          const Tolerances & tol)
        {
            if(noData(front, tol) || noData(back, tol))
            {
                return {.value = {}, .warnings = {Warning::NoData}};
            }
            if(std::abs(front.internalTransmittance) < tol.limitOfDetection
               || std::abs(back.internalTransmittance) < tol.limitOfDetection)
            {
                return {.value = {}, .warnings = {Warning::OpaqueLayer}};
            }
            return physical(contactInterface(measured, front, back), {}, tol);
        }

        [[nodiscard]] ThickLayerPoint thickLayerAt(const Interface & surface,
                                                   const Bulk & bulk,
                                                   const size_t index)
        {
            return {.surface = {.transmittance = surface.transmittance[index].value(),
                                .reflectanceFront = surface.reflectanceFront[index].value(),
                                .reflectanceBack = surface.reflectanceBack[index].value()},
                    .internalTransmittance = bulk.internalTransmittance[index].value()};
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

        [[nodiscard]] WithDiagnostics<Interface>
          toInterface(const std::vector<double> & wavelengths,
                      const std::vector<PointResult<InterfacePoint>> & points)
        {
            const auto series{[&](double InterfacePoint::* member) {
                return Helper::makeSeries(
                  wavelengths, [&](const size_t index) { return points[index].value.*member; });
            }};
            return {.value = {.transmittance = series(&InterfacePoint::transmittance),
                              .reflectanceFront = series(&InterfacePoint::reflectanceFront),
                              .reflectanceBack = series(&InterfacePoint::reflectanceBack)},
                    .diagnostics = diagnosticsOf(wavelengths, points)};
        }

        [[nodiscard]] std::vector<const CSeries *> seriesOf(const ExternalProperties & measured)
        {
            return {&measured.transmittance, &measured.reflectanceFront, &measured.reflectanceBack};
        }

        [[nodiscard]] std::vector<const CSeries *>
          seriesOf(const ExternalProperties & measured,
                   const std::vector<const Monolithic *> & monolithics)
        {
            auto result{seriesOf(measured)};
            for(const auto * monolithic : monolithics)
            {
                result.push_back(&monolithic->surfaceReflectance);
                result.push_back(&monolithic->internalTransmittance);
            }
            return result;
        }

        [[nodiscard]] std::vector<const CSeries *> seriesOf(const KnownPart & known)
        {
            auto result{seriesOf(known.externalProperties)};
            result.push_back(&known.bulkToUnknownInterface.internalTransmittance);
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
              .bulk = known->bulkToUnknownInterface.internalTransmittance[index].value()};
        }

        //! Interface with a known part on one side or on both sides. A side without a known
        //! part is a null pointer.
        [[nodiscard]] WithDiagnostics<Interface> interfaceNextTo(const ExternalProperties & measured,
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
            const auto wavelengths{measured.transmittance.getXArray()};
            const auto points{
              evaluate<InterfacePoint>(wavelengths.size(), [&](const size_t index) {
                  return deconstructInterfaceAt(measuredAt(measured, index),
                                                knownPartAt(frontPart, index),
                                                knownPartAt(backPart, index),
                                                tolerances);
              })};
            return toInterface(wavelengths, points);
        }
    }   // namespace

    Interface uncoatedInterface(const Monolithic & substrate)
    {
        return {.transmittance = 1.0 - substrate.surfaceReflectance,
                .reflectanceFront = substrate.surfaceReflectance,
                .reflectanceBack = substrate.surfaceReflectance};
    }

    WithDiagnostics<Interface> deconstructInterface(const ExternalProperties & measured,
                                                  const KnownPart & frontPart,
                                                  const KnownPart & backPart,
                                                  const Tolerances & tolerances)
    {
        return interfaceNextTo(measured, &frontPart, &backPart, tolerances);
    }

    WithDiagnostics<Interface> deconstructLastInterface(const ExternalProperties & measured,
                                                      const KnownPart & frontPart,
                                                      const Tolerances & tolerances)
    {
        return interfaceNextTo(measured, &frontPart, nullptr, tolerances);
    }

    WithDiagnostics<Interface> deconstructFirstInterface(const ExternalProperties & measured,
                                                       const KnownPart & backPart,
                                                       const Tolerances & tolerances)
    {
        return interfaceNextTo(measured, nullptr, &backPart, tolerances);
    }

    WithDiagnostics<Bulk> deconstructBulk(const ExternalProperties & measured,
                                        const ExternalProperties & frontPart,
                                        const ExternalProperties & backPart,
                                        const Tolerances & tolerances)
    {
        auto allSeries{seriesOf(measured)};
        std::ranges::copy(seriesOf(frontPart), std::back_inserter(allSeries));
        std::ranges::copy(seriesOf(backPart), std::back_inserter(allSeries));
        Helper::requireSameWavelengths(allSeries);
        const auto wavelengths{measured.transmittance.getXArray()};
        const auto points{evaluate<double>(wavelengths.size(), [&](const size_t index) {
            return deconstructBulkAt(measuredAt(measured, index),
                                     measuredAt(frontPart, index),
                                     measuredAt(backPart, index),
                                     tolerances);
        })};
        return {.value = {.internalTransmittance = Helper::makeSeries(
                            wavelengths, [&](const size_t index) { return points[index].value; })},
                .diagnostics = diagnosticsOf(wavelengths, points)};
    }

    WithDiagnostics<Monolithic> deconstructMonolithic(const ExternalProperties & measured,
                                                    const ReflectanceChoice choice,
                                                    const Tolerances & tolerances)
    {
        Helper::requireSameWavelengths(seriesOf(measured));
        const auto wavelengths{measured.transmittance.getXArray()};
        const auto points{
          evaluate<MonolithicPoint>(wavelengths.size(), [&](const size_t index) {
              return deconstructMonolithicAt(measuredAt(measured, index), choice, tolerances);
          })};
        const auto series{[&](double MonolithicPoint::* member) {
            return Helper::makeSeries(
              wavelengths, [&](const size_t index) { return points[index].value.*member; });
        }};
        return {.value = {.surfaceReflectance = series(&MonolithicPoint::surfaceReflectance),
                          .internalTransmittance =
                            series(&MonolithicPoint::internalTransmittance)},
                .diagnostics = diagnosticsOf(wavelengths, points)};
    }

    WithDiagnostics<Interface> deconstructCoating(const ExternalProperties & coated,
                                                const Monolithic & substrate,
                                                const Side coatedSide,
                                                const Tolerances & tolerances)
    {
        Helper::requireSameWavelengths(seriesOf(coated, {&substrate}));
        const auto wavelengths{coated.transmittance.getXArray()};
        const auto onFront{coatedSide == Side::Front};
        const auto points{
          evaluate<InterfacePoint>(wavelengths.size(), [&](const size_t index) {
              const auto measured{measuredAt(coated, index)};
              const auto result{
                deconstructCoatingAt(onFront ? flippedPoint(measured) : measured,
                                     monolithicAt(substrate, index),
                                     tolerances)};
              return onFront ? flippedResult(result) : result;
          })};
        return toInterface(wavelengths, points);
    }

    WithDiagnostics<Bulk> deconstructInterlayer(const ExternalProperties & laminate,
                                              const Monolithic & front,
                                              const Monolithic & back,
                                              const Tolerances & tolerances)
    {
        Helper::requireSameWavelengths(seriesOf(laminate, {&front, &back}));
        const auto wavelengths{laminate.transmittance.getXArray()};
        const auto points{evaluate<double>(wavelengths.size(), [&](const size_t index) {
            return deconstructInterlayerAt(measuredAt(laminate, index),
                                           monolithicAt(front, index),
                                           monolithicAt(back, index),
                                           tolerances);
        })};
        return {.value = {.internalTransmittance = Helper::makeSeries(
                            wavelengths, [&](const size_t index) { return points[index].value; })},
                .diagnostics = diagnosticsOf(wavelengths, points)};
    }

    WithDiagnostics<Interface> deconstructEmbeddedCoating(const ExternalProperties & laminate,
                                                        const Monolithic & front,
                                                        const Monolithic & back,
                                                        const Bulk & interlayer,
                                                        const Tolerances & tolerances)
    {
        auto allSeries{seriesOf(laminate, {&front, &back})};
        allSeries.push_back(&interlayer.internalTransmittance);
        Helper::requireSameWavelengths(allSeries);
        const auto wavelengths{laminate.transmittance.getXArray()};
        const auto points{
          evaluate<InterfacePoint>(wavelengths.size(), [&](const size_t index) {
              return deconstructEmbeddedCoatingAt(measuredAt(laminate, index),
                                                  monolithicAt(front, index),
                                                  monolithicAt(back, index),
                                                  interlayer.internalTransmittance[index].value(),
                                                  tolerances);
          })};
        return toInterface(wavelengths, points);
    }

    WithDiagnostics<Interface> deconstructDoubleMonolithic(const ExternalProperties & measured,
                                                         const Interface & frontSurface,
                                                         const Bulk & frontBulk,
                                                         const Interface & backSurface,
                                                         const Bulk & backBulk,
                                                         const Tolerances & tolerances)
    {
        auto allSeries{seriesOf(measured)};
        for(const auto * surface : {&frontSurface, &backSurface})
        {
            allSeries.push_back(&surface->transmittance);
            allSeries.push_back(&surface->reflectanceFront);
            allSeries.push_back(&surface->reflectanceBack);
        }
        allSeries.push_back(&frontBulk.internalTransmittance);
        allSeries.push_back(&backBulk.internalTransmittance);
        Helper::requireSameWavelengths(allSeries);
        const auto wavelengths{measured.transmittance.getXArray()};
        const auto points{
          evaluate<InterfacePoint>(wavelengths.size(), [&](const size_t index) {
              return deconstructDoubleMonolithicAt(measuredAt(measured, index),
                                                   thickLayerAt(frontSurface, frontBulk, index),
                                                   thickLayerAt(backSurface, backBulk, index),
                                                   tolerances);
          })};
        return toInterface(wavelengths, points);
    }

    WithDiagnostics<Interface> deconstructDoubleMonolithic(const ExternalProperties & measured,
                                                         const Monolithic & front,
                                                         const Monolithic & back,
                                                         const Tolerances & tolerances)
    {
        return deconstructDoubleMonolithic(
          measured,
          uncoatedInterface(front),
          Bulk{.internalTransmittance = front.internalTransmittance},
          uncoatedInterface(back),
          Bulk{.internalTransmittance = back.internalTransmittance},
          tolerances);
    }
}   // namespace CompoundProduct
