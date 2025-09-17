#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

#include "vectorTesting.hpp"

class TestDoubleClearOutdoorShadeAir : public testing::Test
{
private:
    std::shared_ptr<Tarcog::ISO15099::CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        // Outdoor
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
        // Indoor
        /////////////////////////////////////////////////////////

        auto roomTemperature = 295.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        // IGU
        /////////////////////////////////////////////////////////
        auto solidLayerThickness = 0.005715;   // [m]
        auto solidLayerConductance = 1.0;

        auto shadeLayerThickness = 0.01;
        auto shadeLayerConductance = 160.0;
        auto dtop = 0.1;
        auto dbot = 0.1;
        auto dleft = 0.1;
        auto dright = 0.1;
        auto PermeabilityFactor = 0.2;

        EffectiveLayers::ShadeOpenness openness{dleft, dright, dtop, dbot};

        EffectiveLayers::EffectiveLayerCommon effectiveLayer{shadeLayerThickness, PermeabilityFactor, openness};

        auto layer1 = Tarcog::ISO15099::Layers::shading(
          shadeLayerThickness, shadeLayerConductance, effectiveLayer.getEffectiveOpenness());

        ASSERT_TRUE(layer1 != nullptr);

        auto layer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(layer2 != nullptr);

        auto layer3 = Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);

        auto gapThickness = 0.0127;
        auto gap1 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gap1 != nullptr);

        auto gap2 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gap2 != nullptr);

        constexpr double windowWidth = 1;
        constexpr double windowHeight = 1;

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, gap1, layer2, gap2, layer3});

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_shared<Tarcog::ISO15099::CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    std::shared_ptr<Tarcog::ISO15099::CSingleSystem> getSystem() const
    {
        return m_TarcogSystem;
    };
};

TEST_F(TestDoubleClearOutdoorShadeAir, Test1)
{
    SCOPED_TRACE("Begin Test: Outdoor Shade - Air");

    auto aSystem = getSystem();

    auto temperature = aSystem->getTemperatures();
    auto radiosity = aSystem->getRadiosities();

    const std::vector<double> correctTemp = {
      256.947912, 256.952013, 269.280883, 269.726307, 283.977613, 284.423038};
    const std::vector<double> correctJ = {
      246.043352, 254.183208, 291.084956, 309.557246, 359.253818, 380.514114};

    Helper::testVectors("Temperature", correctTemp, temperature, 1e-6);
    Helper::testVectors("Radiosity", correctJ, radiosity, 1e-6);

    const auto numOfIter = aSystem->getNumberOfIterations();
    EXPECT_EQ(70u, numOfIter);

    const auto ventilatedFlow = aSystem->getVentilationFlow(Tarcog::ISO15099::Environment::Outdoor);
    EXPECT_NEAR(-25.444198, ventilatedFlow, 1e-6);
}
