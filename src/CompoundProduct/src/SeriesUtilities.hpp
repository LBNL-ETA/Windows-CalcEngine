#pragma once

#include <algorithm>
#include <cmath>
#include <ranges>
#include <stdexcept>
#include <utility>
#include <vector>

#include <WCECommon.hpp>

// Helpers shared by the construction and the deconstruction routines. Not a part of public
// interface of the module.
namespace CompoundProduct::Helper
{
    constexpr double SmallNumber{1e-12};
    constexpr double WavelengthTolerance{1e-9};

    [[nodiscard]] inline bool sameWavelengths(const FenestrationCommon::CSeries & first,
                                              const FenestrationCommon::CSeries & second)
    {
        return first.size() == second.size()
               && std::ranges::equal(first, second, [](const auto & left, const auto & right) {
                      return std::abs(left.x() - right.x()) < WavelengthTolerance;
                  });
    }

    //! Throws if any of the series is not given at the wavelengths of the first one.
    inline void
      requireSameWavelengths(const std::vector<const FenestrationCommon::CSeries *> & series)
    {
        const auto mismatch{std::ranges::any_of(series, [&](const auto * current) {
            return !sameWavelengths(*series.front(), *current);
        })};
        if(mismatch)
        {
            throw std::runtime_error(
              "Compound product: all properties must be given at the same wavelengths.");
        }
    }

    //! Creates series from wavelengths and from a value calculated for every index.
    template<typename ValueAtIndex>
    [[nodiscard]] FenestrationCommon::CSeries makeSeries(const std::vector<double> & wavelengths,
                                                         ValueAtIndex valueAtIndex)
    {
        std::vector<std::pair<double, double>> points(wavelengths.size());
        std::ranges::transform(std::views::iota(size_t{0}, wavelengths.size()),
                               points.begin(),
                               [&](const size_t index) {
                                   return std::pair{wavelengths[index], valueAtIndex(index)};
                               });
        return FenestrationCommon::CSeries(points);
    }

    //! Returns value that can safely be used as denominator.
    [[nodiscard]] inline double nonZero(const double value)
    {
        if(std::abs(value) >= SmallNumber)
        {
            return value;
        }
        return value < 0 ? -SmallNumber : SmallNumber;
    }

    [[nodiscard]] inline bool isPhysical(const double value)
    {
        return value >= 0.0 && value <= 1.0;
    }
}   // namespace CompoundProduct::Helper
