#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

class PillarArrayRectangular : public testing::Test
{
protected:
    std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer> createModel(double length, double width)
    {   /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        auto layer1Thickness = 0.003;          // [m]
        auto layer1Conductance = 1.0;          // [W/(m·K)]
        auto layer1TransmittanceIR = 0.0;      // [-]
        auto layer1EmissivityFrontIR = 0.84;   // [-]
        auto layer1EmissivityBackIR = 0.84;    // [-]

        auto layer2Thickness = 0.003;          // [m]
        auto layer2Conductance = 1.0;          // [W/(m·K)]
        auto layer2TransmittanceIR = 0.0;      // [-]
        auto layer2EmissivityFrontIR = 0.84;   // [-]
        auto layer2EmissivityBackIR = 0.84;    // [-]

        auto layer1 = Tarcog::ISO15099::Layers::solid(layer1Thickness,
                                                      layer1Conductance,
                                                      layer1EmissivityFrontIR,
                                                      layer1TransmittanceIR,
                                                      layer1EmissivityBackIR,
                                                      layer1TransmittanceIR);

        auto layer2 = Tarcog::ISO15099::Layers::solid(layer2Thickness,
                                                      layer2Conductance,
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

        Tarcog::ISO15099::RectangularPillar pillar{pillarHeight, pillarConductivity, pillarArea, length, width};

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

TEST_F(PillarArrayRectangular, WidthMuchBiggerThanLength)
{
    constexpr auto tolerance = 1e-6;
    constexpr auto length = 0.25e-3;   //[m]
    constexpr auto width = 0.05e-3;   //[m]

    const auto aGap = createModel(length, width);

    ASSERT_TRUE(aGap != nullptr);

    const auto heatFlow = aGap->getConvectionConductionFlow();
    EXPECT_NEAR(0.328667, heatFlow, tolerance);
}
