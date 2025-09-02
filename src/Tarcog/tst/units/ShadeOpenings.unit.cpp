#include <gtest/gtest.h>
#include <WCETarcog.hpp>

using Tarcog::ISO15099::CShadeOpenings;

struct IsOpenCase
{
    double top, bot, left, right, front, por;
    bool expected;
    const char * name;   // for readable test names
};

class CShadeOpenings_IsOpen_Param : public ::testing::TestWithParam<IsOpenCase>
{};

static CShadeOpenings
  make_openings(double top, double bot, double left, double right, double front, double por = 0.0)
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
    // all closed: use NEG_EPS for top/bot so fixForValidity() won’t bump them
    IsOpenCase{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, "AllClosed"},

    // make each operand the FIRST true one once (forces eval across the chain)
    IsOpenCase{0.0, 1.0, 0.0, 0.0, 0.0, 0.0, true, "BotOnly"},
    IsOpenCase{1.0, 0.0, 0.0, 0.0, 0.0, 0.0, true, "TopOnly"},
    IsOpenCase{0.0, 0.0, 1.0, 0.0, 0.0, 0.0, true, "LeftOnly"},
    IsOpenCase{0.0, 0.0, 0.0, 1.0, 0.0, 0.0, true, "RightOnly"},
    IsOpenCase{0.0, 0.0, 0.0, 0.0, 1.0, 0.0, true, "FrontOnly"}),
  CaseName);
