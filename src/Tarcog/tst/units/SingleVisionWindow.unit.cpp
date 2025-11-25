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

#include "mapTesting.hpp"

class TestSingleVisionWindow : public testing::Test
{
protected:
    static std::shared_ptr<Tarcog::ISO15099::CSystem> getSingleLayerUValueBC()
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
        constexpr auto solidLayerThickness = 0.003048;   // [m]
        constexpr auto solidLayerConductance = 1.0;

        auto aSolidLayer =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);

        constexpr auto windowWidth = 2.0;
        constexpr auto windowHeight = 2.0;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayer(aSolidLayer);

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        return std::make_shared<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
    }

    static std::shared_ptr<Tarcog::ISO15099::CSystem> getSingleLayerSHGCBC()
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
        constexpr auto solidLayerThickness = 0.003048;   // [m]
        constexpr auto solidLayerConductance = 1.0;
        constexpr auto solarAbsorptance = 0.0914;

        auto aSolidLayer =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer->setSolarHeatGain(solarAbsorptance, solarRadiation);

        constexpr auto windowWidth = 2.0;
        constexpr auto windowHeight = 2.0;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayer(aSolidLayer);

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        return std::make_shared<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
    }

    static std::shared_ptr<Tarcog::ISO15099::CSystem> getDoubleLayerUValueBC()
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
        constexpr auto solidLayerThickness = 0.005715;   // [m]
        constexpr auto solidLayerConductance = 1.0;

        auto aSolidLayer1 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);

        auto aSolidLayer2 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);

        constexpr auto gapThickness = 0.012;
        auto gapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);

        constexpr auto windowWidth = 1.2;
        constexpr auto windowHeight = 1.5;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({aSolidLayer1, gapLayer, aSolidLayer2});

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        return std::make_shared<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
    }

    static std::shared_ptr<Tarcog::ISO15099::CSystem> getDoubleLayerSHGCBC()
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
        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);

        /////////////////////////////////////////////////////////
        // IGU
        /////////////////////////////////////////////////////////
        constexpr auto solidLayerThickness = 0.005715;   // [m]
        constexpr auto solidLayerConductance = 1.0;
        constexpr auto solarAbsorptance1 = 0.166707709432;
        constexpr auto solarAbsorptance2 = 0.112737670541;

        auto aSolidLayer1 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer1->setSolarHeatGain(solarAbsorptance1, solarRadiation);

        auto aSolidLayer2 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer2->setSolarHeatGain(solarAbsorptance2, solarRadiation);

        constexpr auto gapThickness = 0.012;
        auto gapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);

        constexpr auto windowWidth = 1.2;
        constexpr auto windowHeight = 1.5;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({aSolidLayer1, gapLayer, aSolidLayer2});

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        return std::make_shared<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
    }
};

TEST_F(TestSingleVisionWindow, FramesRetrieval)
{
    constexpr Tarcog::ISO15099::FrameData frameData{.UValue = 5.68,
                                                    .EdgeUValue = 5.575,
                                                    .ProjectedFrameDimension = 0.05715,
                                                    .WettedLength = 0.05715,
                                                    .Absorptance = 0.9};

    constexpr auto width{2.0};
    constexpr auto height{2.0};
    constexpr auto iguUValue{5.575};
    constexpr auto shgc{0.86};
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};
    constexpr auto hout{20.42635};

    auto window = Tarcog::ISO15099::WindowSingleVision(
      width,
      height,
      tVis,
      tSol,
      std::make_shared<Tarcog::ISO15099::SimpleIGU>(iguUValue, shgc, hout));

    window.setFrameData({{Tarcog::ISO15099::SingleVisionFramePosition::Top, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Left, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Right, frameData}});

    using Tarcog::ISO15099::SingleVisionFramePosition;

    /// Testing frame areas

    std::map<SingleVisionFramePosition, double> areas;
    for(const auto & [framePosition, frame] : window.frames())
    {
        areas.emplace(framePosition, Tarcog::ISO15099::frameArea(frame));
    }

    std::map<SingleVisionFramePosition, double> correctAreas{
      {SingleVisionFramePosition::Top, 0.1110338775},
      {SingleVisionFramePosition::Bottom, 0.1110338775},
      {SingleVisionFramePosition::Left, 0.1110338775},
      {SingleVisionFramePosition::Right, 0.1110338775}};

    Helper::testMaps("Single vision frame areas", correctAreas, areas, 1e-6);

    /// Testing edge areas

    std::map<SingleVisionFramePosition, double> edgeAreas;
    for(const auto & [framePosition, frame] : window.frames())
    {
        edgeAreas.emplace(framePosition, Tarcog::ISO15099::edgeOfGlassArea(frame));
    }

    std::map<SingleVisionFramePosition, double> correctEdgeAreas{
      {SingleVisionFramePosition::Top, 0.1157097},
      {SingleVisionFramePosition::Bottom, 0.1157097},
      {SingleVisionFramePosition::Left, 0.1157097},
      {SingleVisionFramePosition::Right, 0.1157097}};

    Helper::testMaps("Single vision frame areas", correctEdgeAreas, edgeAreas, 1e-6);
}

TEST_F(TestSingleVisionWindow, PredefinedCOGValues)
{
    SCOPED_TRACE("Begin Test: Single vision window with predefined COG values.");


    constexpr Tarcog::ISO15099::FrameData frameData{.UValue = 5.68,
                                                    .EdgeUValue = 5.575,
                                                    .ProjectedFrameDimension = 0.05715,
                                                    .WettedLength = 0.05715,
                                                    .Absorptance = 0.9};

    constexpr auto width{2.0};
    constexpr auto height{2.0};
    constexpr auto iguUValue{5.575};
    constexpr auto shgc{0.86};
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};
    constexpr auto hout{20.42635};

    auto window = Tarcog::ISO15099::WindowSingleVision(
      width,
      height,
      tVis,
      tSol,
      std::make_shared<Tarcog::ISO15099::SimpleIGU>(iguUValue, shgc, hout));

    window.setFrameData({{Tarcog::ISO15099::SingleVisionFramePosition::Top, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Left, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Right, frameData}});


    const double vt{window.vt()};
    EXPECT_NEAR(0.799180, vt, 1e-6);

    const double uvalue{window.uValue()};
    EXPECT_NEAR(5.586659, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.792299, windowSHGC, 1e-6);

    const double totalFrameArea{window.getFrameArea()};
    EXPECT_NEAR(0.444136, totalFrameArea, 1e-6);
}

TEST_F(TestSingleVisionWindow, PredefinedCOGWithDividersValues)
{
    SCOPED_TRACE("Begin Test: Single vision window with predefined COG values.");

    constexpr Tarcog::ISO15099::FrameData frameData{.UValue = 5.68,
                                                    .EdgeUValue = 5.575,
                                                    .ProjectedFrameDimension = 0.05715,
                                                    .WettedLength = 0.05715,
                                                    .Absorptance = 0.9};

    constexpr auto width{2.0};
    constexpr auto height{2.0};
    constexpr auto iguUValue{5.575};
    constexpr auto shgc{0.86};
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};
    constexpr auto hout{20.42635};

    auto window = Tarcog::ISO15099::WindowSingleVision(
      width,
      height,
      tVis,
      tSol,
      std::make_shared<Tarcog::ISO15099::SimpleIGU>(iguUValue, shgc, hout));

    window.setFrameData({{Tarcog::ISO15099::SingleVisionFramePosition::Top, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Left, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Right, frameData}});


    constexpr size_t nHorizontal{2};
    constexpr size_t nVertical{2};
    window.setDividers(frameData, nHorizontal, nVertical);


    const double vt{window.vt()};
    EXPECT_NEAR(0.705234, vt, 1e-6);

    const double uvalue{window.uValue()};
    EXPECT_NEAR(5.597631, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.728580, windowSHGC, 1e-6);
}

// cpp
TEST_F(TestSingleVisionWindow, SingleLayer)
{
    SCOPED_TRACE("Uvalue environmental conditions (single layer Sample-sill, sample-head and "
                 "sample-jamb) - thermal numbers.");

    constexpr auto width{1.2};
    constexpr auto height{1.5};
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerUValueBC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::sampleHead()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::sampleSill()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::sampleJamb()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::sampleJamb()}});

    const double uvalue{window.uValue()};
    EXPECT_NEAR(4.676005, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.002301, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.787038, vt, 1e-6);
}

TEST_F(TestSingleVisionWindow, FrameAreas)
{
    SCOPED_TRACE("Uvalue environmental conditions (single layer Sample-sill, sample-head and "
                 "sample-jamb) - frame areas.");

    constexpr auto width{1.2};
    constexpr auto height{1.5};
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerUValueBC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::sampleHead()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::sampleSill()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::sampleJamb()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::sampleJamb()}});

    // Validated vs WINDOW 7.8.80. Values can be read from FrameList table in the database

    const double headFrameArea{
      window.getFrameArea(Tarcog::ISO15099::SingleVisionFramePosition::Top)};
    EXPECT_NEAR(0.049612, headFrameArea, 1e-6);

    const double leftJambFrameArea{
      window.getFrameArea(Tarcog::ISO15099::SingleVisionFramePosition::Left)};
    EXPECT_NEAR(0.062474, leftJambFrameArea, 1e-6);

    const double rightJambFrameArea{
      window.getFrameArea(Tarcog::ISO15099::SingleVisionFramePosition::Right)};
    EXPECT_NEAR(0.062474, rightJambFrameArea, 1e-6);

    const double sillFrameArea{
      window.getFrameArea(Tarcog::ISO15099::SingleVisionFramePosition::Bottom)};
    EXPECT_NEAR(0.049612, sillFrameArea, 1e-6);

    const double totalFrameArea{window.getFrameArea()};
    EXPECT_NEAR(0.224173, totalFrameArea, 1e-6);
}

TEST_F(TestSingleVisionWindow, FrameEdgeOfGlassAreas)
{
    SCOPED_TRACE("Uvalue environmental conditions (single layer Sample-sill, sample-head and "
                 "sample-jamb) - edge of glass areas.");

    constexpr auto width{1.2};
    constexpr auto height{1.5};
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerUValueBC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::sampleHead()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::sampleSill()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::sampleJamb()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::sampleJamb()}});

    // Validated vs WINDOW 7.8.80. Values can be read from FrameList table in the database

    const double headEdgeOfGlassFrameArea{
      window.getFrameEdgeOfGlassArea(Tarcog::ISO15099::SingleVisionFramePosition::Top)};
    EXPECT_NEAR(0.066723, headEdgeOfGlassFrameArea, 1e-6);

    const double leftEdgeOfGlassJambFrameArea{
      window.getFrameEdgeOfGlassArea(Tarcog::ISO15099::SingleVisionFramePosition::Left)};
    EXPECT_NEAR(0.085773, leftEdgeOfGlassJambFrameArea, 1e-6);

    const double rightEdgeOfGlassJambFrameArea{
      window.getFrameEdgeOfGlassArea(Tarcog::ISO15099::SingleVisionFramePosition::Right)};
    EXPECT_NEAR(0.085773, rightEdgeOfGlassJambFrameArea, 1e-6);

    const double sillEdgeOfGlassFrameArea{
      window.getFrameEdgeOfGlassArea(Tarcog::ISO15099::SingleVisionFramePosition::Bottom)};
    EXPECT_NEAR(0.066723, sillEdgeOfGlassFrameArea, 1e-6);

    const double totalEdgeOfGlassFrameArea{window.getFrameEdgeOfGlassArea()};
    EXPECT_NEAR(0.304990, totalEdgeOfGlassFrameArea, 1e-6);
}

TEST_F(TestSingleVisionWindow, CalculatedSingleLayerSHGC)
{
    SCOPED_TRACE(
      "SHGC environmental conditions (single layer Sample-sill, sample-head and sample-jamb).");

    constexpr auto width{1.2};
    constexpr auto height{1.5};

    // Optical data correspond to NFRC 102 sample
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerSHGCBC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::sampleHead()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::sampleSill()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::sampleJamb()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::sampleJamb()}});


    // Values in this test are validated against WINDOW 7.8.80.
    // If this test fails make sure that changes are expected to do so.

    const double uvalue{window.uValue()};
    EXPECT_NEAR(4.330241, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.756163, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.787038, vt, 1e-6);
}

TEST_F(TestSingleVisionWindow, GenericFramesSingleLayerUValue)
{
    SCOPED_TRACE("Generic frames with single clear (Winter run).");

    constexpr auto width{1.2};
    constexpr auto height{1.5};

    // Optical data correspond to NFRC 102 sample
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerUValueBC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::genericFrameClass1()}});

    // Values in this test are validated against WINDOW 7.8.80.
    // If this test fails make sure that changes are expected to do so.

    const double uvalue{window.uValue()};
    EXPECT_NEAR(5.704964, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.028638, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.751391, vt, 1e-6);
}

TEST_F(TestSingleVisionWindow, GenericFramesSingleLayerSHGC)
{
    SCOPED_TRACE("Generic frames with single clear (Summer run).");

    constexpr auto width{1.2};
    constexpr auto height{1.5};

    // Optical data correspond to NFRC 102 sample
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerSHGCBC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::genericFrameClass1()}});

    // Values in this test are validated against WINDOW 7.8.80.
    // If this test fails make sure that changes are expected to do so.

    const double uvalue{window.uValue()};
    EXPECT_NEAR(5.295638, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.759857, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.751391, vt, 1e-6);
}

TEST_F(TestSingleVisionWindow, GenericFramesSingleLayerUValueWithDividers)
{
    SCOPED_TRACE("Generic frames with single clear with dividers (Winter run).");

    constexpr auto width{1.2};
    constexpr auto height{1.5};

    // Optical data correspond to NFRC 102 sample
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerUValueBC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::genericFrameClass1()}});

    constexpr size_t nHorizontal{2};
    constexpr size_t nVertical{2};
    window.setDividers(Frame::genericDividerAluminumHollow(), nHorizontal, nVertical);

    // In this case WINDOW gives an error message:
    // LBL Dividers and Correlations not valid for single glazing

    const double uvalue{window.uValue()};
    EXPECT_NEAR(5.689011, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.036176, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.712693, vt, 1e-6);
}

TEST_F(TestSingleVisionWindow, GenericFramesSingleLayerSHGCWithDividers)
{
    SCOPED_TRACE("Generic frames with single clear (Summer run).");

    constexpr auto width{1.2};
    constexpr auto height{1.5};

    // Optical data correspond to NFRC 102 sample
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerSHGCBC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::genericFrameClass1()}});

    constexpr size_t nHorizontal{2};
    constexpr size_t nVertical{2};
    window.setDividers(Frame::genericDividerAluminumHollow(), nHorizontal, nVertical);

    // In this case WINDOW gives an error message:
    // LBL Dividers and Correlations not valid for single glazing

    const double uvalue{window.uValue()};
    EXPECT_NEAR(5.323107, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.733646, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.712693, vt, 1e-6);
}

TEST_F(TestSingleVisionWindow, CalculatedDoubleLayerUValue)
{
    SCOPED_TRACE("Begin Test: Single vision window with calculated COG values.");

    constexpr auto width{1.2};
    constexpr auto height{1.5};

    // These values correspond to double clear (NFRC-103; NFRC-103)
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getDoubleLayerUValueBC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::sampleHead()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::sampleSill()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::sampleJamb()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::sampleJamb()}});

    // Values in this test are validated against WINDOW 7.8.80.
    // If this test fails make sure that changes are expected to do so.

    const double uvalue{window.uValue()};
    EXPECT_NEAR(2.525314, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.002307, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.688199, vt, 1e-6);
}

TEST_F(TestSingleVisionWindow, CalculatedDoubleLayerSHGC)
{
    constexpr auto width{1.2};
    constexpr auto height{1.5};

    // These values correspond to double clear (NFRC-103; NFRC-103)
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getDoubleLayerSHGCBC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::sampleHead()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::sampleSill()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::sampleJamb()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::sampleJamb()}});

    // Values in this test are validated against WINDOW 7.8.80.
    // If this test fails make sure that changes are expected to do so.

    const double uvalue{window.uValue()};
    // This value should be read from the database (UvalSummer) and not from the screen
    EXPECT_NEAR(2.631899, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.618737, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.688199, vt, 1e-6);
}

TEST_F(TestSingleVisionWindow, GenericFramesDoubleLayerUValue)
{
    SCOPED_TRACE("Begin Test: Double clear window with winter boundary conditions.");

    constexpr auto width{1.2};
    constexpr auto height{1.5};

    // These values correspond to double clear (NFRC-103; NFRC-103)
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getDoubleLayerUValueBC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::genericFrameClass1()}});

    // Values in this test are validated against WINDOW 7.8.80.
    // If this test fails make sure that changes are expected to do so.

    const double uvalue{window.uValue()};
    EXPECT_NEAR(3.261272, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.028714, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.657029, vt, 1e-6);
}

TEST_F(TestSingleVisionWindow, GenericFramesDoubleLayerSHGC)
{
    SCOPED_TRACE("Begin Test: Single vision window with calculated COG values.");

    constexpr auto width{1.2};
    constexpr auto height{1.5};

    // These values correspond to double clear (NFRC-103; NFRC-103)
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getDoubleLayerSHGCBC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::genericFrameClass1()}});

    // Values in this test are validated against WINDOW 7.8.80.
    // If this test fails make sure that changes are expected to do so.

    const double uvalue{window.uValue()};
    // This value should be read from the database (UvalSummer) and not from the screen
    EXPECT_NEAR(3.381108, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.628385, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.657029, vt, 1e-6);
}

TEST_F(TestSingleVisionWindow, GenericFramesDoubleLayerUValueWithFixedDividers)
{
    constexpr auto width{1.2};
    constexpr auto height{1.5};

    // These values correspond to double clear (NFRC-103; NFRC-103)
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getDoubleLayerUValueBC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::genericFrameClass1()}});

    constexpr size_t nHorizontal{2U};
    constexpr size_t nVertical{2U};
    window.setDividers(Frame::genericDividerAluminumHollow(), nHorizontal, nVertical);

    // Numbers when window include dividers will not match with WINDOW because results
    // in WINDOW are incorrect. Still would like to see additional validation on this. (Simon)

    const double uvalue{window.uValue()};
    EXPECT_NEAR(3.530437, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.036204, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.623190, vt, 1e-6);
}

TEST_F(TestSingleVisionWindow, GenericFramesDoubleLayerSHGCWithFixedDividers)
{
    constexpr auto width{1.2};
    constexpr auto height{1.5};

    // These values correspond to double clear (NFRC-103; NFRC-103)
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getDoubleLayerSHGCBC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::genericFrameClass1()}});

    constexpr size_t nHorizontal{2U};
    constexpr size_t nVertical{2U};

    window.setDividers(Frame::genericDividerAluminumHollow(), nHorizontal, nVertical);

    // Numbers when window include dividers will not match with WINDOW because results
    // in WINDOW are incorrect. Still would like to see additional validation on this. (Simon)
    const double uvalue{window.uValue()};
    EXPECT_NEAR(3.635180, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.608772, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.623190, vt, 1e-6);
}

TEST_F(TestSingleVisionWindow, GenericFramesDoubleLayerUValueWithAutoDividers)
{
    constexpr auto width{1.2};
    constexpr auto height{1.5};

    // These values correspond to double clear (NFRC-103; NFRC-103)
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getDoubleLayerUValueBC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::genericFrameClass1()}});

    window.setDividersAuto(Frame::genericDividerAluminumHollow());

    // Numbers when window include dividers will not match with WINDOW because results
    // in WINDOW are incorrect. Still would like to see additional validation on this. (Simon)

    const double uvalue{window.uValue()};
    EXPECT_NEAR(3.709885, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.041930, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.597322, vt, 1e-6);
}

TEST_F(TestSingleVisionWindow, GenericFramesDoubleLayerSHGCWithAutoDividers)
{
    constexpr auto width{1.2};
    constexpr auto height{1.5};

    // These values correspond to double clear (NFRC-103; NFRC-103)
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getDoubleLayerSHGCBC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, Frame::genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, Frame::genericFrameClass1()}});

    window.setDividersAuto(Frame::genericDividerAluminumHollow());

    const double uvalue{window.uValue()};
    EXPECT_NEAR(3.804730, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.593779, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.597322, vt, 1e-6);
}

TEST_F(TestSingleVisionWindow, IGUMismatchDetected)
{
    SCOPED_TRACE("Begin Test: Mismatch detection for single vision window.");

    // Frame expected IGU values
    constexpr double designIGUFrameUValue{5.575};
    constexpr double designIGUFrameThickness{0.006};   // deliberately smaller than real thickness

    // Add spec to frame data
    Tarcog::ISO15099::FrameData frameData{.UValue = 5.68,
                                          .EdgeUValue = 5.575,
                                          .ProjectedFrameDimension = 0.05715,
                                          .WettedLength = 0.05715,
                                          .Absorptance = 0.9};

    frameData.iguData = Tarcog::ISO15099::IGUData{designIGUFrameUValue, designIGUFrameThickness};

    // IGU under test (from getCOG) has different uValue and thickness
    constexpr auto width{2.0};
    constexpr auto height{2.0};
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerSHGCBC());

    // Set tight tolerance to force mismatch
    constexpr double tightTolerance{0.0001};   // very tight to guarantee mismatch
    window.setUValueIGUTolerance(tightTolerance);
    window.setThicknessIGUTolerance(tightTolerance);

    window.setFrameData({{Tarcog::ISO15099::SingleVisionFramePosition::Top, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Left, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Right, frameData}});

    // We have to use geometrical thickness and not IGU thickness because that would represent
    // thermal thickness
    constexpr double iguThickness{0.005};

    const auto mismatch = window.iguMismatch(iguThickness);
    EXPECT_TRUE(mismatch.any());
    EXPECT_TRUE(mismatch.uCenterMissmatch);
    EXPECT_TRUE(mismatch.thicknessMissmatch);
}
