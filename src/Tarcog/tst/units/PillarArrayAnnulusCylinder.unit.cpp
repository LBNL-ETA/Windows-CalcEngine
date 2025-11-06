#include <memory>
#include <gtest/gtest.h>
#include <WCETarcog.hpp>

class PillarArrayAnnulusCylinder : public testing::Test
{
protected:
    // Keep IGU alive for the duration of the test otherwise gap will be deleted and test will crash
    std::unique_ptr<Tarcog::ISO15099::CIGU> m_IGU;

    std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer> createModel(
        double innerRadius, double outerRadius,
        double glass1Cond, double glass2Cond)
    {
        /////////////////////////////////////////////////////////
        /// IGU setup
        /////////////////////////////////////////////////////////
        constexpr auto t_Thickness = 0.003;          // [m]
        constexpr auto t_TransmittanceIR = 0.0;      // [-]
        constexpr auto t_EmissivityFrontIR = 0.84;   // [-]
        constexpr auto t_EmissivityBackIR = 0.84;    // [-]

        auto layer1 = Tarcog::ISO15099::Layers::solid(
            t_Thickness, glass1Cond, t_EmissivityFrontIR, t_TransmittanceIR,
            t_EmissivityBackIR, t_TransmittanceIR);

        auto layer2 = Tarcog::ISO15099::Layers::solid(
            t_Thickness, glass2Cond, t_EmissivityFrontIR, t_TransmittanceIR,
            t_EmissivityBackIR, t_TransmittanceIR);

        const auto gapPressure = 0.0;                // [Pa]
        const auto pillarHeight = 0.2e-3;            // [m]
        const auto pillarConductivity = 20.0;        // [W/(m·K)]
        const auto pillarArea = 0.02 * 0.02;         // [m²]

        const Tarcog::ISO15099::AnnulusCylinderPillar pillar{
            pillarHeight, pillarConductivity, pillarArea, innerRadius, outerRadius};

        auto gap = Tarcog::ISO15099::Layers::createPillar(pillar, gapPressure);

        // Keep IGU as member so it’s not destroyed after function returns
        const auto windowWidth = 1.0;
        const auto windowHeight = 1.0;
        m_IGU = std::make_unique<Tarcog::ISO15099::CIGU>(windowWidth, windowHeight);
        m_IGU->addLayers({layer1, gap, layer2});

        // Assign artificial temperatures for conduction flow
        auto frontSurface = gap->getSurface(FenestrationCommon::Side::Front);
        frontSurface->setTemperature(290);

        auto backSurface = gap->getSurface(FenestrationCommon::Side::Back);
        backSurface->setTemperature(291);

        return gap;
    }
};

// Now tests stay unchanged

TEST_F(PillarArrayAnnulusCylinder, RadiusesVeryDifferent)
{
    constexpr auto tol = 1e-6;
    const auto aGap = createModel(0.05e-3, 0.25e-3, 1.0, 1.0);
    ASSERT_TRUE(aGap != nullptr);
    EXPECT_NEAR(1.217163, aGap->getConvectionConductionFlow(), tol);
}

TEST_F(PillarArrayAnnulusCylinder, RadiusesSimilar)
{
    constexpr auto tol = 1e-6;
    const auto aGap = createModel(0.21e-3, 0.25e-3, 1.0, 1.0);
    ASSERT_TRUE(aGap != nullptr);
    EXPECT_NEAR(1.013254, aGap->getConvectionConductionFlow(), tol);
}

TEST_F(PillarArrayAnnulusCylinder, DifferentGlassConductances)
{
    constexpr auto tol = 1e-6;
    const auto aGap = createModel(0.21e-3, 0.25e-3, 1.0, 5.0);
    ASSERT_TRUE(aGap != nullptr);
    EXPECT_NEAR(1.613343, aGap->getConvectionConductionFlow(), tol);
}

TEST_F(PillarArrayAnnulusCylinder, InnerRadiusZero)
{
    constexpr auto tol = 1e-6;
    const auto aGap = createModel(0.0, 0.25e-3, 1.0, 1.0);
    ASSERT_TRUE(aGap != nullptr);
    EXPECT_NEAR(1.218959, aGap->getConvectionConductionFlow(), tol);
}

TEST_F(PillarArrayAnnulusCylinder, IdenticalRadiuses)
{
    constexpr auto tol = 1e-6;
    const auto aGap = createModel(0.25e-3, 0.25e-3, 1.0, 1.0);
    ASSERT_TRUE(aGap != nullptr);
    EXPECT_NEAR(0.000392, aGap->getConvectionConductionFlow(), tol);
}
