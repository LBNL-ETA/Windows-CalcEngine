#include <memory>
#include <gtest/gtest.h>
#include <algorithm>
#include <sstream>
#include <string>

#include <WCESpectralAveraging.hpp>
#include <WCECommon.hpp>

#include "standardData.hpp"
#include "spectralSampleData.hpp"

using namespace SpectralAveraging;
using namespace FenestrationCommon;

class TestDirectDiffuse_Angular : public ::testing::Test
{
    const double thickness{3e-3};
    CAngularSpectralSample m_Sample{
      {SpectralSample::DirectDiffuse(), StandardData::solarRadiationASTM_E891_87_Table1()},
      thickness,
      MaterialType::Coated};

public:
    CAngularSpectralSample & getSample()
    {
        return m_Sample;
    }
};

// -------------------- Parameterized suite --------------------

struct Case
{
    double angle;
    ScatteringType scatter;
    double Tf, Tb, Rf, Rb, Absf, Absb;
};

class TestDirectDiffuse_Angular_Param : public TestDirectDiffuse_Angular,
                                        public ::testing::WithParamInterface<Case>
{};

TEST_P(TestDirectDiffuse_Angular_Param, PropertiesMatch)
{
    constexpr double lowLambda = 0.3;
    constexpr double highLambda = 2.5;
    constexpr double eps = 1e-6;

    auto & angularSample{getSample()};
    const auto & c = GetParam();

    auto get = [&](Property p, Side s) {
        return angularSample.getProperty(lowLambda, highLambda, p, s, c.angle, c.scatter);
    };

    EXPECT_NEAR(c.Tf, get(Property::T, Side::Front), eps);
    EXPECT_NEAR(c.Tb, get(Property::T, Side::Back), eps);
    EXPECT_NEAR(c.Rf, get(Property::R, Side::Front), eps);
    EXPECT_NEAR(c.Rb, get(Property::R, Side::Back), eps);
    EXPECT_NEAR(c.Absf, get(Property::Abs, Side::Front), eps);
    EXPECT_NEAR(c.Absb, get(Property::Abs, Side::Back), eps);
}

static std::string scatterName(ScatteringType s)
{
    switch(s)
    {
        case ScatteringType::Direct:
            return "Direct";
        case ScatteringType::Diffuse:
            return "Diffuse";
        case ScatteringType::Total:
            return "Total";
        default:
            return "Unknown";
    }
}

static auto nameFromParam = [](const ::testing::TestParamInfo<Case> & info) {
    std::ostringstream os;
    std::string a = std::to_string(info.param.angle);
    std::ranges::replace(a, '.', '_');
    // trim trailing zeros and possible trailing '_'
    while(!a.empty() && (a.back() == '0'))
        a.pop_back();
    if(!a.empty() && a.back() == '.')
        a.pop_back();
    if(!a.empty() && a.back() == '_')
        a.pop_back();

    os << scatterName(info.param.scatter) << "_" << (a.empty() ? "0" : a) << "deg";
    return os.str();
};

INSTANTIATE_TEST_SUITE_P(
  AngularProps,
  TestDirectDiffuse_Angular_Param,
  ::testing::Values(
    // clang-format off
    // ---- 0° ----                        Tf        Tb        Rf        Rb        Absf      Absb
    Case{0.0,  ScatteringType::Total,   0.300000, 0.300000, 0.500000, 0.500000, 0.200000, 0.200000},
    Case{0.0,  ScatteringType::Direct,  0.100000, 0.100000, 0.200000, 0.200000, 0.200000, 0.200000},
    Case{0.0,  ScatteringType::Diffuse, 0.200000, 0.200000, 0.300000, 0.300000, 0.200000, 0.200000},

    // ---- 10° ----
    Case{10.0, ScatteringType::Total,   0.301978, 0.301978, 0.495871, 0.495871, 0.202150, 0.202150},
    Case{10.0, ScatteringType::Direct,  0.100659, 0.100659, 0.198349, 0.198349, 0.202150, 0.202150},
    Case{10.0, ScatteringType::Diffuse, 0.201319, 0.201319, 0.297523, 0.297523, 0.202150, 0.202150},

    // ---- 20° ----
    Case{20.0, ScatteringType::Total,   0.298126, 0.298126, 0.495000, 0.495000, 0.206874, 0.206874},
    Case{20.0, ScatteringType::Direct,  0.099375, 0.099375, 0.198000, 0.198000, 0.206874, 0.206874},
    Case{20.0, ScatteringType::Diffuse, 0.198751, 0.198751, 0.297000, 0.297000, 0.206874, 0.206874},

    // ---- 30° ----
    Case{30.0, ScatteringType::Total,   0.293440, 0.293440, 0.496713, 0.496713, 0.209847, 0.209847},
    Case{30.0, ScatteringType::Direct,  0.097813, 0.097813, 0.198685, 0.198685, 0.209847, 0.209847},
    Case{30.0, ScatteringType::Diffuse, 0.195626, 0.195626, 0.298028, 0.298028, 0.209847, 0.209847},

    // ---- 40° ----
    Case{40.0, ScatteringType::Total,   0.287900, 0.287900, 0.502344, 0.502344, 0.209757, 0.209757},
    Case{40.0, ScatteringType::Direct,  0.095967, 0.095967, 0.200937, 0.200937, 0.209757, 0.209757},
    Case{40.0, ScatteringType::Diffuse, 0.191933, 0.191933, 0.301406, 0.301406, 0.209757, 0.209757},

    // ---- 50° ----
    Case{50.0, ScatteringType::Total,   0.277791, 0.277791, 0.511924, 0.511924, 0.210285, 0.210285},
    Case{50.0, ScatteringType::Direct,  0.092597, 0.092597, 0.204769, 0.204769, 0.210285, 0.210285},
    Case{50.0, ScatteringType::Diffuse, 0.185194, 0.185194, 0.307154, 0.307154, 0.210285, 0.210285},

    // ---- 60° ----
    Case{60.0, ScatteringType::Total,   0.254859, 0.254859, 0.530266, 0.530266, 0.214875, 0.214875},
    Case{60.0, ScatteringType::Direct,  0.084953, 0.084953, 0.212106, 0.212106, 0.214875, 0.214875},
    Case{60.0, ScatteringType::Diffuse, 0.169906, 0.169906, 0.318159, 0.318159, 0.214875, 0.214875},

    // ---- 70° ----
    Case{70.0, ScatteringType::Total,   0.207649, 0.207649, 0.578700, 0.578700, 0.213651, 0.213651},
    Case{70.0, ScatteringType::Direct,  0.069216, 0.069216, 0.231480, 0.231480, 0.213651, 0.213651},
    Case{70.0, ScatteringType::Diffuse, 0.138433, 0.138433, 0.347220, 0.347220, 0.213651, 0.213651},

    // ---- 82.5° ----
    Case{82.5, ScatteringType::Total,   0.097600, 0.097600, 0.761130, 0.761130, 0.141270, 0.141270},
    Case{82.5, ScatteringType::Direct,  0.032533, 0.032533, 0.304452, 0.304452, 0.141270, 0.141270},
    Case{82.5, ScatteringType::Diffuse, 0.065067, 0.065067, 0.456678, 0.456678, 0.141270, 0.141270}
    // clang-format on
    ),
  nameFromParam);
