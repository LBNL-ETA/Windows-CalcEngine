#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCEGases.hpp"
#include "WCETarcog.hpp"
#include "WCECommon.hpp"

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
        auto Afront = 0.2;
        auto PermeabilityFactor = 0.2;

        EffectiveLayers::ShadeOpenness openness{Afront, Aleft, Aright, Atop, Abot};

        double windowWidth = 1;
        double windowHeight = 1;

        EffectiveLayers::EffectiveLayerOther effectiveLayer{
          windowWidth, windowHeight, shadeLayerThickness, openness, PermeabilityFactor};

        auto aLayer2 = Tarcog::ISO15099::Layers::shading(
          shadeLayerThickness, shadeLayerConductance, effectiveLayer.getEffectiveOpenness());

        ASSERT_TRUE(aLayer2 != nullptr);

        auto gapThickness = 0.0127;
        auto GapLayer1 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(GapLayer1 != nullptr);

        auto GapLayer2 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(GapLayer2 != nullptr);

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({aLayer1, GapLayer1, aLayer2, GapLayer2, aLayer3});

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::unique_ptr<Tarcog::ISO15099::CSingleSystem>(
          new Tarcog::ISO15099::CSingleSystem(aIGU, Indoor, Outdoor));
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    Tarcog::ISO15099::CSingleSystem * GetSystem() const
    {
        return m_TarcogSystem.get();
    };
};

TEST_F(TestInBetweenShadeAir, Test1)
{
    SCOPED_TRACE("Begin Test: InBetween Shade - Air");

    auto aSystem = GetSystem();

    const auto temperature = aSystem->getTemperatures();
    const auto radiosity = aSystem->getRadiosities();

    std::vector<double> correctTemp{
      257.929443, 258.393532, 271.502308, 271.506693, 283.533816, 283.997905};
    std::vector<double> correctJ{
      249.233614, 260.378539, 300.442209, 316.138397, 358.375200, 378.654837};

    EXPECT_EQ(correctTemp.size(), temperature.size());
    EXPECT_EQ(correctJ.size(), radiosity.size());

    for(size_t i = 0; i < temperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemp[i], temperature[i], 1e-6);
        EXPECT_NEAR(correctJ[i], radiosity[i], 1e-6);
    }

    const auto numOfIter = aSystem->getNumberOfIterations();
    EXPECT_EQ(25u, numOfIter);
}
