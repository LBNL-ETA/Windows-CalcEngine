// SideEnumIndex.hpp  (or next to where Side is defined)
#pragma once
#include "EnumArray.hpp"
#include "FenestrationCommon.hpp"

namespace util
{

    template<>
    struct enum_index<FenestrationCommon::Side>
    {
        static constexpr std::size_t size = 2;
        static constexpr std::size_t to_index(FenestrationCommon::Side side) noexcept
        {
            switch(side)
            {
                case FenestrationCommon::Side::Front:
                    return 0;
                case FenestrationCommon::Side::Back:
                    return 1;
            }
            return 0;   // defensive fallback
        }
    };

}   // namespace util
