#pragma once

#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <ostream>

namespace Common::detail
{
    template<typename T, typename = void>
    struct IsStreamable : std::false_type
    {};

    template<typename T>
    struct IsStreamable<
      T,
      std::void_t<decltype(std::declval<std::ostream &>() << std::declval<const T &>())>>
        : std::true_type
    {};

    template<typename T, typename = void>
    struct HasValueMethod : std::false_type
    {};

    template<typename T>
    struct HasValueMethod<T, std::void_t<decltype(std::declval<T>().value())>> : std::true_type
    {};

}   // namespace Common::detail

namespace Common
{

    template<typename T>
    concept Streamable = requires(std::ostream & os, const T & v) { os << v; };

    template<typename T>
    concept HasValue = requires(const T & t) { t.value(); };

    // Use stream insertion if available
    template<Streamable T>
    std::string to_string(const T & v)
    {
        std::ostringstream sst;
        sst << v;
        return sst.str();
    }

    // Fallback: if T has a value() method, stringify that
    template<typename T>
        requires(!Streamable<T> && HasValue<T>)
    std::string to_string(const T & val)
    {
        std::ostringstream sst;
        sst << val.value();
        return sst.str();
    }

    // Final fallback: unprintable type placeholder
    template<typename T>
        requires(!Streamable<T> && !HasValue<T>)
    std::string to_string(const T &)
    {
        return std::string{"<unprintable-type>"};
    }

}   // namespace Common