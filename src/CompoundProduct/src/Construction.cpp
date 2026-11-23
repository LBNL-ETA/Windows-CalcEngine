#include <algorithm>
#include <cmath>
#include <ranges>
#include <stdexcept>

#include "Construction.hpp"
#include "SeriesUtilities.hpp"

using FenestrationCommon::CSeries;

namespace CompoundProduct
{
    namespace
    {
        //! External properties of the part of the stack that has been processed so far.
        struct PointProperties
        {
            double transmittance{0};
            double reflectanceFront{0};
            double reflectanceBack{0};
        };

        [[nodiscard]] PointProperties interfaceAt(const Interface & layerInterface, const size_t index)
        {
            return {.transmittance = layerInterface.transmittance[index].value(),
                    .reflectanceFront = layerInterface.reflectanceFront[index].value(),
                    .reflectanceBack = layerInterface.reflectanceBack[index].value()};
        }

        //! Adds one bulk layer and the interface behind it to already processed part of stack.
        [[nodiscard]] PointProperties addLayer(const PointProperties & previous,
                                               const double internalTransmittance,
                                               const PointProperties & layerInterface)
        {
            const auto tauSquared{internalTransmittance * internalTransmittance};
            const auto denominator{Helper::nonZero(
              1.0 - previous.reflectanceBack * layerInterface.reflectanceFront * tauSquared)};
            return {.transmittance = previous.transmittance * layerInterface.transmittance
                                     * internalTransmittance / denominator,
                    .reflectanceFront = previous.reflectanceFront
                                        + previous.transmittance * previous.transmittance
                                            * layerInterface.reflectanceFront * tauSquared
                                            / denominator,
                    .reflectanceBack = layerInterface.reflectanceBack
                                       + layerInterface.transmittance * layerInterface.transmittance
                                           * previous.reflectanceBack * tauSquared / denominator};
        }

        [[nodiscard]] PointProperties constructAt(const Stack & stack, const size_t index)
        {
            auto result{interfaceAt(stack.interfaces.front(), index)};
            for(const auto layer : std::views::iota(size_t{0}, stack.bulks.size()))
            {
                result = addLayer(result,
                                  stack.bulks[layer].internalTransmittance[index].value(),
                                  interfaceAt(stack.interfaces[layer + 1], index));
            }
            return result;
        }

        [[nodiscard]] std::vector<const CSeries *> allSeries(const Stack & stack)
        {
            std::vector<const CSeries *> result;
            for(const auto & layerInterface : stack.interfaces)
            {
                result.push_back(&layerInterface.transmittance);
                result.push_back(&layerInterface.reflectanceFront);
                result.push_back(&layerInterface.reflectanceBack);
            }
            for(const auto & bulk : stack.bulks)
            {
                result.push_back(&bulk.internalTransmittance);
            }
            return result;
        }

        void validate(const Stack & stack)
        {
            if(stack.interfaces.size() != stack.bulks.size() + 1)
            {
                throw std::runtime_error("Compound product: number of interfaces must be number "
                                         "of bulk layers plus one.");
            }
            Helper::requireSameWavelengths(allSeries(stack));
        }

        [[nodiscard]] std::vector<PointProperties> constructAllPoints(const Stack & stack)
        {
            const auto size{stack.interfaces.front().transmittance.size()};
            std::vector<PointProperties> points(size);
            std::ranges::transform(std::views::iota(size_t{0}, size),
                                   points.begin(),
                                   [&](const size_t index) { return constructAt(stack, index); });
            return points;
        }
    }   // namespace

    ExternalProperties construct(const Stack & stack)
    {
        validate(stack);
        const auto wavelengths{stack.interfaces.front().transmittance.getXArray()};
        const auto points{constructAllPoints(stack)};
        return {.transmittance = Helper::makeSeries(
                  wavelengths, [&](const size_t index) { return points[index].transmittance; }),
                .reflectanceFront = Helper::makeSeries(
                  wavelengths, [&](const size_t index) { return points[index].reflectanceFront; }),
                .reflectanceBack = Helper::makeSeries(
                  wavelengths, [&](const size_t index) { return points[index].reflectanceBack; })};
    }

    Interface nonReflectingInterface(const std::vector<double> & wavelengths)
    {
        const auto constant{[&](const double value) {
            return Helper::makeSeries(wavelengths, [=](const size_t) { return value; });
        }};
        return {.transmittance = constant(1.0),
                .reflectanceFront = constant(0.0),
                .reflectanceBack = constant(0.0)};
    }

    Interface flipped(const Interface & layerInterface)
    {
        return {.transmittance = layerInterface.transmittance,
                .reflectanceFront = layerInterface.reflectanceBack,
                .reflectanceBack = layerInterface.reflectanceFront};
    }

    Stack flipped(const Stack & stack)
    {
        Stack result;
        std::ranges::transform(stack.interfaces | std::views::reverse,
                               std::back_inserter(result.interfaces),
                               [](const Interface & layerInterface) { return flipped(layerInterface); });
        std::ranges::copy(stack.bulks | std::views::reverse, std::back_inserter(result.bulks));
        return result;
    }

    Stack joined(const Stack & front, const Stack & back, const Interface & between)
    {
        if(front.interfaces.empty() || back.interfaces.empty())
        {
            throw std::runtime_error("Compound product: cannot join an empty stack.");
        }
        Stack result;
        std::ranges::copy(front.interfaces | std::views::take(front.interfaces.size() - 1),
                          std::back_inserter(result.interfaces));
        result.interfaces.push_back(between);
        std::ranges::copy(back.interfaces | std::views::drop(1),
                          std::back_inserter(result.interfaces));
        std::ranges::copy(front.bulks, std::back_inserter(result.bulks));
        std::ranges::copy(back.bulks, std::back_inserter(result.bulks));
        return result;
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
            std::ranges::copy(product.transmittance.getXArray(), std::back_inserter(result));
        }
        std::ranges::sort(result);
        const auto duplicates{std::ranges::unique(result, [](const double left, const double right) {
            return std::abs(left - right) < Helper::WavelengthTolerance;
        })};
        result.erase(duplicates.begin(), duplicates.end());
        return result;
    }
}   // namespace CompoundProduct
