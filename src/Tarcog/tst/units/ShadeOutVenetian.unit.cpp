#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

#include "vectorTesting.hpp"

// This test is created to accommodate same test in WC so we can confirm the results (Shading material
// is NFRC 31100 - White Venetian)
class TestShadeOutVenetian : public testing::Test
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

        // These values are taken from WC optical calculations
        auto emissivityFront = 0.52204944036998135;
        auto emissivityBack = 0.52204944036998102;
        auto transmittanceFront = 0.44074853220969445;
        auto transmittanceBack = 0.44074853220969445;

        auto materialThickness = 0.0001;
        auto materialConductance = 160.0;

        double windowWidth = 1;
        double windowHeight = 1;

        FenestrationCommon::Venetian::Geometry geometry{0.05, 0.07, 45, 0.00};

        EffectiveLayers::EffectiveHorizontalVenetian effectiveVenetian{
          windowWidth, windowHeight, materialThickness, geometry};

        auto layer1 = Tarcog::ISO15099::Layers::shading(effectiveVenetian.effectiveThickness(),
                                                        materialConductance,
                                                        effectiveVenetian.getEffectiveOpenness(),
                                                        emissivityFront,
                                                        transmittanceFront,
                                                        emissivityBack,
                                                        transmittanceBack);

        ASSERT_TRUE(layer1 != nullptr);

        const auto solidLayerThickness = 0.003048;   // [m]
        const auto solidLayerConductance = 1.0;
        const auto emissivity1 = 0.84;
        const auto emissivity2 = 0.84;
        const auto transmittance = 0.0;

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

TEST_F(TestShadeOutVenetian, Test1)
{
    SCOPED_TRACE("Begin Test: ShadeOutVenetian - U-value");

    constexpr auto Tolerance = 1e-6;

    const auto aSystem = GetSystem();

    EXPECT_NEAR(3.244674, aSystem->getUValue(), Tolerance);

    const std::vector correctTemperature{257.504498, 257.795556, 276.669580, 277.055281};
    Helper::testVectors("Temperature", correctTemperature, aSystem->getTemperatures(), Tolerance);

    const std::vector correctRadiosity{279.595282, 248.498982, 318.811755, 348.524663};
    Helper::testVectors("Radiosity", correctRadiosity, aSystem->getRadiosities(), Tolerance);
}
