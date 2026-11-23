#include <algorithm>
#include <cmath>
#include <iterator>
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
            double transmittanceFront{0};
            double transmittanceBack{0};
            double reflectanceFront{0};
            double reflectanceBack{0};
        };

        [[nodiscard]] PointProperties interfaceAt(const Interface & layerInterface,
                                                  const size_t index)
        {
            return {.transmittanceFront = layerInterface.transmittanceFront[index].value(),
                    .transmittanceBack = layerInterface.transmittanceBack[index].value(),
                    .reflectanceFront = layerInterface.reflectanceFront[index].value(),
                    .reflectanceBack = layerInterface.reflectanceBack[index].value()};
        }

        //! Adds one bulk layer and the part "back" behind it to the part "front". The result is
        //! one part.
        [[nodiscard]] PointProperties add(const PointProperties & front,
                                          const double internalTransmittance,
                                          const PointProperties & back)
        {
            const auto tauSquared{internalTransmittance * internalTransmittance};
            const auto denominator{
              Helper::nonZero(1.0 - front.reflectanceBack * back.reflectanceFront * tauSquared)};
            const auto frontRoundTrip{front.transmittanceFront * front.transmittanceBack};
            const auto backRoundTrip{back.transmittanceFront * back.transmittanceBack};
            return {.transmittanceFront = front.transmittanceFront * back.transmittanceFront
                                          * internalTransmittance / denominator,
                    .transmittanceBack = front.transmittanceBack * back.transmittanceBack
                                         * internalTransmittance / denominator,
                    .reflectanceFront = front.reflectanceFront
                                        + frontRoundTrip * back.reflectanceFront * tauSquared
                                            / denominator,
                    .reflectanceBack = back.reflectanceBack
                                       + backRoundTrip * front.reflectanceBack * tauSquared
                                           / denominator};
        }

        //! Interfaces and bulk layers of an assembly, from the front to the back. The number of
        //! interfaces is the number of bulk layers plus one. Assembly keeps this rule.
        struct Components
        {
            const std::vector<Interface> & interfaces;
            const std::vector<Bulk> & bulks;
        };

        [[nodiscard]] PointProperties constructAt(const Components & components, const size_t index)
        {
            auto result{interfaceAt(components.interfaces.front(), index)};
            for(const auto layer : std::views::iota(size_t{0}, components.bulks.size()))
            {
                result = add(result,
                             components.bulks[layer].internalTransmittance[index].value(),
                             interfaceAt(components.interfaces[layer + 1], index));
            }
            return result;
        }

        [[nodiscard]] std::vector<const CSeries *> allSeries(const Components & components)
        {
            std::vector<const CSeries *> result;
            for(const auto & layerInterface : components.interfaces)
            {
                result.push_back(&layerInterface.transmittanceFront);
                result.push_back(&layerInterface.transmittanceBack);
                result.push_back(&layerInterface.reflectanceFront);
                result.push_back(&layerInterface.reflectanceBack);
            }
            for(const auto & bulk : components.bulks)
            {
                result.push_back(&bulk.internalTransmittance);
            }
            return result;
        }

        [[nodiscard]] std::vector<PointProperties> constructAllPoints(const Components & components)
        {
            const auto size{components.interfaces.front().transmittanceFront.size()};
            std::vector<PointProperties> points(size);
            std::ranges::transform(
              std::views::iota(size_t{0}, size), points.begin(), [&](const size_t index) {
                  return constructAt(components, index);
              });
            return points;
        }

        [[nodiscard]] ExternalProperties construct(const Components & components)
        {
            Helper::requireSameWavelengths(allSeries(components));
            const auto wavelengths{components.interfaces.front().transmittanceFront.getXArray()};
            const auto points{constructAllPoints(components)};
            const auto series{[&](double PointProperties::* member) {
                return Helper::makeSeries(
                  wavelengths, [&](const size_t index) { return points[index].*member; });
            }};
            return {.transmittanceFront = series(&PointProperties::transmittanceFront),
                    .transmittanceBack = series(&PointProperties::transmittanceBack),
                    .reflectanceFront = series(&PointProperties::reflectanceFront),
                    .reflectanceBack = series(&PointProperties::reflectanceBack)};
        }

        //! A part that is known only as a whole acts in an assembly as one interface with the
        //! external properties of the part.
        [[nodiscard]] Interface asInterface(const ExternalProperties & part)
        {
            return {.transmittanceFront = part.transmittanceFront,
                    .transmittanceBack = part.transmittanceBack,
                    .reflectanceFront = part.reflectanceFront,
                    .reflectanceBack = part.reflectanceBack};
        }
    }   // namespace

    Assembly::Assembly(const Interface & frontPart) : m_Interfaces{frontPart}
    {}

    Assembly::Assembly(const ExternalProperties & frontPart) : m_Interfaces{asInterface(frontPart)}
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
        result.m_Bulks.push_back(bulk);
        std::ranges::copy(part.m_Interfaces, std::back_inserter(result.m_Interfaces));
        std::ranges::copy(part.m_Bulks, std::back_inserter(result.m_Bulks));
        return result;
    }

    Assembly Assembly::flipped() const
    {
        auto result{*this};
        std::ranges::reverse(result.m_Bulks);
        std::ranges::transform(m_Interfaces | std::views::reverse,
                               result.m_Interfaces.begin(),
                               [](const Interface & layerInterface) {
                                   return layerInterface.flipped();
                               });
        return result;
    }

    ExternalProperties Assembly::externalProperties() const
    {
        return construct(Components{.interfaces = m_Interfaces, .bulks = m_Bulks});
    }

    Interface nonReflectingInterface(const std::vector<double> & wavelengths)
    {
        const auto constant{[&](const double value) {
            return Helper::makeSeries(wavelengths, [=](const size_t) { return value; });
        }};
        return {.transmittanceFront = constant(1.0),
                .transmittanceBack = constant(1.0),
                .reflectanceFront = constant(0.0),
                .reflectanceBack = constant(0.0)};
    }

    Interface Interface::flipped() const
    {
        return {.transmittanceFront = transmittanceBack,
                .transmittanceBack = transmittanceFront,
                .reflectanceFront = reflectanceBack,
                .reflectanceBack = reflectanceFront};
    }

    ExternalProperties ExternalProperties::flipped() const
    {
        return {.transmittanceFront = transmittanceBack,
                .transmittanceBack = transmittanceFront,
                .reflectanceFront = reflectanceBack,
                .reflectanceBack = reflectanceFront};
    }

    Bulk withThickness(const Bulk & bulk, const double oldThickness, const double newThickness)
    {
        if(oldThickness <= 0.0 || newThickness < 0.0)
        {
            throw std::runtime_error("Compound product: thickness must be positive.");
        }
        const auto exponent{newThickness / oldThickness};
        const auto & tau{bulk.internalTransmittance};
        return {.internalTransmittance =
                  Helper::makeSeries(tau.getXArray(), [&](const size_t index) {
                      return std::pow(std::max(tau[index].value(), 0.0), exponent);
                  })};
    }

    std::vector<double> commonWavelengths(const std::vector<ExternalProperties> & products)
    {
        std::vector<double> result;
        for(const auto & product : products)
        {
            std::ranges::copy(product.transmittanceFront.getXArray(),
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
