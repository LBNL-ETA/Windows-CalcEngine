// EnumArray.hpp
#pragma once
#include <array>
#include <stdexcept>
#include <type_traits>

namespace util
{

    template<typename Enum>
    struct enum_index;   // ← specialize this for your enum (size, to_index)

    template<typename Enum, typename T>
    class EnumArray
    {
        static_assert(std::is_enum_v<Enum>, "EnumArray requires an enum type");

    public:
        static constexpr std::size_t size_v = enum_index<Enum>::size;
        using value_type = T;

        constexpr EnumArray() = default;

        // Keep your existing syntax: arr[Enum::X] and arr.at(Enum::X)
        T & at(Enum key)
        {
            const auto idx = enum_index<Enum>::to_index(key);
            if(idx >= size_v)
                throw std::out_of_range("EnumArray::at");
            return data_.at(idx);
        }
        const T & at(Enum key) const
        {
            const auto idx = enum_index<Enum>::to_index(key);
            if(idx >= size_v)
                throw std::out_of_range("EnumArray::at");
            return data_.at(idx);
        }

        T & operator[](Enum key)
        {
            return data_.at(enum_index<Enum>::to_index(key));
        }
        const T & operator[](Enum key) const
        {
            return data_.at(enum_index<Enum>::to_index(key));
        }

        // Convenience forwards
        constexpr std::size_t size() const noexcept
        {
            return size_v;
        }
        void fill(const T & value)
        {
            data_.fill(value);
        }
        auto begin() noexcept
        {
            return data_.begin();
        }
        auto end() noexcept
        {
            return data_.end();
        }
        auto begin() const noexcept
        {
            return data_.begin();
        }
        auto end() const noexcept
        {
            return data_.end();
        }

    private:
        std::array<T, size_v> data_{};
    };

}   // namespace util
