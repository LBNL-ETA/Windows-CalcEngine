#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>

#include "WCETarcog.hpp"

class TestSingleVisionWindow : public testing::Test
{
protected:
    void SetUp() override
    {}

    // NOLINTBEGIN

    //! Data are picked up from WINDOW database frame table (imported from THMZ file)
    static Tarcog::ISO15099::FrameData sampleSill()
    {
        return {.UValue = 2.017913,
                .EdgeUValue = 2.339592,
                .ProjectedFrameDimension = 0.04287518,
                .WettedLength = 0.05633283,
                .Absorptance = 0.3};
    }

    static Tarcog::ISO15099::FrameData sampleHead()
    {
        return {.UValue = 2.024318,
                .EdgeUValue = 2.34355,
                .ProjectedFrameDimension = 0.04287518,
                .WettedLength = 0.05674332,
                .Absorptance = 0.3};
    }

    static Tarcog::ISO15099::FrameData sampleJamb()
    {
        return {.UValue = 1.943306,
                .EdgeUValue = 2.358972,
                .ProjectedFrameDimension = 0.04287518,
                .WettedLength = 0.04122826,
                .Absorptance = 0.3};
    }

    static Tarcog::ISO15099::FrameData genericFrameClass1()
    {
        constexpr double projectedFrameDimension{0.05715};

        //! This is class 1 generic frame (as defined in WINDOW database)
        return {.UValue = 5.68,
                .EdgeUValue = 0.0,
                .ProjectedFrameDimension = projectedFrameDimension,
                .WettedLength = projectedFrameDimension,
                .Absorptance = 0.9,
                .Class = Tarcog::ISO15099::GenericFrame{{2.33, -0.01, 0.138, 0, 0}}};
    }

    static Tarcog::ISO15099::FrameData genericDividerAluminumHollow()
    {
        constexpr double projectedFrameDimension{0.01588};

        // Data pulled from WINDOW database, record 1
        return {.UValue = 0.18,
                .EdgeUValue = 3.0,
                .ProjectedFrameDimension = projectedFrameDimension,
                .WettedLength = projectedFrameDimension,
                .Absorptance = 0.9,
                .Class = Tarcog::ISO15099::GenericDivider{
                    .EdgePoly = {1.19, 0.0, 0.73, 0.009},
                    .BodyPoly = {5.56, 0.0004, -0.00003, 0.042, -0.003}}};
    }

    // NOLINTEND

    static std::shared_ptr<Tarcog::ISO15099::CSystem> getSingleLayerUValue()
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

    static std::shared_ptr<Tarcog::ISO15099::CSystem> getSingleLayerSHGC()
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

    static std::shared_ptr<Tarcog::ISO15099::CSystem> getDoubleLayerUValue()
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

    static std::shared_ptr<Tarcog::ISO15099::CSystem> getDoubleLayerSHGC()
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

TEST_F(TestSingleVisionWindow, PredefinedCOGValues)
{
    SCOPED_TRACE("Begin Test: Single vision window with predefined COG values.");

    constexpr double uValue{5.68};
    constexpr double edgeUValue{5.575};
    constexpr double projectedFrameDimension{0.05715};
    constexpr double wettedLength{0.05715};
    constexpr double absorptance{0.9};

    constexpr Tarcog::ISO15099::FrameData frameData{.UValue = uValue,
                                                    .EdgeUValue = edgeUValue,
                                                    .ProjectedFrameDimension =
                                                      projectedFrameDimension,
                                                    .WettedLength = wettedLength,
                                                    .Absorptance = absorptance};

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
}

TEST_F(TestSingleVisionWindow, CalculatedSingleLayerUValue)
{
    SCOPED_TRACE(
      "Uvalue environmental conditions (single layer Sample-sill, sample-head and sample-jamb).");

    constexpr auto width{1.2};
    constexpr auto height{1.5};
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerUValue());

    window.setFrameData({{Tarcog::ISO15099::SingleVisionFramePosition::Top, sampleHead()},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, sampleSill()},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Left, sampleJamb()},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Right, sampleJamb()}});


    const double uvalue{window.uValue()};
    EXPECT_NEAR(4.676005, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.002301, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.787038, vt, 1e-6);
}

TEST_F(TestSingleVisionWindow, CalculatedSingleLayerSHGC)
{
    SCOPED_TRACE(
      "SHGC environmental conditions (single layer Sample-sill, sample-head and sample-jamb).");

    constexpr auto width{1.2};
    constexpr auto height{1.5};
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerSHGC());

    window.setFrameData({{Tarcog::ISO15099::SingleVisionFramePosition::Top, sampleHead()},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, sampleSill()},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Left, sampleJamb()},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Right, sampleJamb()}});


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
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerUValue());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, genericFrameClass1()}});

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
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerSHGC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, genericFrameClass1()}});

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
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerUValue());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, genericFrameClass1()}});

    constexpr size_t nHorizontal{2};
    constexpr size_t nVertical{2};
    window.setDividers(genericDividerAluminumHollow(), nHorizontal, nVertical);

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
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerSHGC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, genericFrameClass1()}});

    constexpr size_t nHorizontal{2};
    constexpr size_t nVertical{2};
    window.setDividers(genericDividerAluminumHollow(), nHorizontal, nVertical);

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
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getDoubleLayerUValue());

    window.setFrameData({{Tarcog::ISO15099::SingleVisionFramePosition::Top, sampleHead()},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, sampleSill()},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Left, sampleJamb()},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Right, sampleJamb()}});

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
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getDoubleLayerSHGC());

    window.setFrameData({{Tarcog::ISO15099::SingleVisionFramePosition::Top, sampleHead()},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, sampleSill()},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Left, sampleJamb()},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Right, sampleJamb()}});

    const double uvalue{window.uValue()};
    EXPECT_NEAR(2.631899, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.618737, windowSHGC, 1e-6);

    const double vt{window.vt()};
    EXPECT_NEAR(0.688199, vt, 1e-6);
}

TEST_F(TestSingleVisionWindow, GenericFramesDoubleLayerUValue)
{
    SCOPED_TRACE("Begin Test: Single vision window with calculated COG values.");

    constexpr auto width{1.2};
    constexpr auto height{1.5};
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getDoubleLayerUValue());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, genericFrameClass1()}});

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
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getDoubleLayerSHGC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, genericFrameClass1()}});

    const double uvalue{window.uValue()};
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
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getDoubleLayerUValue());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, genericFrameClass1()}});

    constexpr size_t nHorizontal{2u};
    constexpr size_t nVertical{2u};
    window.setDividers(genericDividerAluminumHollow(), nHorizontal, nVertical);

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
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getDoubleLayerSHGC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, genericFrameClass1()}});

    constexpr size_t nHorizontal{2u};
    constexpr size_t nVertical{2u};
    window.setDividers(genericDividerAluminumHollow(), nHorizontal, nVertical);

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
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getDoubleLayerUValue());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, genericFrameClass1()}});

    window.setDividersAuto(genericDividerAluminumHollow());

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
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getDoubleLayerSHGC());

    window.setFrameData(
      {{Tarcog::ISO15099::SingleVisionFramePosition::Top, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Left, genericFrameClass1()},
       {Tarcog::ISO15099::SingleVisionFramePosition::Right, genericFrameClass1()}});

    window.setDividersAuto(genericDividerAluminumHollow());

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

    // Frame physical parameters
    constexpr double frameUValue{5.68};
    constexpr double edgeUValue{5.575};
    constexpr double projectedFrameDimension{0.05715};
    constexpr double wettedLength{0.05715};
    constexpr double absorptance{0.9};

    // Frame expected IGU values
    constexpr double designIGUFrameUValue{5.575};
    constexpr double designIGUFrameThickness{0.006};   // deliberately smaller than real thickness

    // Add spec to frame data
    Tarcog::ISO15099::FrameData frameData{.UValue = frameUValue,
                                          .EdgeUValue = edgeUValue,
                                          .ProjectedFrameDimension = projectedFrameDimension,
                                          .WettedLength = wettedLength,
                                          .Absorptance = absorptance};
    frameData.iguData = Tarcog::ISO15099::IGUData{designIGUFrameUValue, designIGUFrameThickness};

    // IGU under test (from getCOG) has different uValue and thickness
    constexpr auto width{2.0};
    constexpr auto height{2.0};
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerSHGC());

    // Set tight tolerance to force mismatch
    constexpr double tightTolerance{0.0001};    // very tight to guarantee mismatch
    window.setUValueIGUTolerance(tightTolerance);
    window.setThicknessIGUTolerance(tightTolerance);

    window.setFrameData({{Tarcog::ISO15099::SingleVisionFramePosition::Top, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Left, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Right, frameData}});

    // We have to use geometrical thickness and not IGU thickness because that would represent
    // thermal thickness
    constexpr double iguThickness{0.005};

    const auto mismatch = window.iguMissmatch(iguThickness);
    EXPECT_TRUE(mismatch.any());
    EXPECT_TRUE(mismatch.uCenterMissmatch);
    EXPECT_TRUE(mismatch.thicknessMissmatch);
}
