#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

class PillarArrayAnnulusCylinder : public testing::Test
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
        auto gapPressure = 0.0;   // [Pa]

        // Add support pillars
        auto pillarHeight = 0.2e-3;       // [m]
        auto pillarConductivity = 20.0;   // [W/(m·K)]
        auto pillarArea = 0.02 * 0.02;    // [m²]
        auto gap = Tarcog::ISO15099::Layers::annulusCylinderPillar(
          innerRadius, outerRadius, pillarHeight, pillarConductivity, pillarArea, gapPressure);

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

TEST_F(PillarArrayAnnulusCylinder, RadiusesVeryDifferent)
{
    constexpr auto tolerance = 1e-6;
    auto glass1Conductance = 1.0;           // [W/(m·K)]
    auto glass2Conductance = 1.0;           // [W/(m·K)]
    constexpr auto innerRadius = 0.05e-3;   // [m]
    constexpr auto outerRadius = 0.25e-3;   // [m]

    const auto aGap = createModel(innerRadius, outerRadius, glass1Conductance, glass2Conductance);

    ASSERT_TRUE(aGap != nullptr);

    const auto heatFlow = aGap->getConvectionConductionFlow();
    EXPECT_NEAR(1.217163, heatFlow, tolerance);
}
