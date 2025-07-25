#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"
#include "WCECommon.hpp"

class TestDoubleLowEHorizontalSliderUFactorRun : public testing::Test
{
private:
    Tarcog::ISO15099::DualVisionHorizontal m_Window;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        const auto airTemperature{255.15};   // Kelvins
        const auto airSpeed{5.5};            // meters per second
        const auto tSky{255.15};             // Kelvins
        const auto solarRadiation{0.0};

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        const auto roomTemperature{294.15};
        const auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        // IGU
        /////////////////////////////////////////////////////////
        const auto solidLayerThickness1{0.00318};   // [m]
        const auto solidLayerConductance1{1.0};
        const auto tIR1{0.0};
        const auto frontEmissivity1{0.84};
        const auto backEmissivity1{0.046578168869};

        const auto layer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness1,
                                                            solidLayerConductance1,
                                                            frontEmissivity1,
                                                            tIR1,
                                                            backEmissivity1,
                                                            tIR1);
        ASSERT_TRUE(layer1 != nullptr);

        const auto gapThickness{0.0127};
        auto gap{Tarcog::ISO15099::Layers::gap(gapThickness)};

        const auto solidLayerThickness2{0.005715};   // [m]
        const auto solidLayerConductance2{1.0};

        const auto layer2 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness2, solidLayerConductance2);

        const auto iguWidth{1.0};
        const auto iguHeight{1.0};
        Tarcog::ISO15099::CIGU aIGU(iguWidth, iguHeight);
        aIGU.addLayers({layer1, gap, layer2});

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        const auto igu{std::make_shared<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor)};

        /////////////////////////////////////////////////////////
        /// Frames
        /////////////////////////////////////////////////////////

        const double uValue{2.134059};
        const double edgeUValue{2.251039};
        const double projectedFrameDimension{0.050813};
        const double wettedLength{0.05633282};
        const double absorptance{0.3};

        const Tarcog::ISO15099::FrameData frameData{
            .UValue = uValue,
            .EdgeUValue = edgeUValue,
            .ProjectedFrameDimension = projectedFrameDimension,
            .WettedLength = wettedLength,
            .Absorptance = absorptance
        };

        const auto windowWidth{1.5};
        const auto windowHeight{1.2};
        const auto tVis{0.6385};
        const auto tSol{0.371589958668};

        m_Window = Tarcog::ISO15099::DualVisionHorizontal(
          windowWidth, windowHeight, tVis, tSol, igu, tVis, tSol, igu);

        using Tarcog::ISO15099::DualHorizontalFramePosition;

        m_Window.setFrameData({{DualHorizontalFramePosition::Left, frameData},
                               {DualHorizontalFramePosition::Right, frameData},
                               {DualHorizontalFramePosition::BottomLeft, frameData},
                               {DualHorizontalFramePosition::BottomRight, frameData},
                               {DualHorizontalFramePosition::TopLeft, frameData},
                               {DualHorizontalFramePosition::TopRight, frameData},
                               {DualHorizontalFramePosition::MeetingRail, frameData}});
    }

public:
    [[nodiscard]] Tarcog::ISO15099::DualVisionHorizontal & getWindow()
    {
        return m_Window;
    }
};

TEST_F(TestDoubleLowEHorizontalSliderUFactorRun, Test1)
{
    SCOPED_TRACE("Begin Test: Double Low-e with Horizontal Slider - U-value run");

    constexpr auto Tolerance = 1e-6;

    const auto window{getWindow()};

    const auto UValue{window.uValue()};
    EXPECT_NEAR(UValue, 1.891190, Tolerance);

    const auto SHGC{window.shgc()};
    EXPECT_NEAR(SHGC, 0.003514, Tolerance);

    const auto vt{window.vt()};
    EXPECT_NEAR(vt, 0.525034, Tolerance);
}
