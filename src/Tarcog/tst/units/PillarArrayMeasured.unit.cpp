#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

class PillarArrayMeasured : public testing::Test
{
protected:
    static std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer> createModel(
    const Tarcog::ISO15099::PillarMeasurement & measuredPillar)
    {   /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        auto layer1TransmittanceIR = 0.0;      // [-]
        auto layer1EmissivityFrontIR = 0.84;   // [-]

        auto layer2TransmittanceIR = 0.0;      // [-]
        auto layer2EmissivityBackIR = 0.84;    // [-]

        auto layer1 = Tarcog::ISO15099::Layers::solid(measuredPillar.glass1.thickness,
                                                      measuredPillar.glass1.conductivity,
                                                      layer1EmissivityFrontIR,
                                                      layer1TransmittanceIR,
                                                      measuredPillar.glass1.emissivity,
                                                      layer1TransmittanceIR);

        auto layer2 = Tarcog::ISO15099::Layers::solid(measuredPillar.glass2.thickness,
                                                      measuredPillar.glass2.conductivity,
                                                      measuredPillar.glass2.emissivity,
                                                      layer2TransmittanceIR,
                                                      layer2EmissivityBackIR,
                                                      layer2TransmittanceIR);

        auto pillarGap = Tarcog::ISO15099::Layers::createPillar(measuredPillar);

        auto windowWidth = 1.0;    // [m]
        auto windowHeight = 1.0;   // [m]
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, pillarGap, layer2});

        // No real IGU calculations will be performed. One degree is set so we can obtain
        // conductivity of the pillar array
        auto frontSurface{pillarGap->getSurface(FenestrationCommon::Side::Front)};
        frontSurface->setTemperature(290);

        auto backSurface{pillarGap->getSurface(FenestrationCommon::Side::Back)};
        backSurface->setTemperature(291);

        return pillarGap;
    }
};

TEST_F(PillarArrayMeasured, CStar)
{
    // Pillar measurement
        Tarcog::ISO15099::Glass glass1{0.003048, 1.0, 0.84};
        Tarcog::ISO15099::Glass glass2{0.00312, 1.0, 0.15};
        Tarcog::ISO15099::PillarMeasurement pillar{0.0064, 0.0112, 288.15, 308.15, glass1, glass2};

    const auto aGap = createModel(pillar);

    ASSERT_TRUE(aGap != nullptr);

    constexpr auto tolerance{1e-6};
    const auto heatFlow = aGap->getConvectionConductionFlow();
    EXPECT_NEAR(0.891583, heatFlow, tolerance);
}
