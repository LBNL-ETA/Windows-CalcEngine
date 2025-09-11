#pragma once

#include <type_traits>
#include <ranges>
#include <vector>

namespace FenestrationCommon
{

    // C++20 helper (avoid name clash with C++23 std::to_underlying)
    template<class E>
    constexpr auto toUnderlying(E e) noexcept
    {
        return static_cast<std::underlying_type_t<E>>(e);
    }

    // Declare per-enum inclusive bounds:
    //   template<> struct EnumBounds<MyEnum> { static constexpr MyEnum first = ...; static
    //   constexpr MyEnum last = ...; };
    template<class E>
    struct EnumBounds;

    // View of all values of a contiguous enum [first, last].
    template<class E>
        requires std::is_enum_v<E>
    constexpr auto enumValues()
    {
        using U = std::underlying_type_t<E>;
        constexpr U first = toUnderlying(EnumBounds<E>::first);
        constexpr U last = toUnderlying(EnumBounds<E>::last);
        static_assert(first <= last, "EnumBounds first/last invalid");

        return std::ranges::views::iota(first, last + 1)
               | std::ranges::views::transform([](U v) { return static_cast<E>(v); });
    }

    // Optional: materialize into a vector when algorithms need containers
    template<class E>
    auto enumVector()
    {
        auto v = enumValues<E>();
        return std::vector<E>(v.begin(), v.end());
    }

}   // namespace FenestrationCommon
