#include <memory>
#include <gtest/gtest.h>
#include <WCETarcog.hpp>

class PillarArrayMeasured : public testing::Test
{
protected:
    // Keep IGU alive for the lifetime of the fixture
    std::unique_ptr<Tarcog::ISO15099::CIGU> m_IGU;

    std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>
    createModel(const Tarcog::ISO15099::PillarMeasurement & measuredPillar)
    {
        /////////////////////////////////////////////////////////
        /// IGU setup
        /////////////////////////////////////////////////////////
        const auto layer1TransmittanceIR = 0.0;     // [-]
        const auto layer1EmissivityFrontIR = 0.84;  // [-]

        const auto layer2TransmittanceIR = 0.0;     // [-]
        const auto layer2EmissivityBackIR = 0.84;   // [-]

        // Build the two glass layers using measured parameters
        auto layer1 = Tarcog::ISO15099::Layers::solid(
            measuredPillar.glass1.thickness,
            measuredPillar.glass1.conductivity,
            layer1EmissivityFrontIR,
            layer1TransmittanceIR,
            measuredPillar.glass1.emissivity,
            layer1TransmittanceIR);

        auto layer2 = Tarcog::ISO15099::Layers::solid(
            measuredPillar.glass2.thickness,
            measuredPillar.glass2.conductivity,
            measuredPillar.glass2.emissivity,
            layer2TransmittanceIR,
            layer2EmissivityBackIR,
            layer2TransmittanceIR);

        // Create pillar gap
        auto pillarGap = Tarcog::ISO15099::Layers::createPillar(measuredPillar);

        // Persist IGU as a member so layer links remain valid
        const auto windowWidth = 1.0;   // [m]
        const auto windowHeight = 1.0;  // [m]
        m_IGU = std::make_unique<Tarcog::ISO15099::CIGU>(windowWidth, windowHeight);
        m_IGU->addLayers({layer1, pillarGap, layer2});

        // Set dummy temperatures for heat flow calculation
        auto frontSurface = pillarGap->getSurface(FenestrationCommon::Side::Front);
        frontSurface->setTemperature(290);

        auto backSurface = pillarGap->getSurface(FenestrationCommon::Side::Back);
        backSurface->setTemperature(291);

        return pillarGap;
    }
};

TEST_F(PillarArrayMeasured, CStar)
{
    // Pillar measurement
    Tarcog::ISO15099::Glass glass1{0.003048, 1.0, 0.84};
    Tarcog::ISO15099::Glass glass2{0.00312, 1.0, 0.15};
    Tarcog::ISO15099::PillarMeasurement pillar{
        0.0064, 0.0112, 288.15, 308.15, glass1, glass2};

    const auto aGap = createModel(pillar);
    ASSERT_TRUE(aGap);

    constexpr auto tolerance = 1e-6;
    EXPECT_NEAR(0.891583, aGap->getConvectionConductionFlow(), tolerance);
}
