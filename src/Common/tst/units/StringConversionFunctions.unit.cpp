#include <string>
#include <ostream>

#include <gtest/gtest.h>
#include <WCECommon.hpp>

using Common::to_string;

// Streamable-only type (no value())
struct StreamableOnly
{
    int v;
};

static std::ostream & operator<<(std::ostream & os, const StreamableOnly & s)
{
    return os << "S:" << s.v;
}

// Has value() but not streamable
struct WithValue
{
    double value() const { return 3.14; }
};

// Both streamable and has value() - streamable should be used
struct Both
{
    int value() const { return 7; }
    friend std::ostream & operator<<(std::ostream & os, const Both & b)
    {
        return os << "B(" << b.value() << ")";
    }
};

// Unprintable: no operator<< and no value()
struct Unprintable
{
    int x;
};

TEST(ToString, BuiltInTypes)
{
    EXPECT_EQ(to_string(123), "123");
    EXPECT_EQ(to_string(std::string{"hello"}), "hello");
}

TEST(ToString, StreamableOnly)
{
    StreamableOnly s{42};
    EXPECT_EQ(to_string(s), "S:42");
}

TEST(ToString, WithValue)
{
    WithValue w;
    EXPECT_EQ(to_string(w), "3.14");
}

TEST(ToString, PrefersStreamableWhenBoth)
{
    Both b;
    EXPECT_EQ(to_string(b), "B(7)");
}

TEST(ToString, UnprintableFallback)
{
    Unprintable u{1};
    EXPECT_EQ(to_string(u), "<unprintable-type>");
}