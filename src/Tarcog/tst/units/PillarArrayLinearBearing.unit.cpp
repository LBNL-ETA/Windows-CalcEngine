#include <memory>
#include <gtest/gtest.h>
#include <WCETarcog.hpp>

class PillarArrayLinearBearing : public testing::Test
{
protected:
    // Keep IGU alive as a member variable
    std::unique_ptr<Tarcog::ISO15099::CIGU> m_IGU;

    std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer> createModel(double length, double width)
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

        const Tarcog::ISO15099::LinearBearingPillar pillar{
            pillarHeight, pillarConductivity, pillarArea, length, width};

        auto gap = Tarcog::ISO15099::Layers::createPillar(pillar, gapPressure);

        // Keep IGU alive across the test
        const auto windowWidth = 1.0;
        const auto windowHeight = 1.0;
        m_IGU = std::make_unique<Tarcog::ISO15099::CIGU>(windowWidth, windowHeight);
        m_IGU->addLayers({layer1, gap, layer2});

        // Artificial temperature difference for conduction
        auto frontSurface = gap->getSurface(FenestrationCommon::Side::Front);
        frontSurface->setTemperature(290);

        auto backSurface = gap->getSurface(FenestrationCommon::Side::Back);
        backSurface->setTemperature(291);

        return gap;
    }
};

TEST_F(PillarArrayLinearBearing, LengthMuchBiggerThanWidth)
{
    constexpr auto tolerance = 1e-6;
    constexpr auto length = 0.25e-3;   // [m]
    constexpr auto width  = 0.05e-3;   // [m]

    const auto aGap = createModel(length, width);
    ASSERT_TRUE(aGap);
    EXPECT_NEAR(0.328667, aGap->getConvectionConductionFlow(), tolerance);
}

TEST_F(PillarArrayLinearBearing, LengthAndWidthSimilar)
{
    constexpr auto tolerance = 1e-6;
    constexpr auto length = 0.07e-3;   // [m]
    constexpr auto width  = 0.05e-3;   // [m]

    const auto aGap = createModel(length, width);
    ASSERT_TRUE(aGap);
    EXPECT_NEAR(0.140157, aGap->getConvectionConductionFlow(), tolerance);
}
