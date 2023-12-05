#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

class PillarArrayCylindrical : public testing::Test
{
private:
    std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer> m_Gap;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        auto solidLayerThickness = 0.003;   // [m]
        auto solidLayerConductance = 1.0;
        auto TransmittanceIR = 0.0;
        auto emissivityFrontIR = 0.84;
        auto emissivityBackIR = 0.84;

        auto layer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness,
                                                      solidLayerConductance,
                                                      emissivityFrontIR,
                                                      TransmittanceIR,
                                                      emissivityBackIR,
                                                      TransmittanceIR);

        auto layer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness,
                                                      solidLayerConductance,
                                                      emissivityFrontIR,
                                                      TransmittanceIR,
                                                      emissivityBackIR,
                                                      TransmittanceIR);

        // setting pressure to zero will calculate conductivity of the pillar array
        auto gapPressure = 0.0;

        // Add support pillars
        auto pillarHeight = 0.2e-3;
        auto pillarConductivity = 20.0;
        auto pillarArea = 0.02 * 0.02;
        auto pillarRadius = 0.25e-3;
        m_Gap = Tarcog::ISO15099::Layers::cylindricalPillar(
          pillarRadius, pillarHeight, pillarConductivity, pillarArea, gapPressure);

        auto windowWidth = 1.0;   //[m]
        auto windowHeight = 1.0;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, m_Gap, layer2});

        // No real IGU calculations will be performed. One degree is set so we can obtain
        // conductivity of the pillar array
        auto frontSurface{m_Gap->getSurface(FenestrationCommon::Side::Front)};
        frontSurface->setTemperature(290);

        auto backSurface{m_Gap->getSurface(FenestrationCommon::Side::Back)};
        backSurface->setTemperature(291);

    }

public:
    [[nodiscard]] std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer> GetGap() const
    {
        return m_Gap;
    }
};

TEST_F(PillarArrayCylindrical, Test1)
{

    constexpr auto tolerance = 1e-6;

    const auto aGap = GetGap();

    ASSERT_TRUE(aGap != nullptr);

    const auto heatFlow = aGap->getConvectionConductionFlow();
    EXPECT_NEAR(1.218959, heatFlow, tolerance);
}
