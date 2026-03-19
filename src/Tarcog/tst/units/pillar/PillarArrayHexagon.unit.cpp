#include <memory>
#include <gtest/gtest.h>
#include <WCETarcog.hpp>

class PillarArrayHexagon : public testing::Test
{
protected:
    // Keep IGU alive as a fixture member
    std::unique_ptr<Tarcog::ISO15099::CIGU> m_IGU;

    std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer> createModel(double length)
    {
        /////////////////////////////////////////////////////////
        /// IGU setup
        /////////////////////////////////////////////////////////
        constexpr auto t_Thickness = 0.003;          // [m]
        constexpr auto t_TransmittanceIR = 0.0;      // [-]
        constexpr auto t_EmissivityFrontIR = 0.84;   // [-]
        constexpr auto t_EmissivityBackIR = 0.84;    // [-]
        constexpr auto glassConductance = 1.0;       // [W/(m·K)]

        auto layer1 = Tarcog::ISO15099::Layers::solid(
            t_Thickness, glassConductance,
            t_EmissivityFrontIR, t_TransmittanceIR,
            t_EmissivityBackIR, t_TransmittanceIR);

        auto layer2 = Tarcog::ISO15099::Layers::solid(
            t_Thickness, glassConductance,
            t_EmissivityFrontIR, t_TransmittanceIR,
            t_EmissivityBackIR, t_TransmittanceIR);

        const auto gapPressure = 0.0;               // [Pa]
        const auto pillarHeight = 0.2e-3;           // [m]
        const auto pillarConductivity = 20.0;       // [W/(m·K)]
        const auto pillarArea = 0.02 * 0.02;        // [m²]

        const Tarcog::ISO15099::HexagonPillar pillar{
            pillarHeight, pillarConductivity, pillarArea, length};

        auto gap = Tarcog::ISO15099::Layers::createPillar(pillar, gapPressure);

        const auto windowWidth = 1.0;
        const auto windowHeight = 1.0;
        m_IGU = std::make_unique<Tarcog::ISO15099::CIGU>(windowWidth, windowHeight);
        m_IGU->addLayers({layer1, gap, layer2});

        // Dummy temperature gradient for conductive flow
        auto frontSurface = gap->getSurface(FenestrationCommon::Side::Front);
        frontSurface->setTemperature(290);

        auto backSurface = gap->getSurface(FenestrationCommon::Side::Back);
        backSurface->setTemperature(291);

        return gap;
    }
};

TEST_F(PillarArrayHexagon, Test1)
{
    constexpr auto tolerance = 1e-6;
    constexpr auto length = 0.25e-3;   // [m]

    const auto aGap = createModel(length);
    ASSERT_TRUE(aGap);
    EXPECT_NEAR(1.105776, aGap->getConvectionConductionFlow(), tolerance);
}
