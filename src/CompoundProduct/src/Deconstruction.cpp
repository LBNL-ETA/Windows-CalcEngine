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
            double transmittanceFront{0};
            double transmittanceBack{0};
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
            double transmittanceFront{0};
            double transmittanceBack{0};
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
            return {.transmittanceFront = measured.transmittanceFront[index].value(),
                    .transmittanceBack = measured.transmittanceBack[index].value(),
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
            const auto allPhysical{Helper::isPhysical(point.transmittanceFront)
                                   && Helper::isPhysical(point.transmittanceBack)
                                   && Helper::isPhysical(point.reflectanceFront)
                                   && Helper::isPhysical(point.reflectanceBack)};
            if(!tolerances.clampToPhysical || allPhysical)
            {
                return {.value = point, .warnings = warnings};
            }
            return {.value = {.transmittanceFront = std::clamp(point.transmittanceFront, 0.0, 1.0),
                              .transmittanceBack = std::clamp(point.transmittanceBack, 0.0, 1.0),
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
        /// same four external properties as a measured product, so it is a MeasuredPoint.
        ///////////////////////////////////////////////////////////////////////////

        [[nodiscard]] MeasuredPoint flippedPoint(const MeasuredPoint & point)
        {
            return {.transmittanceFront = point.transmittanceBack,
                    .transmittanceBack = point.transmittanceFront,
                    .reflectanceFront = point.reflectanceBack,
                    .reflectanceBack = point.reflectanceFront};
        }

        //! Transmittance of a product that has one transmittance by nature (an uncoated pane, or
        //! a product whose unknown is a bulk layer): the mean of the two measured ones.
        [[nodiscard]] double meanTransmittance(const MeasuredPoint & point)
        {
            return 0.5 * (point.transmittanceFront + point.transmittanceBack);
        }

        //! Surface of uncoated glass towards the air, as a part.
        [[nodiscard]] MeasuredPoint bareSurface(const double reflectance)
        {
            return {.transmittanceFront = 1 - reflectance,
                    .transmittanceBack = 1 - reflectance,
                    .reflectanceFront = reflectance,
                    .reflectanceBack = reflectance};
        }

        //! A part that consists of one interface.
        [[nodiscard]] InterfacePoint asInterface(const MeasuredPoint & part)
        {
            return {.transmittanceFront = part.transmittanceFront,
                    .transmittanceBack = part.transmittanceBack,
                    .reflectanceFront = part.reflectanceFront,
                    .reflectanceBack = part.reflectanceBack};
        }

        [[nodiscard]] MeasuredPoint asPart(const InterfacePoint & layerInterface)
        {
            return {.transmittanceFront = layerInterface.transmittanceFront,
                    .transmittanceBack = layerInterface.transmittanceBack,
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
                                * (front.transmittanceFront * front.transmittanceBack
                                   + front.reflectanceBack * added))};
            const auto interReflection{
              Helper::nonZero(1 - front.reflectanceBack * reflectanceFront * bulkSquared)};
            const auto transmittanceFront{whole.transmittanceFront * interReflection
                                          / Helper::nonZero(front.transmittanceFront * bulk)};
            const auto transmittanceBack{whole.transmittanceBack * interReflection
                                         / Helper::nonZero(front.transmittanceBack * bulk)};
            return {.transmittanceFront = transmittanceFront,
                    .transmittanceBack = transmittanceBack,
                    .reflectanceFront = reflectanceFront,
                    .reflectanceBack = whole.reflectanceBack
                                       - transmittanceFront * transmittanceBack
                                           * front.reflectanceBack * bulkSquared
                                           / interReflection};
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
        //! transmittance of the whole product. Each direction of the light gives one equation
        //! for the same unknown. The mean of the two equations is solved.
        [[nodiscard]] double bulkBetween(const MeasuredPoint & whole,
                                         const MeasuredPoint & front,
                                         const MeasuredPoint & back)
        {
            const auto transmittance{meanTransmittance(whole)};
            const auto linear{0.5
                              * (front.transmittanceFront * back.transmittanceFront
                                 + front.transmittanceBack * back.transmittanceBack)};
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
            if(std::abs(meanTransmittance(measured)) < tol.limitOfDetection)
            {
                return {.value = 0.0, .warnings = {Warning::OpaqueLayer}};
            }
            return physical(bulkBetween(measured, front, back), {}, tol);
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
            const auto transmittance{meanTransmittance(measured)};
            const auto opaque{std::abs(transmittance) < tol.limitOfDetection};
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
            const auto surface{interfaceReflectance(transmittance, reflectance)};
            const auto [tau, warnings]{
              physical(internalTransmittance(transmittance, reflectance, surface), {}, tol)};
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
                return {.value = {.transmittanceFront = 0.0,
                                  .transmittanceBack = 0.0,
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
            return {.value = asInterface(flippedPoint(asPart(result.value))),
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
            return {.transmittanceFront = (1 - pane.surfaceReflectance) * tau,
                    .transmittanceBack = (1 - pane.surfaceReflectance) * tau,
                    .reflectanceFront = pane.surfaceReflectance,
                    .reflectanceBack = pane.surfaceReflectance * tau * tau};
        }

        //! Interlayer is the bulk layer between the two panes.
        [[nodiscard]] double solveInterlayer(const MeasuredPoint & laminate,
                                             const MonolithicPoint & front,
                                             const MonolithicPoint & back)
        {
            return bulkBetween(laminate,
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
            if(std::abs(meanTransmittance(laminate)) < tol.limitOfDetection
               || panes < Helper::SmallNumber)
            {
                return {.value = 0.0, .warnings = {Warning::InsensitiveInterlayer}};
            }
            const auto insensitive{std::sqrt(panes) < tol.interlayerSensitivityThreshold};
            return physical(solveInterlayer(laminate, front, back),
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
            return {.transmittanceFront = interlayerSide ? 0.0 : coating.transmittanceFront,
                    .transmittanceBack = interlayerSide ? 0.0 : coating.transmittanceBack,
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
            return std::abs(layer.surface.transmittanceFront) < tolerances.limitOfDetection
                   && std::abs(layer.surface.transmittanceBack) < tolerances.limitOfDetection
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
            return {.surface = {.transmittanceFront = surface.transmittanceFront[index].value(),
                                .transmittanceBack = surface.transmittanceBack[index].value(),
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
            return {.value = {.transmittanceFront = series(&InterfacePoint::transmittanceFront),
                              .transmittanceBack = series(&InterfacePoint::transmittanceBack),
                              .reflectanceFront = series(&InterfacePoint::reflectanceFront),
                              .reflectanceBack = series(&InterfacePoint::reflectanceBack)},
                    .diagnostics = diagnosticsOf(wavelengths, points)};
        }

        [[nodiscard]] std::vector<const CSeries *> seriesOf(const ExternalProperties & measured)
        {
            return {&measured.transmittanceFront,
                    &measured.transmittanceBack,
                    &measured.reflectanceFront,
                    &measured.reflectanceBack};
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
            result.push_back(&known.bulkToRemainingPart.internalTransmittance);
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
              .bulk = known->bulkToRemainingPart.internalTransmittance[index].value()};
        }

        //! Interface with a known part on one side or on both sides. A side without a known
        //! part is a null pointer.
        [[nodiscard]] WithDiagnostics<Interface>
          interfaceNextTo(const ExternalProperties & measured,
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
            const auto wavelengths{measured.transmittanceFront.getXArray()};
            const auto points{
              evaluate<InterfacePoint>(wavelengths.size(), [&](const size_t index) {
                  return deconstructInterfaceAt(measuredAt(measured, index),
                                                knownPartAt(frontPart, index),
                                                knownPartAt(backPart, index),
                                                tolerances);
              })};
            return toInterface(wavelengths, points);
        }

        //! The result of a removal is a part. A part and an interface hold the same four
        //! properties.
        [[nodiscard]] WithDiagnostics<ExternalProperties>
          asPart(const WithDiagnostics<Interface> & remaining)
        {
            return {.value = {.transmittanceFront = remaining.value.transmittanceFront,
                              .transmittanceBack = remaining.value.transmittanceBack,
                              .reflectanceFront = remaining.value.reflectanceFront,
                              .reflectanceBack = remaining.value.reflectanceBack},
                    .diagnostics = remaining.diagnostics};
        }
    }   // namespace

    Interface uncoatedInterface(const Monolithic & substrate)
    {
        return {.transmittanceFront = 1.0 - substrate.surfaceReflectance,
                .transmittanceBack = 1.0 - substrate.surfaceReflectance,
                .reflectanceFront = substrate.surfaceReflectance,
                .reflectanceBack = substrate.surfaceReflectance};
    }

    WithDiagnostics<Interface>
      ExternalProperties::deconstructInterface(const Assembly & frontPart,
                                               const Bulk & frontBulk,
                                               const Bulk & backBulk,
                                               const Assembly & backPart,
                                               const Tolerances & tolerances) const
    {
        const KnownPart front{.externalProperties = frontPart.externalProperties(),
                              .bulkToRemainingPart = frontBulk};
        const KnownPart back{.externalProperties = backPart.externalProperties(),
                             .bulkToRemainingPart = backBulk};
        return interfaceNextTo(*this, &front, &back, tolerances);
    }

    WithDiagnostics<ExternalProperties>
      ExternalProperties::deconstructPartBehind(const Assembly & frontPart,
                                                const Bulk & bulk,
                                                const Tolerances & tolerances) const
    {
        const KnownPart front{.externalProperties = frontPart.externalProperties(),
                              .bulkToRemainingPart = bulk};
        return asPart(interfaceNextTo(*this, &front, nullptr, tolerances));
    }

    WithDiagnostics<ExternalProperties>
      ExternalProperties::deconstructPartInFrontOf(const Bulk & bulk,
                                                   const Assembly & backPart,
                                                   const Tolerances & tolerances) const
    {
        const KnownPart back{.externalProperties = backPart.externalProperties(),
                             .bulkToRemainingPart = bulk};
        return asPart(interfaceNextTo(*this, nullptr, &back, tolerances));
    }

    WithDiagnostics<Bulk> ExternalProperties::deconstructBulk(const Assembly & frontPart,
                                                              const Assembly & backPart,
                                                              const Tolerances & tolerances) const
    {
        const auto front{frontPart.externalProperties()};
        const auto back{backPart.externalProperties()};
        auto allSeries{seriesOf(*this)};
        std::ranges::copy(seriesOf(front), std::back_inserter(allSeries));
        std::ranges::copy(seriesOf(back), std::back_inserter(allSeries));
        Helper::requireSameWavelengths(allSeries);
        const auto wavelengths{transmittanceFront.getXArray()};
        const auto points{evaluate<double>(wavelengths.size(), [&](const size_t index) {
            return deconstructBulkAt(measuredAt(*this, index),
                                     measuredAt(front, index),
                                     measuredAt(back, index),
                                     tolerances);
        })};
        return {.value = {.internalTransmittance = Helper::makeSeries(
                            wavelengths, [&](const size_t index) { return points[index].value; })},
                .diagnostics = diagnosticsOf(wavelengths, points)};
    }

    WithDiagnostics<Monolithic>
      ExternalProperties::deconstructMonolithic(const ReflectanceChoice choice,
                                                const Tolerances & tolerances) const
    {
        Helper::requireSameWavelengths(seriesOf(*this));
        const auto wavelengths{transmittanceFront.getXArray()};
        const auto points{
          evaluate<MonolithicPoint>(wavelengths.size(), [&](const size_t index) {
              return deconstructMonolithicAt(measuredAt(*this, index), choice, tolerances);
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

    WithDiagnostics<Interface>
      ExternalProperties::deconstructCoating(const Monolithic & substrate,
                                             const Side coatedSide,
                                             const Tolerances & tolerances) const
    {
        Helper::requireSameWavelengths(seriesOf(*this, {&substrate}));
        const auto wavelengths{transmittanceFront.getXArray()};
        const auto onFront{coatedSide == Side::Front};
        const auto points{
          evaluate<InterfacePoint>(wavelengths.size(), [&](const size_t index) {
              const auto measured{measuredAt(*this, index)};
              const auto result{
                deconstructCoatingAt(onFront ? flippedPoint(measured) : measured,
                                     monolithicAt(substrate, index),
                                     tolerances)};
              return onFront ? flippedResult(result) : result;
          })};
        return toInterface(wavelengths, points);
    }

    WithDiagnostics<Bulk>
      ExternalProperties::deconstructInterlayer(const Monolithic & front,
                                                const Monolithic & back,
                                                const Tolerances & tolerances) const
    {
        Helper::requireSameWavelengths(seriesOf(*this, {&front, &back}));
        const auto wavelengths{transmittanceFront.getXArray()};
        const auto points{evaluate<double>(wavelengths.size(), [&](const size_t index) {
            return deconstructInterlayerAt(measuredAt(*this, index),
                                           monolithicAt(front, index),
                                           monolithicAt(back, index),
                                           tolerances);
        })};
        return {.value = {.internalTransmittance = Helper::makeSeries(
                            wavelengths, [&](const size_t index) { return points[index].value; })},
                .diagnostics = diagnosticsOf(wavelengths, points)};
    }

    WithDiagnostics<Interface>
      ExternalProperties::deconstructEmbeddedCoating(const Monolithic & front,
                                                     const Monolithic & back,
                                                     const Bulk & interlayer,
                                                     const Tolerances & tolerances) const
    {
        auto allSeries{seriesOf(*this, {&front, &back})};
        allSeries.push_back(&interlayer.internalTransmittance);
        Helper::requireSameWavelengths(allSeries);
        const auto wavelengths{transmittanceFront.getXArray()};
        const auto points{
          evaluate<InterfacePoint>(wavelengths.size(), [&](const size_t index) {
              return deconstructEmbeddedCoatingAt(measuredAt(*this, index),
                                                  monolithicAt(front, index),
                                                  monolithicAt(back, index),
                                                  interlayer.internalTransmittance[index].value(),
                                                  tolerances);
          })};
        return toInterface(wavelengths, points);
    }

    WithDiagnostics<Interface>
      ExternalProperties::deconstructDoubleMonolithic(const Interface & frontSurface,
                                                      const Bulk & frontBulk,
                                                      const Interface & backSurface,
                                                      const Bulk & backBulk,
                                                      const Tolerances & tolerances) const
    {
        auto allSeries{seriesOf(*this)};
        for(const auto * surface : {&frontSurface, &backSurface})
        {
            allSeries.push_back(&surface->transmittanceFront);
            allSeries.push_back(&surface->transmittanceBack);
            allSeries.push_back(&surface->reflectanceFront);
            allSeries.push_back(&surface->reflectanceBack);
        }
        allSeries.push_back(&frontBulk.internalTransmittance);
        allSeries.push_back(&backBulk.internalTransmittance);
        Helper::requireSameWavelengths(allSeries);
        const auto wavelengths{transmittanceFront.getXArray()};
        const auto points{
          evaluate<InterfacePoint>(wavelengths.size(), [&](const size_t index) {
              return deconstructDoubleMonolithicAt(measuredAt(*this, index),
                                                   thickLayerAt(frontSurface, frontBulk, index),
                                                   thickLayerAt(backSurface, backBulk, index),
                                                   tolerances);
          })};
        return toInterface(wavelengths, points);
    }

    WithDiagnostics<Interface>
      ExternalProperties::deconstructDoubleMonolithic(const Monolithic & front,
                                                      const Monolithic & back,
                                                      const Tolerances & tolerances) const
    {
        return deconstructDoubleMonolithic(
          uncoatedInterface(front),
          Bulk{.internalTransmittance = front.internalTransmittance},
          uncoatedInterface(back),
          Bulk{.internalTransmittance = back.internalTransmittance},
          tolerances);
    }
}   // namespace CompoundProduct
