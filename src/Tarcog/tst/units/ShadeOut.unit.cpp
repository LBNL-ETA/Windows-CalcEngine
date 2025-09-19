#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

#include "vectorTesting.hpp"

class TestShadeOut : public testing::Test
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

        auto roomTemperature = 294.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        auto emissivity = 0.832855582237;
        auto transmittance = 0.074604861438;

        auto shadeLayerThickness = 0.0006;
        auto shadeLayerConductance = 160.0;
        auto PermeabilityFactor = 0.5;

        const auto effectiveLayer{EffectiveLayers::makeCommonValues(shadeLayerThickness, PermeabilityFactor)};


        auto layer1 = Tarcog::ISO15099::Layers::shading(effectiveLayer.effectiveThickness(),
                                                        shadeLayerConductance,
                                                        effectiveLayer.getEffectiveOpenness(),
                                                        emissivity,
                                                        transmittance,
                                                        emissivity,
                                                        transmittance);

        ASSERT_TRUE(layer1 != nullptr);

        auto solidLayerThickness = 0.0056134;   // [m]
        auto solidLayerConductance = 1.0;
        auto emissivity1 = 0.84;
        auto emissivity2 = 0.038798544556;
        transmittance = 0.0;

        auto layer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness,
                                                      solidLayerConductance,
                                                      emissivity1,
                                                      transmittance,
                                                      emissivity2,
                                                      transmittance);
        ASSERT_TRUE(layer2 != nullptr);

        auto gapThickness = 0.0127;
        auto gap = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(gap != nullptr);

        constexpr double windowWidth = 1;
        constexpr double windowHeight = 1;

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, gap, layer2});

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_unique<Tarcog::ISO15099::CSingleSystem>(aIGU, Indoor, Outdoor);

        m_TarcogSystem->solve();
    }

public:
    [[nodiscard]] Tarcog::ISO15099::CSingleSystem * GetSystem() const
    {
        return m_TarcogSystem.get();
    }
};

TEST_F(TestShadeOut, Test1)
{
    SCOPED_TRACE("Begin Test: Single Clear - U-value");

    constexpr auto Tolerance = 1e-6;

    const auto aSystem = GetSystem();

    const std::vector correctTemperature{257.039975, 257.040388, 269.858197, 270.315817};
    Helper::testVectors("Temperature", correctTemperature, aSystem->getTemperatures(), Tolerance);

    const std::vector correctRadiosity{250.203343, 251.145517, 292.753317, 419.735593};
    Helper::testVectors("Radiosity", correctRadiosity, aSystem->getRadiosities(), Tolerance);
}
