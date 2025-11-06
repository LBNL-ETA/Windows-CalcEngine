#include <gtest/gtest.h>
#include <WCETarcog.hpp>
#include <string>

using Tarcog::ISO15099::CShadeOpenings;

struct IsOpenCase {
    double top{};
    double bot{};
    double left{};
    double right{};
    double front{};
    double por{};
    bool expected{};
    std::string name;   // safer than const char*
};

class CShadeOpenings_IsOpen_Param : public ::testing::TestWithParam<IsOpenCase> {};

static CShadeOpenings make_openings(double top, double bot, double left, double right,
                                    double front, double por = 0.0)
{
    return CShadeOpenings(top, bot, left, right, front, por);
}

TEST_P(CShadeOpenings_IsOpen_Param, Works)
{
    const auto & c = GetParam();
    auto o = make_openings(c.top, c.bot, c.left, c.right, c.front, c.por);
    EXPECT_EQ(o.isOpen(), c.expected);
}

// Pretty names in test output
static std::string CaseName(const ::testing::TestParamInfo<IsOpenCase> & info)
{
    return info.param.name;
}

INSTANTIATE_TEST_SUITE_P(
    AllCases,
    CShadeOpenings_IsOpen_Param,
    ::testing::Values(
        IsOpenCase{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, "AllClosed"},
        IsOpenCase{0.0, 1.0, 0.0, 0.0, 0.0, 0.0, true,  "BotOnly"},
        IsOpenCase{1.0, 0.0, 0.0, 0.0, 0.0, 0.0, true,  "TopOnly"},
        IsOpenCase{0.0, 0.0, 1.0, 0.0, 0.0, 0.0, true,  "LeftOnly"},
        IsOpenCase{0.0, 0.0, 0.0, 1.0, 0.0, 0.0, true,  "RightOnly"},
        IsOpenCase{0.0, 0.0, 0.0, 0.0, 1.0, 0.0, true,  "FrontOnly"}
    ),
    CaseName);
