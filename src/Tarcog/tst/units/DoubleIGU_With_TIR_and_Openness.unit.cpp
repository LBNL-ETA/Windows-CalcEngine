#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>
#include <WCECommon.hpp>

class DoubleIGU_With_TIR_and_Openness : public testing::Test
{
    std::shared_ptr<Tarcog::ISO15099::CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        // Outdoor
        /////////////////////////////////////////////////////////
        constexpr auto airTemperature = 255.15;   // Kelvins
        constexpr auto airSpeed = 5.5;            // meters per second
        constexpr auto tSky = 255.15;             // Kelvins
        constexpr auto solarRadiation = 0.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        // Indoor
        /////////////////////////////////////////////////////////

        constexpr auto roomTemperature = 294.15;

        const auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        // IGU
        /////////////////////////////////////////////////////////
        constexpr auto solidLayerThickness = 0.003048;   // [m]
        constexpr auto solidLayerConductance = 1.0;

        const auto layer1 =
          Tarcog::ISO15099::Layers::shading(solidLayerThickness, solidLayerConductance);
        ASSERT_TRUE(layer1 != nullptr);

        constexpr auto shadeLayerThickness = 0.001;
        constexpr auto shadeLayerConductance = 0.15;
        constexpr auto emissivity = 0.796259999275;
        constexpr auto tir = 0.10916;
        constexpr auto PermeabilityFactor = 0.049855;

        const auto effectiveLayer{
          EffectiveLayers::makeCommonValues(shadeLayerThickness, PermeabilityFactor)};

        const auto layer2 = Tarcog::ISO15099::Layers::shading(shadeLayerThickness,
                                                              shadeLayerConductance,
                                                              effectiveLayer.getEffectiveOpenness(),
                                                              emissivity,
                                                              tir,
                                                              emissivity,
                                                              tir);

        ASSERT_TRUE(layer2 != nullptr);

        constexpr auto gapThickness = 0.0127;
        const auto gap1 = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gap1 != nullptr);

        constexpr double windowWidth = 1;
        constexpr double windowHeight = 1;

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, gap1, layer2});

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_shared<Tarcog::ISO15099::CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    [[nodiscard]] Tarcog::ISO15099::CSingleSystem getSystem() const
    {
        return *m_TarcogSystem;
    }
};

TEST_F(DoubleIGU_With_TIR_and_Openness, Test1)
{
    SCOPED_TRACE("Begin Test: Outdoor Shade - Air");

    const auto aSystem = getSystem();

    const auto temperature = aSystem.getTemperatures();
    const auto radiosity = aSystem.getRadiosities();

    const std::vector correctTemp{259.350462, 259.724865, 279.767733, 280.443187};
    const std::vector correctJ{253.917317, 272.505694, 348.677765, 349.142912};

    EXPECT_EQ(correctTemp.size(), temperature.size());
    EXPECT_EQ(correctJ.size(), radiosity.size());

    for(size_t i = 0; i < temperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemp[i], temperature[i], 1e-6);
        EXPECT_NEAR(correctJ[i], radiosity[i], 1e-6);
    }

    const auto numOfIter = aSystem.getNumberOfIterations();
    EXPECT_EQ(24u, numOfIter);

    const auto ventilatedFlowOutdoor =
      aSystem.getVentilationFlow(Tarcog::ISO15099::Environment::Outdoor);
    EXPECT_NEAR(0.0, ventilatedFlowOutdoor, 1e-6);

    const auto ventilatedFlowIndoor =
      aSystem.getVentilationFlow(Tarcog::ISO15099::Environment::Indoor);
    EXPECT_NEAR(9.152949, ventilatedFlowIndoor, 1e-6);

    const auto uValue = aSystem.getUValue();
    EXPECT_NEAR(3.149632, uValue, 1e-6);
}
