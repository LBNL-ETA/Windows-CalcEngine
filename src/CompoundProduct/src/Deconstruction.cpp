#include <algorithm>
#include <cmath>
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
            const auto surface{substrate.surfaceReflectance};
            const auto tauSquared{substrate.internalTransmittance
                                  * substrate.internalTransmittance};
            const auto substrateSide{
              (coated.reflectanceFront - surface)
              / Helper::nonZero(tauSquared * (1 + surface * (coated.reflectanceFront - 2)))};
            const auto interReflection{Helper::nonZero(1 - surface * substrateSide * tauSquared)};
            const auto transmittance{
              coated.transmittance * interReflection
              / Helper::nonZero((1 - surface) * substrate.internalTransmittance)};
            const auto airSide{coated.reflectanceBack
                               - transmittance * transmittance * surface * tauSquared
                                   / interReflection};
            return {.transmittance = transmittance,
                    .reflectanceFront = substrateSide,
                    .reflectanceBack = airSide};
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

        [[nodiscard]] MeasuredPoint flippedPoint(const MeasuredPoint & point)
        {
            return {.transmittance = point.transmittance,
                    .reflectanceFront = point.reflectanceBack,
                    .reflectanceBack = point.reflectanceFront};
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

        //! Solves the laminate transmittance equation for the interlayer internal transmittance.
        [[nodiscard]] double solveInterlayer(const double transmittance,
                                             const MonolithicPoint & front,
                                             const MonolithicPoint & back)
        {
            const auto panes{front.internalTransmittance * back.internalTransmittance};
            const auto quadratic{transmittance * front.surfaceReflectance * back.surfaceReflectance * panes
                                 * panes};
            const auto linear{(1 - front.surfaceReflectance) * (1 - back.surfaceReflectance) * panes};
            if(std::abs(quadratic) < Helper::SmallNumber)
            {
                return transmittance / Helper::nonZero(linear);
            }
            const auto discriminant{linear * linear + 4 * quadratic * transmittance};
            return (-linear + std::sqrt(discriminant)) / (2 * quadratic);
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

        //! Properties of everything that is behind the bulk of the front pane (coating, interlayer
        //! and back pane), obtained by removing the front pane from the measured laminate.
        [[nodiscard]] MeasuredPoint behindFrontPane(const MeasuredPoint & laminate,
                                                    const MonolithicPoint & front)
        {
            const auto surface{front.surfaceReflectance};
            const auto surfaceTransmittance{1 - surface};
            const auto tau{Helper::nonZero(front.internalTransmittance)};
            const auto reflectanceFront{
              (laminate.reflectanceFront - surface)
              / Helper::nonZero(tau * tau
                                * (surfaceTransmittance * surfaceTransmittance
                                   + surface * (laminate.reflectanceFront - surface)))};
            const auto interReflection{
              Helper::nonZero(1 - tau * tau * reflectanceFront * surface)};
            const auto transmittance{laminate.transmittance * interReflection
                                     / Helper::nonZero(surfaceTransmittance * tau)};
            const auto reflectanceBack{laminate.reflectanceBack
                                       - transmittance * transmittance * surface * tau * tau
                                           / interReflection};
            return {.transmittance = transmittance,
                    .reflectanceFront = reflectanceFront,
                    .reflectanceBack = reflectanceBack};
        }

        //! Removes interlayer and back pane from the part that is behind the front pane.
        //! pathTransmittance is internal transmittance of the interlayer and the back pane.
        [[nodiscard]] InterfacePoint coatingInFrontOf(const MeasuredPoint & remaining,
                                                      const double backSurface,
                                                      const double pathTransmittance)
        {
            const auto pathSquared{pathTransmittance * pathTransmittance};
            const auto backTransmittance{1 - backSurface};
            const auto interlayerSide{
              (remaining.reflectanceBack - backSurface)
              / Helper::nonZero(pathSquared
                                * ((remaining.reflectanceBack - backSurface) * backSurface
                                   + backTransmittance * backTransmittance))};
            const auto interReflection{
              Helper::nonZero(1 - interlayerSide * backSurface * pathSquared)};
            const auto transmittance{remaining.transmittance * interReflection
                                     / Helper::nonZero(backTransmittance * pathTransmittance)};
            const auto substrateSide{remaining.reflectanceFront
                                     - transmittance * transmittance * backSurface * pathSquared
                                         / interReflection};
            return {.transmittance = transmittance,
                    .reflectanceFront = substrateSide,
                    .reflectanceBack = interlayerSide};
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
              coatingInFrontOf(behindFrontPane(laminate, front), back.surfaceReflectance, path)};
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

        //! Square of interface transmittance minus product of interface reflectances.
        [[nodiscard]] double surfaceDifference(const InterfacePoint & surface)
        {
            return surface.transmittance * surface.transmittance
                   - surface.reflectanceFront * surface.reflectanceBack;
        }

        //! Square of measured transmittance minus product of measured reflectances.
        [[nodiscard]] double measuredDifference(const MeasuredPoint & measured)
        {
            return measured.transmittance * measured.transmittance
                   - measured.reflectanceFront * measured.reflectanceBack;
        }

        [[nodiscard]] double contactDenominator(const MeasuredPoint & measured,
                                                const InterfacePoint & front,
                                                const InterfacePoint & back)
        {
            return Helper::nonZero(
              surfaceDifference(front) * surfaceDifference(back)
              + measured.reflectanceBack * back.reflectanceFront * surfaceDifference(front)
              + measured.reflectanceFront * front.reflectanceBack * surfaceDifference(back)
              - front.reflectanceBack * back.reflectanceFront * measuredDifference(measured));
        }

        //! Inputs for reflectance of the contact interface seen from one of the two layers
        //! ("near" layer). "Outer" reflectance is seen from the air, "inner" from the layer.
        struct ContactSide
        {
            double measuredNear{0};
            double measuredFar{0};
            double nearOuterReflectance{0};
            double farInnerReflectance{0};
            double farDifference{0};
        };

        [[nodiscard]] double contactNumerator(const ContactSide & side, const double mixed)
        {
            return (side.measuredNear - side.nearOuterReflectance) * side.farDifference
                   - side.farInnerReflectance * mixed
                   - side.farInnerReflectance * side.nearOuterReflectance * side.measuredFar;
        }

        [[nodiscard]] ContactSide frontSide(const MeasuredPoint & measured,
                                            const InterfacePoint & front,
                                            const InterfacePoint & back)
        {
            return {.measuredNear = measured.reflectanceFront,
                    .measuredFar = measured.reflectanceBack,
                    .nearOuterReflectance = front.reflectanceFront,
                    .farInnerReflectance = back.reflectanceFront,
                    .farDifference = surfaceDifference(back)};
        }

        [[nodiscard]] ContactSide backSide(const MeasuredPoint & measured,
                                           const InterfacePoint & front,
                                           const InterfacePoint & back)
        {
            return {.measuredNear = measured.reflectanceBack,
                    .measuredFar = measured.reflectanceFront,
                    .nearOuterReflectance = back.reflectanceBack,
                    .farInnerReflectance = front.reflectanceBack,
                    .farDifference = surfaceDifference(front)};
        }

        [[nodiscard]] InterfacePoint contactInterface(const MeasuredPoint & measured,
                                                      const ThickLayerPoint & front,
                                                      const ThickLayerPoint & back)
        {
            const auto denominator{contactDenominator(measured, front.surface, back.surface)};
            const auto mixed{measuredDifference(measured)};
            const auto frontTau{Helper::nonZero(front.internalTransmittance)};
            const auto backTau{Helper::nonZero(back.internalTransmittance)};
            return {.transmittance = front.surface.transmittance * measured.transmittance
                                     * back.surface.transmittance
                                     / (frontTau * backTau * denominator),
                    .reflectanceFront =
                      contactNumerator(frontSide(measured, front.surface, back.surface), mixed)
                      / (frontTau * frontTau * denominator),
                    .reflectanceBack =
                      contactNumerator(backSide(measured, front.surface, back.surface), mixed)
                      / (backTau * backTau * denominator)};
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
    }   // namespace

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
