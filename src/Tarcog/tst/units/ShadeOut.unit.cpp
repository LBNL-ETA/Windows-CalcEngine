#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"
#include "WCECommon.hpp"

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
        auto Atop = 0.0;
        auto Abot = 0.0;
        auto Aleft = 0.0;
        auto Aright = 0.0;
        auto Afront = 0.5;
        auto PermeabilityFactor = 0.99;

        EffectiveLayers::ShadeOpenness openness{Afront, Aleft, Aright, Atop, Abot};

        double windowWidth = 1;
        double windowHeight = 1;

        EffectiveLayers::EffectiveLayerOther effectiveLayer{
          windowWidth, windowHeight, shadeLayerThickness, openness, PermeabilityFactor};

        auto effectiveThermal = effectiveLayer.getEffectiveOpenness();

        auto layer1 = Tarcog::ISO15099::Layers::shading(shadeLayerThickness,
                                                        shadeLayerConductance,
                                                        effectiveThermal,
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

        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, gap, layer2});

        // Alternative way of adding layers.
        // aIGU.addLayer(layer1);
        // aIGU.addLayer(gap);
        // aIGU.addLayer(layer2);

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::unique_ptr<Tarcog::ISO15099::CSingleSystem>(
          new Tarcog::ISO15099::CSingleSystem(aIGU, Indoor, Outdoor));

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

    const auto Temperature = aSystem->getTemperatures();
    const std::vector correctTemperature{256.98924905242745, 257.00908910176321, 269.67211159465268, 270.13404194545456};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], Tolerance);
    }

    const auto Radiosity = aSystem->getRadiosities();
    const std::vector correctRadiosity{249.98676196927647, 250.97825831600656, 292.03061975484945, 419.70403266542121};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], Tolerance);
    }
}
