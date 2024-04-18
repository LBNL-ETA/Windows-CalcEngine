#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

class PillarArrayCShapedCylinder : public testing::Test
{
protected:
    static std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer>
      createModel(double innerRadius, double outerRadius, double glass1Cond, double glass2Cond)
    {   /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        auto layer1Thickness = 0.003;          // [m]
        auto layer1TransmittanceIR = 0.0;      // [-]
        auto layer1EmissivityFrontIR = 0.84;   // [-]
        auto layer1EmissivityBackIR = 0.84;    // [-]

        auto layer2Thickness = 0.003;          // [m]
        auto layer2TransmittanceIR = 0.0;      // [-]
        auto layer2EmissivityFrontIR = 0.84;   // [-]
        auto layer2EmissivityBackIR = 0.84;    // [-]

        auto layer1 = Tarcog::ISO15099::Layers::solid(layer1Thickness,
                                                      glass1Cond,
                                                      layer1EmissivityFrontIR,
                                                      layer1TransmittanceIR,
                                                      layer1EmissivityBackIR,
                                                      layer1TransmittanceIR);

        auto layer2 = Tarcog::ISO15099::Layers::solid(layer2Thickness,
                                                      glass2Cond,
                                                      layer2EmissivityFrontIR,
                                                      layer2TransmittanceIR,
                                                      layer2EmissivityBackIR,
                                                      layer2TransmittanceIR);

        // setting pressure to zero will calculate conductivity of the pillar array
        const auto gapPressure = 0.0;   // [Pa]

        // Add support pillars
        const auto pillarHeight = 0.2e-3;       // [m]
        const auto pillarConductivity = 20.0;   // [W/(m·K)]
        const auto pillarArea = 0.02 * 0.02;    // [m²]
        const auto coverageFraction = 0.65;     // [-]

        const Tarcog::ISO15099::CShapedCylinderPillar pillar{
          pillarHeight, pillarConductivity, pillarArea, innerRadius, outerRadius, coverageFraction};

        auto gap = Tarcog::ISO15099::Layers::createPillar(pillar, gapPressure);

        auto windowWidth = 1.0;    // [m]
        auto windowHeight = 1.0;   // [m]
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, gap, layer2});

        // No real IGU calculations will be performed. One degree is set so we can obtain
        // conductivity of the pillar array
        auto frontSurface{gap->getSurface(FenestrationCommon::Side::Front)};
        frontSurface->setTemperature(290);

        auto backSurface{gap->getSurface(FenestrationCommon::Side::Back)};
        backSurface->setTemperature(291);

        return gap;
    }
};

TEST_F(PillarArrayCShapedCylinder, RadiusesVeryDifferent)
{
    constexpr auto tolerance = 1e-6;
    auto glass1Conductance = 1.0;           // [W/(m·K)]
    auto glass2Conductance = 1.0;           // [W/(m·K)]
    constexpr auto innerRadius = 0.05e-3;   // [m]
    constexpr auto outerRadius = 0.25e-3;   // [m]

    const auto aGap = createModel(innerRadius, outerRadius, glass1Conductance, glass2Conductance);

    ASSERT_TRUE(aGap != nullptr);

    const auto heatFlow = aGap->getConvectionConductionFlow();
    EXPECT_NEAR(0.938363, heatFlow, tolerance);
}

TEST_F(PillarArrayCShapedCylinder, RadiusesSimilar)
{
    constexpr auto tolerance = 1e-6;
    auto glass1Conductance = 1.0;           // [W/(m·K)]
    auto glass2Conductance = 1.0;           // [W/(m·K)]
    constexpr auto innerRadius = 0.21e-3;   // [m]
    constexpr auto outerRadius = 0.25e-3;   // [m]

    const auto aGap = createModel(innerRadius, outerRadius, glass1Conductance, glass2Conductance);

    ASSERT_TRUE(aGap != nullptr);

    const auto heatFlow = aGap->getConvectionConductionFlow();
    EXPECT_NEAR(0.763210, heatFlow, tolerance);
}

TEST_F(PillarArrayCShapedCylinder, DifferentGlassConductances)
{
    constexpr auto tolerance = 1e-6;
    auto glass1Conductance = 1.0;           // [W/(m·K)]
    auto glass2Conductance = 5.0;           // [W/(m·K)]
    constexpr auto innerRadius = 0.21e-3;   // [m]
    constexpr auto outerRadius = 0.25e-3;   // [m]

    const auto aGap = createModel(innerRadius, outerRadius, glass1Conductance, glass2Conductance);

    ASSERT_TRUE(aGap != nullptr);

    const auto heatFlow = aGap->getConvectionConductionFlow();
    EXPECT_NEAR(1.189583, heatFlow, tolerance);
}

TEST_F(PillarArrayCShapedCylinder, InnerRadiusZero)
{
    constexpr auto tolerance = 1e-6;
    auto glass1Conductance = 1.0;           // [W/(m·K)]
    auto glass2Conductance = 1.0;           // [W/(m·K)]
    constexpr auto innerRadius = 0.0;       // [m]
    constexpr auto outerRadius = 0.25e-3;   // [m]

    const auto aGap = createModel(innerRadius, outerRadius, glass1Conductance, glass2Conductance);

    ASSERT_TRUE(aGap != nullptr);

    const auto heatFlow = aGap->getConvectionConductionFlow();
    EXPECT_NEAR(0.940245, heatFlow, tolerance);
}

TEST_F(PillarArrayCShapedCylinder, IdenticalRadiuses)
{
    constexpr auto tolerance = 1e-6;
    auto glass1Conductance = 1.0;           // [W/(m·K)]
    auto glass2Conductance = 1.0;           // [W/(m·K)]
    constexpr auto innerRadius = 0.25e-3;   // [m]
    constexpr auto outerRadius = 0.25e-3;   // [m]

    const auto aGap = createModel(innerRadius, outerRadius, glass1Conductance, glass2Conductance);

    ASSERT_TRUE(aGap != nullptr);

    const auto heatFlow = aGap->getConvectionConductionFlow();
    EXPECT_NEAR(0.000199, heatFlow, tolerance);
}