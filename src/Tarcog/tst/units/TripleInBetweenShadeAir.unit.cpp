#include <memory>
#include <gtest/gtest.h>

#include <WCEGases.hpp>
#include <WCETarcog.hpp>

#include "vectorTesting.hpp"

class TestInBetweenShadeAir : public testing::Test
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

        auto aLayer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(aLayer1 != nullptr);

        auto aLayer3 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(aLayer3 != nullptr);

        auto shadeLayerThickness = 0.01;
        auto shadeLayerConductance = 160.0;
        auto Atop = 0.1;
        auto Abot = 0.1;
        auto Aleft = 0.1;
        auto Aright = 0.1;
        auto PermeabilityFactor = 0.2;

        EffectiveLayers::ShadeOpenness openness{Aleft, Aright, Atop, Abot};

        const auto effectiveLayer{
          EffectiveLayers::makeCommonValues(shadeLayerThickness, PermeabilityFactor, openness)};

        auto aLayer2 = Tarcog::ISO15099::Layers::shading(effectiveLayer.effectiveThickness(),
                                                         shadeLayerConductance,
                                                         effectiveLayer.getEffectiveOpenness());

        ASSERT_TRUE(aLayer2 != nullptr);

        auto gapThickness = 0.0127;
        auto GapLayer1 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(GapLayer1 != nullptr);

        auto GapLayer2 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(GapLayer2 != nullptr);

        constexpr double windowWidth = 1;
        constexpr double windowHeight = 1;

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({aLayer1, GapLayer1, aLayer2, GapLayer2, aLayer3});

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_unique<Tarcog::ISO15099::CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    [[nodiscard]] Tarcog::ISO15099::CSingleSystem * GetSystem() const
    {
        return m_TarcogSystem.get();
    };
};

TEST_F(TestInBetweenShadeAir, Test1)
{
    SCOPED_TRACE("Begin Test: InBetween Shade - Air");

    auto aSystem = GetSystem();

    const std::vector correctTemperature{
      257.940780, 258.406765, 271.482442, 271.486793, 283.488766, 283.954752};
    Helper::testVectors("Temperature", correctTemperature, aSystem->getTemperatures(), 1e-6);

    const std::vector correctRadiosity{
      249.270675, 260.410741, 300.371630, 316.028422, 358.162034, 378.466576};
    Helper::testVectors("Radiosity", correctRadiosity, aSystem->getRadiosities(), 1e-6);

    EXPECT_EQ(25u, aSystem->getNumberOfIterations());
}
