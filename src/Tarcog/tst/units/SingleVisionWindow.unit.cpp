#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>

#include "WCETarcog.hpp"

class TestSingleVisionWindow : public testing::Test
{
protected:
    void SetUp() override
    {}

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
        auto roomTemperature = 294.15;
        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);

        /////////////////////////////////////////////////////////
        // IGU
        /////////////////////////////////////////////////////////
        auto solidLayerThickness = 0.003048;   // [m]
        auto solidLayerConductance = 1.0;

        auto aSolidLayer =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);

        auto windowWidth = 2.0;
        auto windowHeight = 2.0;
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
        auto airTemperature = 305.15;   // Kelvins
        auto airSpeed = 2.75;           // meters per second
        auto tSky = 305.15;             // Kelvins
        auto solarRadiation = 783.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////
        auto roomTemperature = 297.15;
        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);

        /////////////////////////////////////////////////////////
        // IGU
        /////////////////////////////////////////////////////////
        auto solidLayerThickness = 0.003048;   // [m]
        auto solidLayerConductance = 1.0;

        auto aSolidLayer =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer->setSolarHeatGain(0.0914, solarRadiation);

        auto windowWidth = 2.0;
        auto windowHeight = 2.0;
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
        auto roomTemperature = 294.15;
        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);

        /////////////////////////////////////////////////////////
        // IGU
        /////////////////////////////////////////////////////////
        auto solidLayerThickness = 0.003048;   // [m]
        auto solidLayerConductance = 1.0;

        auto aSolidLayer1 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);

        auto aSolidLayer2 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);

        constexpr auto gapThickness = 0.012;
        auto gapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);

        auto windowWidth = 1.2;
        auto windowHeight = 1.5;
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

    constexpr Tarcog::ISO15099::FrameData frameData{.Class = std::nullopt,
                                                    .UValue = uValue,
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
    SCOPED_TRACE("Uvalue environmental conditions (single layer).");

    constexpr double uValue{5.68};
    constexpr double edgeUValue{5.575};
    constexpr double projectedFrameDimension{0.05715};
    constexpr double wettedLength{0.05715};
    constexpr double absorptance{0.9};

    constexpr Tarcog::ISO15099::FrameData frameData{.Class = std::nullopt,
                                                    .UValue = uValue,
                                                    .EdgeUValue = edgeUValue,
                                                    .ProjectedFrameDimension =
                                                      projectedFrameDimension,
                                                    .WettedLength = wettedLength,
                                                    .Absorptance = absorptance};

    constexpr auto width{1.2};
    constexpr auto height{1.5};
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerUValue());

    window.setFrameData({{Tarcog::ISO15099::SingleVisionFramePosition::Top, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Left, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Right, frameData}});


    const double vt{window.vt()};
    EXPECT_NEAR(0.751391, vt, 1e-6);

    const double uvalue{window.uValue()};
    EXPECT_NEAR(5.682655, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.028638, windowSHGC, 1e-6);
}

TEST_F(TestSingleVisionWindow, CalculatedSingleLayerSHGC)
{
    SCOPED_TRACE("SHGC environmental conditions (single layer).");

    constexpr double uValue{5.68};
    constexpr double edgeUValue{5.575};
    constexpr double projectedFrameDimension{0.05715};
    constexpr double wettedLength{0.05715};
    constexpr double absorptance{0.9};

    constexpr Tarcog::ISO15099::FrameData frameData{.Class = std::nullopt,
                                                    .UValue = uValue,
                                                    .EdgeUValue = edgeUValue,
                                                    .ProjectedFrameDimension =
                                                      projectedFrameDimension,
                                                    .WettedLength = wettedLength,
                                                    .Absorptance = absorptance};

    constexpr auto width{1.2};
    constexpr auto height{1.5};
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerSHGC());

    window.setFrameData({{Tarcog::ISO15099::SingleVisionFramePosition::Top, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Left, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Right, frameData}});


    const double vt{window.vt()};
    EXPECT_NEAR(0.751391, vt, 1e-6);

    const double uvalue{window.uValue()};
    EXPECT_NEAR(5.354337, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.759857, windowSHGC, 1e-6);
}

TEST_F(TestSingleVisionWindow, GenericFramesSingleLayerUValue)
{
    SCOPED_TRACE("Begin Test: Single vision window with calculated COG values.");

    constexpr double projectedFrameDimension{0.05715};

    //! This is class 1 generic frame (as defined in WINDOW database)
    constexpr Tarcog::ISO15099::FrameData frameData{.Class = {{2.33, -0.01, 0.138, 0, 0}},
                                                    .UValue = 5.68,
                                                    .EdgeUValue = 0.0,
                                                    .ProjectedFrameDimension =
                                                      projectedFrameDimension,
                                                    .WettedLength = projectedFrameDimension,
                                                    .Absorptance = 0.9};

    constexpr auto width{1.2};
    constexpr auto height{1.5};
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerUValue());

    window.setFrameData({{Tarcog::ISO15099::SingleVisionFramePosition::Top, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Left, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Right, frameData}});


    const double vt{window.vt()};
    EXPECT_NEAR(0.751391, vt, 1e-6);

    const double uvalue{window.uValue()};
    EXPECT_NEAR(5.704964, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.028638, windowSHGC, 1e-6);
}

TEST_F(TestSingleVisionWindow, GenericFramesSingleLayerSHGC)
{
    SCOPED_TRACE("Begin Test: Single vision window with calculated COG values.");

    constexpr double projectedFrameDimension{0.05715};

    //! This is class 1 generic frame (as defined in WINDOW database)
    constexpr Tarcog::ISO15099::FrameData frameData{.Class = {{2.33, -0.01, 0.138, 0, 0}},
                                                    .UValue = 5.68,
                                                    .EdgeUValue = 0.0,
                                                    .ProjectedFrameDimension =
                                                      projectedFrameDimension,
                                                    .WettedLength = projectedFrameDimension,
                                                    .Absorptance = 0.9};

    constexpr auto width{1.2};
    constexpr auto height{1.5};
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerSHGC());

    window.setFrameData({{Tarcog::ISO15099::SingleVisionFramePosition::Top, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Left, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Right, frameData}});


    const double vt{window.vt()};
    EXPECT_NEAR(0.751391, vt, 1e-6);

    const double uvalue{window.uValue()};
    EXPECT_NEAR(5.295638, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.759857, windowSHGC, 1e-6);
}

TEST_F(TestSingleVisionWindow, GenericFramesDoubleLayerUValue)
{
    SCOPED_TRACE("Begin Test: Single vision window with calculated COG values.");

    constexpr double projectedFrameDimension{0.05715};

    //! This is class 1 generic frame (as defined in WINDOW database)
    constexpr Tarcog::ISO15099::FrameData frameData{.Class = {{2.33, -0.01, 0.138, 0, 0}},
                                                    .UValue = 5.68,
                                                    .EdgeUValue = 0.0,
                                                    .ProjectedFrameDimension =
                                                      projectedFrameDimension,
                                                    .WettedLength = projectedFrameDimension,
                                                    .Absorptance = 0.9};

    constexpr auto width{1.2};
    constexpr auto height{1.5};
    constexpr auto tVis{0.7861};
    constexpr auto tSol{0.6069};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getDoubleLayerUValue());

    window.setFrameData({{Tarcog::ISO15099::SingleVisionFramePosition::Top, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Left, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Right, frameData}});


    const double vt{window.vt()};
    EXPECT_NEAR(0.657029, vt, 1e-6);

    const double uvalue{window.uValue()};
    EXPECT_NEAR(3.292457, uvalue, 1e-6);

    const double windowSHGC{window.shgc()};
    EXPECT_NEAR(0.028714, windowSHGC, 1e-6);
}

TEST_F(TestSingleVisionWindow, IGUMismatchDetected)
{
    SCOPED_TRACE("Begin Test: Mismatch detection for single vision window.");

    // Frame expected IGU values
    constexpr double designUValue{5.575};
    constexpr double designThickness{0.006};   // deliberately smaller than real thickness
    constexpr double tightTolerance{0.001};    // very tight to guarantee mismatch

    // Frame physical parameters
    constexpr double frameUValue{5.68};
    constexpr double edgeUValue{5.575};
    constexpr double projectedFrameDimension{0.05715};
    constexpr double wettedLength{0.05715};
    constexpr double absorptance{0.9};

    // Add spec to frame data
    Tarcog::ISO15099::FrameData frameData{.Class = std::nullopt,
                                          .UValue = frameUValue,
                                          .EdgeUValue = edgeUValue,
                                          .ProjectedFrameDimension = projectedFrameDimension,
                                          .WettedLength = wettedLength,
                                          .Absorptance = absorptance};
    frameData.iguData = Tarcog::ISO15099::IGUData{designUValue, designThickness};

    // IGU under test (from getCOG) has different uValue and thickness
    constexpr auto width{2.0};
    constexpr auto height{2.0};
    constexpr auto tVis{0.899};
    constexpr auto tSol{0.8338};

    auto window =
      Tarcog::ISO15099::WindowSingleVision(width, height, tVis, tSol, getSingleLayerSHGC());

    // Set tight tolerance to force mismatch
    window.setUValueIGUTolerance(tightTolerance);
    window.setThicknessIGUTolerance(tightTolerance);

    window.setFrameData({{Tarcog::ISO15099::SingleVisionFramePosition::Top, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Bottom, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Left, frameData},
                         {Tarcog::ISO15099::SingleVisionFramePosition::Right, frameData}});

    const auto mismatch = window.iguMissmatch();
    EXPECT_TRUE(mismatch.any());
    EXPECT_TRUE(mismatch.uCenterMissmatch);
    EXPECT_TRUE(mismatch.thicknessMissmatch);
}
