#pragma once

#include <cassert>
#include <map>

namespace CMA
{
    enum class Option
    {
        Best,
        Worst
    };

    enum class CMAProperty
    {
        Glass,
        Spacer
    };

    template<typename T>
    class BestWorst
    {
    public:
        BestWorst(T valueBest, T valueWorst) :
            m_Value({{Option::Best, valueBest}, {Option::Worst, valueWorst}})
        {}

        [[nodiscard]] T value(Option option) const
        {
            assert(m_Value.count(option) != 0);
            return m_Value.at(option);
        }

    private:
        std::map<Option, T> m_Value;
    };

    class ICMAWindow
    {
    public:
        virtual ~ICMAWindow() = default;

        [[nodiscard]] virtual double uValue() const = 0;
        [[nodiscard]] virtual double shgc() const = 0;
        [[nodiscard]] virtual double vt() const = 0;
    };
}   // namespace CMA
