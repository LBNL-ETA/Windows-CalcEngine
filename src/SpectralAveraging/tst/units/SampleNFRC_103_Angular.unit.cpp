#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCECommon.hpp>

#include "standardData.hpp"
#include "spectralSampleData.hpp"
#include "parametrics.hpp"

using namespace SpectralAveraging;
using namespace FenestrationCommon;

using TestUtil::AngleCase;
using TestUtil::angleParamNames;

class TestSampleNFRC_103_Angular : public testing::Test
{
    const double thickness{5.715e-3};
    CAngularSpectralSample m_Sample{
      {SpectralSample::NFRC_103(), StandardData::solarRadiationASTM_E891_87_Table1()},
      thickness,
      MaterialType::Monolithic};

public:
    CAngularSpectralSample & getSample()
    {
        return m_Sample;
    }
};

// -------------------- Parameterized suite --------------------

class TestSampleNFRC_103_Angular_Param : public TestSampleNFRC_103_Angular,
                                         public ::testing::WithParamInterface<AngleCase>
{};

TEST_P(TestSampleNFRC_103_Angular_Param, PropertiesMatch_TotalScatter)
{
    constexpr double lowLambda = 0.3;
    constexpr double highLambda = 2.5;
    constexpr double eps = 1e-6;

    auto & angular{getSample()};
    const auto & c{GetParam()};

    auto get = [&](Property p, Side s) {
        return angular.getProperty(lowLambda, highLambda, p, s, c.angle, c.scatter);
    };

    EXPECT_NEAR(c.Tf, get(Property::T, Side::Front), eps);
    EXPECT_NEAR(c.Tb, get(Property::T, Side::Back), eps);
    EXPECT_NEAR(c.Rf, get(Property::R, Side::Front), eps);
    EXPECT_NEAR(c.Rb, get(Property::R, Side::Back), eps);
    EXPECT_NEAR(c.Absf, get(Property::Abs, Side::Front), eps);
    EXPECT_NEAR(c.Absb, get(Property::Abs, Side::Back), eps);
}

INSTANTIATE_TEST_SUITE_P(
  AngularProps_NFRC103_Total,
  TestSampleNFRC_103_Angular_Param,
  ::testing::Values(
    // clang-format off
    // angle   scatter                              Tf        Tb        Rf        Rb        Absf      Absb
        // ---- 0° ----
        AngleCase{  0.0, ScatteringType::Total,   0.770680, 0.770680, 0.069984, 0.070251, 0.159336, 0.159069 },
        AngleCase{  0.0, ScatteringType::Direct,  0.770680, 0.770680, 0.069984, 0.070251, 0.159336, 0.159069 },
        AngleCase{  0.0, ScatteringType::Diffuse, 0.000000, 0.000000, 0.000000, 0.000000, 0.159336, 0.159069 },

        // ---- 10° ----
        AngleCase{ 10.0, ScatteringType::Total,   0.769803, 0.769807, 0.069951, 0.070217, 0.160246, 0.159976 },
        AngleCase{ 10.0, ScatteringType::Direct,  0.769803, 0.769807, 0.069951, 0.070217, 0.160246, 0.159976 },
        AngleCase{ 10.0, ScatteringType::Diffuse, 0.000000, 0.000000, 0.000000, 0.000000, 0.160246, 0.159976 },

        // ---- 20° ----
        AngleCase{ 20.0, ScatteringType::Total,   0.766911, 0.766927, 0.070136, 0.070402, 0.162953, 0.162671 },
        AngleCase{ 20.0, ScatteringType::Direct,  0.766911, 0.766927, 0.070136, 0.070402, 0.162953, 0.162671 },
        AngleCase{ 20.0, ScatteringType::Diffuse, 0.000000, 0.000000, 0.000000, 0.000000, 0.162953, 0.162671 },

        // ---- 30° ----
        AngleCase{ 30.0, ScatteringType::Total,   0.761038, 0.761072, 0.071607, 0.071875, 0.167355, 0.167053 },
        AngleCase{ 30.0, ScatteringType::Direct,  0.761038, 0.761072, 0.071607, 0.071875, 0.167355, 0.167053 },
        AngleCase{ 30.0, ScatteringType::Diffuse, 0.000000, 0.000000, 0.000000, 0.000000, 0.167355, 0.167053 },

        // ---- 40° ----
        AngleCase{ 40.0, ScatteringType::Total,   0.749796, 0.749854, 0.077031, 0.077303, 0.173173, 0.172843 },
        AngleCase{ 40.0, ScatteringType::Direct,  0.749796, 0.749854, 0.077031, 0.077303, 0.173173, 0.172843 },
        AngleCase{ 40.0, ScatteringType::Diffuse, 0.000000, 0.000000, 0.000000, 0.000000, 0.173173, 0.172843 },

        // ---- 50° ----
        AngleCase{ 50.0, ScatteringType::Total,   0.727476, 0.727561, 0.092803, 0.093085, 0.179721, 0.179354 },
        AngleCase{ 50.0, ScatteringType::Direct,  0.727476, 0.727561, 0.092803, 0.093085, 0.179721, 0.179354 },
        AngleCase{ 50.0, ScatteringType::Diffuse, 0.000000, 0.000000, 0.000000, 0.000000, 0.179721, 0.179354 },

        // ---- 60° ----
        AngleCase{ 60.0, ScatteringType::Total,   0.680213, 0.680322, 0.134396, 0.134693, 0.185391, 0.184985 },
        AngleCase{ 60.0, ScatteringType::Direct,  0.680213, 0.680322, 0.134396, 0.134693, 0.185391, 0.184985 },
        AngleCase{ 60.0, ScatteringType::Diffuse, 0.000000, 0.000000, 0.000000, 0.000000, 0.185391, 0.184985 },

        // ---- 70° ----
        AngleCase{ 70.0, ScatteringType::Total,   0.574836, 0.574959, 0.238890, 0.239199, 0.186274, 0.185842 },
        AngleCase{ 70.0, ScatteringType::Direct,  0.574836, 0.574959, 0.238890, 0.239199, 0.186274, 0.185842 },
        AngleCase{ 70.0, ScatteringType::Diffuse, 0.000000, 0.000000, 0.000000, 0.000000, 0.186274, 0.185842 },

        // ---- 80° ----
        AngleCase{ 80.0, ScatteringType::Total,   0.345876, 0.345984, 0.483999, 0.484279, 0.170124, 0.169737 },
        AngleCase{ 80.0, ScatteringType::Direct,  0.345876, 0.345984, 0.483999, 0.484279, 0.170124, 0.169737 },
        AngleCase{ 80.0, ScatteringType::Diffuse, 0.000000, 0.000000, 0.000000, 0.000000, 0.170124, 0.169737 },

        // ---- 90° ----
        AngleCase{ 90.0, ScatteringType::Total,   0.000000, 0.000000, 1.000000, 1.000000, 0.000000, 0.000000 },
        AngleCase{ 90.0, ScatteringType::Direct,  0.000000, 0.000000, 1.000000, 1.000000, 0.000000, 0.000000 },
        AngleCase{ 90.0, ScatteringType::Diffuse, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000 }
    // clang-format on
    ),
  angleParamNames);
