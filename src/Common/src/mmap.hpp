#pragma once

#include <map>

namespace FenestrationCommon
{
    // Template for multimap.
    template<typename T, typename... Values>
    class mmap
    {
    public:
        mmap() = default;

        T & operator()(const Values... a)
        {
            return m_Types[std::make_tuple(a...)];
        }

        T at(const Values... a) const
        {
            return m_Types.at(std::make_tuple(a...));
        }

    private:
        std::map<std::tuple<Values...>, T> m_Types;
    };

}   // namespace FenestrationCommon
