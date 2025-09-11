#pragma once
#include <map>
#include <tuple>
#include <optional>   // optional version shown below; if you prefer pointers, see alt API

namespace FenestrationCommon
{

    template<typename T, typename... Values>
    class mmap
    {
    public:
        using key_type = std::tuple<Values...>;
        using map_type = std::map<key_type, T>;
        using size_type = typename map_type::size_type;

        mmap() = default;

        // Insert-or-access (like operator[]). Creates a default T if missing.
        T & operator()(const Values &... a)
        {
            return types_[key_type(a...)];
        }

        // at(): reference-returning, throws std::out_of_range if not found
        T & at(const Values &... a)
        {
            return types_.at(key_type(a...));
        }
        const T & at(const Values &... a) const
        {
            return types_.at(key_type(a...));
        }

        // Non-throwing lookups
        [[nodiscard]] bool contains(const Values &... a) const noexcept
        {
            return types_.find(key_type(a...)) != types_.end();
        }

        // Optional-returning getter (const)
        [[nodiscard]] std::optional<std::reference_wrapper<const T>>
          get(const Values &... a) const noexcept
        {
            if(auto it = types_.find(key_type(a...)); it != types_.end())
                return std::cref(it->second);
            return std::nullopt;
        }

        // Optional-returning getter (mutable)
        [[nodiscard]] std::optional<std::reference_wrapper<T>> get(const Values &... a) noexcept
        {
            if(auto it = types_.find(key_type(a...)); it != types_.end())
                return std::ref(it->second);
            return std::nullopt;
        }

        // Basic container ops you’ll likely want
        void clear() noexcept
        {
            types_.clear();
        }
        [[nodiscard]] size_type size() const noexcept
        {
            return types_.size();
        }
        [[nodiscard]] bool empty() const noexcept
        {
            return types_.empty();
        }

        // Expose iterators if needed
        auto begin() noexcept
        {
            return types_.begin();
        }
        auto end() noexcept
        {
            return types_.end();
        }
        auto begin() const noexcept
        {
            return types_.begin();
        }
        auto end() const noexcept
        {
            return types_.end();
        }

    private:
        map_type types_;
    };

}   // namespace FenestrationCommon
