#include <algorithm>
#include <cmath>
#include <iterator>
#include <numeric>
#include <ranges>
#include <stdexcept>

#include "Construction.hpp"
#include "SeriesUtilities.hpp"

using FenestrationCommon::CSeries;

namespace CompoundProduct
{
    namespace
    {
        //! External properties of the part of the assembly that has been processed so far.
        struct PointProperties
        {
            double Tf{0};
            double Tb{0};
            double Rf{0};
            double Rb{0};
        };

        [[nodiscard]] PointProperties interfaceAt(const Interface & layerInterface,
                                                  const size_t index)
        {
            return {.Tf = layerInterface.tf[index].value(),
                    .Tb = layerInterface.tb[index].value(),
                    .Rf = layerInterface.rf[index].value(),
                    .Rb = layerInterface.rb[index].value()};
        }

        //! Adds one bulk layer and the part "back" behind it to the part "front". The result is
        //! one part.
        [[nodiscard]] PointProperties add(const PointProperties & front,
                                          const double internalTransmittance,
                                          const PointProperties & back)
        {
            const auto tauSquared{internalTransmittance * internalTransmittance};
            const auto denominator{
              Helper::nonZero(1.0 - front.Rb * back.Rf * tauSquared)};
            const auto frontRoundTrip{front.Tf * front.Tb};
            const auto backRoundTrip{back.Tf * back.Tb};
            return {.Tf = front.Tf * back.Tf
                                          * internalTransmittance / denominator,
                    .Tb = front.Tb * back.Tb
                                         * internalTransmittance / denominator,
                    .Rf = front.Rf
                                        + frontRoundTrip * back.Rf * tauSquared
                                            / denominator,
                    .Rb = back.Rb
                                       + backRoundTrip * front.Rb * tauSquared
                                           / denominator};
        }

        using Segments = std::vector<Assembly::Segment>;

        [[nodiscard]] PointProperties
          constructAt(const Interface & front, const Segments & segments, const size_t index)
        {
            return std::accumulate(
              segments.begin(),
              segments.end(),
              interfaceAt(front, index),
              [&](const PointProperties & soFar, const Assembly::Segment & segment) {
                  return add(soFar,
                             segment.bulk.tau[index].value(),
                             interfaceAt(segment.backInterface, index));
              });
        }

        [[nodiscard]] std::vector<const CSeries *> seriesOf(const Interface & layerInterface)
        {
            return {
              &layerInterface.tf, &layerInterface.tb, &layerInterface.rf, &layerInterface.rb};
        }

        [[nodiscard]] std::vector<const CSeries *> allSeries(const Interface & front,
                                                             const Segments & segments)
        {
            auto result{seriesOf(front)};
            for(const auto & segment : segments)
            {
                result.push_back(&segment.bulk.tau);
                std::ranges::copy(seriesOf(segment.backInterface), std::back_inserter(result));
            }
            return result;
        }

        [[nodiscard]] ExternalProperties construct(const Interface & front,
                                                   const Segments & segments)
        {
            Helper::requireSameWavelengths(allSeries(front, segments));
            const auto wavelengths{front.tf.getXArray()};
            std::vector<PointProperties> points(wavelengths.size());
            std::ranges::transform(std::views::iota(size_t{0}, wavelengths.size()),
                                   points.begin(),
                                   [&](const size_t index) {
                                       return constructAt(front, segments, index);
                                   });
            const auto series{[&](double PointProperties::* member) {
                return Helper::makeSeries(
                  wavelengths, [&](const size_t index) { return points[index].*member; });
            }};
            return {.Tf = series(&PointProperties::Tf),
                    .Tb = series(&PointProperties::Tb),
                    .Rf = series(&PointProperties::Rf),
                    .Rb = series(&PointProperties::Rb)};
        }

        //! A part that is known only as a whole acts in an assembly as one interface with the
        //! external properties of the part.
        [[nodiscard]] Interface asInterface(const ExternalProperties & part)
        {
            return {.tf = part.Tf, .tb = part.Tb, .rf = part.Rf, .rb = part.Rb};
        }

        //! The interface in front of the bulk layer of the segment with the given index.
        [[nodiscard]] const Interface &
          interfaceInFrontOf(const Interface & front, const Segments & segments, const size_t index)
        {
            return index == 0 ? front : segments[index - 1].backInterface;
        }
    }   // namespace

    Assembly::Assembly(const Interface & frontPart) : m_Front{frontPart}
    {}

    Assembly::Assembly(const ExternalProperties & frontPart) : m_Front{asInterface(frontPart)}
    {}

    Assembly Assembly::add(const Bulk & bulk, const Interface & part) const
    {
        return add(bulk, Assembly{part});
    }

    Assembly Assembly::add(const Bulk & bulk, const ExternalProperties & part) const
    {
        return add(bulk, Assembly{part});
    }

    Assembly Assembly::add(const Bulk & bulk, const Assembly & part) const
    {
        auto result{*this};
        result.m_Segments.push_back({.bulk = bulk, .backInterface = part.m_Front});
        std::ranges::copy(part.m_Segments, std::back_inserter(result.m_Segments));
        return result;
    }

    //! The last interface becomes the first one. Every bulk layer is then followed by the
    //! interface that was in front of it.
    Assembly Assembly::flipped() const
    {
        Assembly result{m_Segments.empty() ? m_Front.flipped()
                                           : m_Segments.back().backInterface.flipped()};
        for(const auto index : std::views::iota(size_t{0}, m_Segments.size()) | std::views::reverse)
        {
            result.m_Segments.push_back(
              {.bulk = m_Segments[index].bulk,
               .backInterface = interfaceInFrontOf(m_Front, m_Segments, index).flipped()});
        }
        return result;
    }

    const Interface & Assembly::front() const
    {
        return m_Front;
    }

    const std::vector<Assembly::Segment> & Assembly::segments() const
    {
        return m_Segments;
    }

    ExternalProperties Assembly::externalProperties() const
    {
        return construct(m_Front, m_Segments);
    }

    Interface nonReflectingInterface(const std::vector<double> & wavelengths)
    {
        const auto constant{[&](const double value) {
            return Helper::makeSeries(wavelengths, [=](const size_t) { return value; });
        }};
        return {.tf = constant(1.0),
                .tb = constant(1.0),
                .rf = constant(0.0),
                .rb = constant(0.0)};
    }

    Interface Interface::flipped() const
    {
        return {.tf = tb, .tb = tf, .rf = rb, .rb = rf};
    }

    ExternalProperties ExternalProperties::flipped() const
    {
        return {.Tf = Tb, .Tb = Tf, .Rf = Rb, .Rb = Rf};
    }

    Bulk withThickness(const Bulk & bulk, const double oldThickness, const double newThickness)
    {
        if(oldThickness <= 0.0 || newThickness < 0.0)
        {
            throw std::runtime_error("Compound product: thickness must be positive.");
        }
        const auto exponent{newThickness / oldThickness};
        const auto & tau{bulk.tau};
        return {.tau =
                  Helper::makeSeries(tau.getXArray(), [&](const size_t index) {
                      return std::pow(std::max(tau[index].value(), 0.0), exponent);
                  })};
    }

    std::vector<double> commonWavelengths(const std::vector<ExternalProperties> & products)
    {
        std::vector<double> result;
        for(const auto & product : products)
        {
            std::ranges::copy(product.Tf.getXArray(),
                              std::back_inserter(result));
        }
        std::ranges::sort(result);
        const auto duplicates{
          std::ranges::unique(result, [](const double left, const double right) {
              return std::abs(left - right) < Helper::WavelengthTolerance;
          })};
        result.erase(duplicates.begin(), duplicates.end());
        return result;
    }
}   // namespace CompoundProduct
