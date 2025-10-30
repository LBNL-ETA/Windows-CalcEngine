#pragma once

#include <tuple>
#include <ranges>
#include <type_traits>
#include <algorithm>
#include <cassert>

namespace common::views
{

    // Requirements: sized + random_access (vectors, arrays, spans, etc.)
    template<class R>
    concept indexable_range = std::ranges::sized_range<R> && std::ranges::random_access_range<R>;

    // --- detail helpers ---------------------------------------------------------

    namespace detail
    {

        // get min(size(rs)...) safely
        template<class... Rs>
        constexpr std::size_t min_size(const Rs &... rs)
        {
            using std::ranges::size;
            // Brackets around std::min are because of MFC library used by THERM and WINDOW
            return (std::min)({static_cast<std::size_t>(size(rs))...});
        }

        // access by index via random-access iterator
        template<class R>
        constexpr decltype(auto) at(R & r, std::size_t i)
        {
            using std::ranges::begin;
            return begin(r)[static_cast<std::ptrdiff_t>(i)];
        }

    }   // namespace detail

    // --- zip: lvalue ranges, references are preserved in tuples -----------------

    // non-const overload (mutating allowed through the references)
    template<indexable_range... Rs>
    auto zip(Rs &... rs)
    {
        const std::size_t n = detail::min_size(rs...);
        return std::views::iota(std::size_t{0}, n) | std::views::transform([&rs...](std::size_t i) {
                   return std::tie(detail::at(rs, i)...);
               });
    }

    // const overload (yields const references)
    template<indexable_range... Rs>
    auto zip(const Rs &... rs)
    {
        const std::size_t n = detail::min_size(rs...);
        return std::views::iota(std::size_t{0}, n) | std::views::transform([&rs...](std::size_t i) {
                   return std::tie(detail::at(rs, i)...);
               });
    }

    // --- zip_strict: same as zip but asserts equal sizes ------------------------

    template<indexable_range... Rs>
    auto zip_strict(Rs &... rs)
    {
        using std::ranges::size;
        assert(((size(rs) == size(std::get<0>(std::tie(rs...)))) && ...)
               && "zip_strict: ranges must have equal sizes");
        const std::size_t n =
          static_cast<std::size_t>(std::ranges::size(std::get<0>(std::tie(rs...))));
        return std::views::iota(std::size_t{0}, n) | std::views::transform([&rs...](std::size_t i) {
                   return std::tie(detail::at(rs, i)...);
               });
    }

    template<indexable_range... Rs>
    auto zip_strict(const Rs &... rs)
    {
        using std::ranges::size;
        assert(((size(rs) == size(std::get<0>(std::tie(rs...)))) && ...)
               && "zip_strict: ranges must have equal sizes");
        const std::size_t n =
          static_cast<std::size_t>(std::ranges::size(std::get<0>(std::tie(rs...))));
        return std::views::iota(std::size_t{0}, n) | std::views::transform([&rs...](std::size_t i) {
                   return std::tie(detail::at(rs, i)...);
               });
    }

    // --- enumerate: yields (index by value, element by reference) ---------------

    template<indexable_range R>
    auto enumerate(R & r)
    {
        using Ref = std::ranges::range_reference_t<R>;   // e.g. T&
        const std::size_t n = static_cast<std::size_t>(std::ranges::size(r));
        return std::views::iota(std::size_t{0}, n)
               | std::views::transform([&r](std::size_t i) -> std::tuple<std::size_t, Ref> {
                     return {i, detail::at(r, i)};   // index by value, elem by ref
                 });
    }

    template<indexable_range R>
    auto enumerate(const R & r)
    {
        using Ref = std::ranges::range_reference_t<const R>;   // e.g. const T&
        const std::size_t n = static_cast<std::size_t>(std::ranges::size(r));
        return std::views::iota(std::size_t{0}, n)
               | std::views::transform([&r](std::size_t i) -> std::tuple<std::size_t, Ref> {
                     return {i, detail::at(r, i)};
                 });
    }


}   // namespace common::views
