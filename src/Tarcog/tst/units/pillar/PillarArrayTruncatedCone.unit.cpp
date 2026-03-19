#include <memory>
#include <gtest/gtest.h>
#include <WCETarcog.hpp>

class PillarArrayTruncatedCone : public testing::Test
{
protected:
    // Persist IGU for test lifetime (prevents dangling weak_ptrs)
    std::unique_ptr<Tarcog::ISO15099::CIGU> m_IGU;

    std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>
    createModel(double radius1, double radius2, double glass1Cond, double glass2Cond)
    {
        /////////////////////////////////////////////////////////
        /// IGU setup
        /////////////////////////////////////////////////////////
        constexpr auto t_Thickness = 0.003;          // [m]
        constexpr auto t_TransmittanceIR = 0.0;      // [-]
        constexpr auto t_EmissivityFrontIR = 0.84;   // [-]
        constexpr auto t_EmissivityBackIR = 0.84;    // [-]

        auto layer1 = Tarcog::ISO15099::Layers::solid(
            t_Thickness, glass1Cond,
            t_EmissivityFrontIR, t_TransmittanceIR,
            t_EmissivityBackIR, t_TransmittanceIR);

        auto layer2 = Tarcog::ISO15099::Layers::solid(
            t_Thickness, glass2Cond,
            t_EmissivityFrontIR, t_TransmittanceIR,
            t_EmissivityBackIR, t_TransmittanceIR);

        // Truncated cone pillar setup
        const auto gapPressure = 0.0;           // [Pa]
        const auto pillarHeight = 0.2e-3;       // [m]
        const auto pillarConductivity = 20.0;   // [W/(m·K)]
        const auto pillarArea = 0.02 * 0.02;    // [m²]

        const Tarcog::ISO15099::TruncatedConePillar pillar{
            pillarHeight, pillarConductivity, pillarArea, radius1, radius2};

        auto gap = Tarcog::ISO15099::Layers::createPillar(pillar, gapPressure);

        // Keep IGU alive to maintain valid references
        const auto windowWidth = 1.0;    // [m]
        const auto windowHeight = 1.0;   // [m]
        m_IGU = std::make_unique<Tarcog::ISO15099::CIGU>(windowWidth, windowHeight);
        m_IGU->addLayers({layer1, gap, layer2});

        // Simple artificial temperature delta
        auto frontSurface = gap->getSurface(FenestrationCommon::Side::Front);
        frontSurface->setTemperature(290);
        auto backSurface = gap->getSurface(FenestrationCommon::Side::Back);
        backSurface->setTemperature(291);

        return gap;
    }
};

TEST_F(PillarArrayTruncatedCone, RadiusesVeryDifferent)
{
    constexpr auto tolerance = 1e-6;
    const auto aGap = createModel(0.25e-3, 0.05e-3, 1.0, 1.0);

    ASSERT_TRUE(aGap);
    EXPECT_NEAR(0.407069, aGap->getConvectionConductionFlow(), tolerance);
}

TEST_F(PillarArrayTruncatedCone, RadiusesSimilar)
{
    constexpr auto tolerance = 1e-6;
    const auto aGap = createModel(0.21e-3, 0.25e-3, 1.0, 1.0);

    ASSERT_TRUE(aGap);
    EXPECT_NEAR(1.110791, aGap->getConvectionConductionFlow(), tolerance);
}

TEST_F(PillarArrayTruncatedCone, GlassConductancesDifferent)
{
    constexpr auto tolerance = 1e-6;
    const auto aGap = createModel(0.21e-3, 0.25e-3, 0.1, 1.0);

    ASSERT_TRUE(aGap);
    EXPECT_NEAR(0.192828, aGap->getConvectionConductionFlow(), tolerance);
}

TEST_F(PillarArrayTruncatedCone, GlassConductancesVeryDifferent)
{
    constexpr auto tolerance = 1e-6;
    const auto aGap = createModel(0.21e-3, 0.25e-3, 0.1, 15.0);

    ASSERT_TRUE(aGap);
    EXPECT_NEAR(0.207786, aGap->getConvectionConductionFlow(), tolerance);
}
