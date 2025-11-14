// SingleVisionWindow.unit.cpp

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Some tests in this module are validated vs WINDOW 7.8
/// Note that WINDOW will report frame incompatibility when trying to combine some frames with
/// certain IGUs. This however can be controlled through INI file settings:
///
/// FrameToleranceGlazingSystemThickness=0.2500
/// FrameToleranceGlazingSystemUcenter=0.250000
///
/// These two can be increased so that WINDOW will not complain and calculations will perform
/// with the results
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>
#include <memory>

#include <WCETarcog.hpp>

#include "thermal/commonFrames.hpp"

class TestSingleVisionWindowTRR97 : public testing::Test
{
protected:
    static std::shared_ptr<Tarcog::ISO15099::CSystem> trr97IGUUvalue()
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        constexpr auto airTemperature = 255.15;   // Kelvins
        constexpr auto airSpeed = 5.5;            // meters per second
        constexpr auto tSky = 255.15;             // Kelvins
        constexpr auto solarRadiation = 0.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////
        constexpr auto roomTemperature = 294.15;
        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);

        /////////////////////////////////////////////////////////
        // IGU
        /////////////////////////////////////////////////////////
        constexpr auto solidLayerThickness1 = 0.00475;   // [m]
        constexpr auto solidLayerConductance1 = 1.0;
        constexpr auto tir = 0.0;
        constexpr auto frontEmiss1 = 0.84;
        constexpr auto backEmiss2 = 0.042;

        auto aSolidLayer1 = Tarcog::ISO15099::Layers::solid(
          solidLayerThickness1, solidLayerConductance1, frontEmiss1, tir, backEmiss2, tir);


        constexpr auto solidLayerThickness2 = 0.00475;   // [m]
        constexpr auto solidLayerConductance2 = 1.0;

        auto aSolidLayer2 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness2, solidLayerConductance2);

        constexpr auto gapThickness = 0.0159;
        auto gapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);

        constexpr auto iguWidth = 1.0;
        constexpr auto iguHeight = 1.0;

        Tarcog::ISO15099::CIGU aIGU(iguWidth, iguHeight);
        aIGU.addLayers({aSolidLayer1, gapLayer, aSolidLayer2});

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        return std::make_shared<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
    }

    static std::shared_ptr<Tarcog::ISO15099::CSystem> trr97IGUSHGC()
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        constexpr auto airTemperature = 305.15;   // Kelvins
        constexpr auto airSpeed = 2.75;           // meters per second
        constexpr auto tSky = 305.15;             // Kelvins
        constexpr auto solarRadiation = 783.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////
        constexpr auto roomTemperature = 297.15;
        const auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);

        /////////////////////////////////////////////////////////
        // IGU
        /////////////////////////////////////////////////////////
        constexpr auto solidLayerThickness1 = 0.00475;   // [m]
        constexpr auto solidLayerConductance1 = 1.0;
        constexpr auto tir = 0.0;
        constexpr auto frontEmiss1 = 0.84;
        constexpr auto backEmiss2 = 0.042;
        constexpr auto solarAbsorptance1 = 0.302034020424;

        auto aSolidLayer1 = Tarcog::ISO15099::Layers::solid(
          solidLayerThickness1, solidLayerConductance1, frontEmiss1, tir, backEmiss2, tir);
        aSolidLayer1->setSolarAbsorptance(solarAbsorptance1);


        constexpr auto solidLayerThickness2 = 0.00475;   // [m]
        constexpr auto solidLayerConductance2 = 1.0;
        constexpr auto solarAbsorptance2 = 0.028353478760;

        auto aSolidLayer2 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness2, solidLayerConductance2);
        aSolidLayer2->setSolarAbsorptance(solarAbsorptance2);

        constexpr auto gapThickness = 0.0159;
        auto gapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);

        constexpr auto iguWidth = 1.0;
        constexpr auto iguHeight = 1.0;

        Tarcog::ISO15099::CIGU aIGU(iguWidth, iguHeight);
        aIGU.addLayers({aSolidLayer1, gapLayer, aSolidLayer2});

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        return std::make_shared<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
    }

};

TEST_F(TestSingleVisionWindowTRR97, UValueRun)
{
    constexpr auto width{1.219};
    constexpr auto height{1.219};
    constexpr auto tVis{0.707};
    constexpr auto tSol{0.3614};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, trr97IGUUvalue());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::headTRR97()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::sillTRR97()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::jambTRR97()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::jambTRR97()}});

    const double uvalue{window.uValue()};
    EXPECT_NEAR(1.952351, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.002370, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.595731, vt, 1e-6);
}

TEST_F(TestSingleVisionWindowTRR97, SHGCRun)
{
    constexpr auto width{1.219};
    constexpr auto height{1.219};
    constexpr auto tVis{0.707};
    constexpr auto tSol{0.3614};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, trr97IGUSHGC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::headTRR97()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::sillTRR97()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::jambTRR97()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::jambTRR97()}});

    const double uvalue{window.uValue()};
    EXPECT_NEAR(1.725743, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.346454, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.595731, vt, 1e-6);
}

TEST_F(TestSingleVisionWindowTRR97, UValueRunWithDividers)
{
    constexpr auto width{1.219};
    constexpr auto height{1.219};
    constexpr auto tVis{0.707};
    constexpr auto tSol{0.3614};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, trr97IGUUvalue());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::headTRR97()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::sillTRR97()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::jambTRR97()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::jambTRR97()}});

    window.setDividersAuto(Frame::sampleDivider());

    const double uvalue{window.uValue()};
    EXPECT_NEAR(1.926663, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.004549, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.517357, vt, 1e-6);
}

TEST_F(TestSingleVisionWindowTRR97, SHGCRunWithDividers)
{
    constexpr auto width{1.219};
    constexpr auto height{1.219};
    constexpr auto tVis{0.707};
    constexpr auto tSol{0.3614};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, trr97IGUSHGC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::headTRR97()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::sillTRR97()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::jambTRR97()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::jambTRR97()}});

    window.setDividersAuto(Frame::sampleDivider());

    const double uvalue{window.uValue()};
    EXPECT_NEAR(1.860804, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.304390, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.517357, vt, 1e-6);
}