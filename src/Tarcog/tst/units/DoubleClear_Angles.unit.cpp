#include <memory>
#include <optional>
#include <iomanip>
#include <sstream>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>
#include <WCECommon.hpp>

namespace
{

    using Tarcog::ISO15099::AirHorizontalDirection;
    using Tarcog::ISO15099::BoundaryConditionsCoeffModel;
    using Tarcog::ISO15099::CIGU;
    using Tarcog::ISO15099::CSystem;
    using Tarcog::ISO15099::Environments;
    using Tarcog::ISO15099::Environment;
    using Tarcog::ISO15099::SkyModel;
    using Tarcog::ISO15099::System;

    struct TiltCase
    {
        double tilt_deg;
        std::optional<double> expected_hc_indoor;   // leave nullopt to “discover & record”
    };

    // helper to build the system with a given tilt
    static std::shared_ptr<CSystem> make_system(double tilt_deg)
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        constexpr auto airTemperature = 255.15;   // K
        constexpr auto airSpeed = 5.5;            // m/s
        constexpr auto pressure = 101325.0;       // Pa
        constexpr auto airDirection = AirHorizontalDirection::Windward;
        constexpr auto tSky = 255.15;   // K
        constexpr auto solarRadiation = 0.0;

        auto Outdoor = Environments::outdoor(airTemperature,
                                             airSpeed,
                                             solarRadiation,
                                             tSky,
                                             SkyModel::AllSpecified,
                                             pressure,
                                             airDirection);
        EXPECT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////
        constexpr auto roomTemperature = 294.15;   // K
        auto Indoor = Environments::indoor(roomTemperature);
        EXPECT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        constexpr auto solidLayerThickness = 0.005715;   // m
        constexpr auto solidLayerConductance = 1.0;

        auto layer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        auto layer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);

        constexpr auto gapThickness = 0.012;   // m
        auto gap = Tarcog::ISO15099::Layers::gap(gapThickness);
        EXPECT_TRUE(gap != nullptr);

        constexpr auto windowWidth = 1.0;
        constexpr auto windowHeight = 1.0;

        CIGU igu(windowWidth, windowHeight, tilt_deg);
        igu.addLayers({layer1, gap, layer2});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        auto sys = std::make_shared<CSystem>(igu, Indoor, Outdoor);
        EXPECT_TRUE(sys != nullptr);
        return sys;
    }

    // pretty param names like Tilt_90deg, Tilt_10deg
    auto tiltParamName = [](const ::testing::TestParamInfo<TiltCase> & info) {
        std::ostringstream os;
        os.setf(std::ios::fixed);
        os << "Tilt_" << std::setprecision(1) << info.param.tilt_deg << "deg";
        auto s = os.str();
        for(char & c : s)
            if(c == '.')
                c = '_';   // gtest-friendly
        return s;
    };

    class TestDoubleClearAngles_Param : public ::testing::TestWithParam<TiltCase>
    {};

    TEST_P(TestDoubleClearAngles_Param, HcIndoor_UValue)
    {
        constexpr double Tolerance = 1e-6;

        const auto tc = GetParam();
        auto sys = make_system(tc.tilt_deg);

        const auto hc = sys->getHc(System::Uvalue, Environment::Indoor);

        if(tc.expected_hc_indoor.has_value())
        {
            EXPECT_NEAR(hc, *tc.expected_hc_indoor, Tolerance);
        }
        else
        {
            // Discover & record for convenience; copy this value into the table once you approve
            // it.
            RecordProperty("tilt_deg", tc.tilt_deg);
            RecordProperty("hc_indoor_measured", hc);
            SUCCEED() << "Captured hc for tilt=" << tc.tilt_deg << " deg: hc_indoor=" << std::fixed
                      << std::setprecision(6) << hc;
        }
    }

    // TODO: This clearly shows the bug at angle above 179 degrees
    INSTANTIATE_TEST_SUITE_P(DoubleClear_Tilts,
                             TestDoubleClearAngles_Param,
                             // clang-format off
      ::testing::Values
      (
          TiltCase{10.0,   3.899828},
          TiltCase{15.0,   3.844879},
          TiltCase{16.0,   3.842826},
          TiltCase{89.0,   2.833360},
          TiltCase{90.0,   2.832772},
          TiltCase{178.0,  1.266671},
          TiltCase{179.0,  1.070578},
          TiltCase{179.3,  1.050170}
      ),
                             // clang-format on
                             tiltParamName);

}   // namespace
