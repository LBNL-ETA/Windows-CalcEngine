#include <memory>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

class TestDoubleClearIndoorShadeAir : public testing::Test
{
private:
    std::unique_ptr<Tarcog::ISO15099::CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        auto airTemperature = 255.15;   // Kelvins
        auto airSpeed = 5.5;            // meters per second
        auto tSky = 255.15;             // Kelvins
        auto solarRadiation = 0.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        auto roomTemperature = 295.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        auto solidLayerThickness = 0.005715;   // [m]
        auto solidLayerConductance = 1.0;

        auto layer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(layer1 != nullptr);

        auto layer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);

        auto shadeLayerThickness = 0.01;
        auto shadeLayerConductance = 160.0;
        auto dtop = 0.1;
        auto dbot = 0.1;
        auto dleft = 0.1;
        auto dright = 0.1;
        auto Afront = 0.2;
        EffectiveLayers::ShadeOpenness openness{Afront, dleft, dright, dtop, dbot};

        auto windowWidth = 1.0;
        auto windowHeight = 1.0;

        EffectiveLayers::EffectiveLayerOther effectiveLayer{
          windowWidth, windowHeight, shadeLayerThickness, openness};

        auto layer3 = Tarcog::ISO15099::Layers::shading(
          shadeLayerThickness, shadeLayerConductance, effectiveLayer.getEffectiveOpenness());

        ASSERT_TRUE(layer3 != nullptr);

        auto gapThickness = 0.0127;
        auto gap1 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gap1 != nullptr);

        auto gap2 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gap2 != nullptr);

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, gap1, layer2, gap2, layer3});

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_unique<Tarcog::ISO15099::CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    [[nodiscard]] Tarcog::ISO15099::CSingleSystem & GetSystem() const
    {
        return *m_TarcogSystem;
    }
};

TEST_F(TestDoubleClearIndoorShadeAir, Test1)
{
    SCOPED_TRACE("Begin Test: Indoor Shade - Air");

    constexpr auto Tolerance = 1e-6;

    const auto aSystem = GetSystem();

    const auto temperature = aSystem.getTemperatures();
    const auto radiosity = aSystem.getRadiosities();

    std::vector correctTemp = {
      258.240219, 258.756303, 276.282652, 276.798736, 288.170470, 288.174332};
    std::vector correctJ = {
      250.251349, 264.677378, 319.842529, 340.847167, 382.961347, 397.285043};

    EXPECT_EQ(correctTemp.size(), temperature.size());
    EXPECT_EQ(correctJ.size(), radiosity.size());

    for(size_t i = 0; i < temperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemp[i], temperature[i], Tolerance);
        EXPECT_NEAR(correctJ[i], radiosity[i], Tolerance);
    }

    // Removing this because it is causing different results on different machines
    // See the issue regarding to this
    //const auto numOfIter = aSystem.getNumberOfIterations();
    //EXPECT_EQ(43u, numOfIter);
    
    const auto ventilatedFlow = aSystem.getVentilationFlow(Tarcog::ISO15099::Environment::Indoor);
    EXPECT_NEAR(40.879045167167362, ventilatedFlow, Tolerance);
}
