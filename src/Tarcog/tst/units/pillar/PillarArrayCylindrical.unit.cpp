#include <memory>
#include <gtest/gtest.h>
#include <WCETarcog.hpp>

class PillarArrayCylindrical : public testing::Test
{
private:
    std::unique_ptr<Tarcog::ISO15099::CIGU> m_IGU;
    std::shared_ptr<Tarcog::ISO15099::CIGUGapLayer> m_Gap;

protected:
    void SetUp() override
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
        const auto pillarRadius = 0.25e-3;          // [m]

        const Tarcog::ISO15099::CylindricalPillar pillar{
            pillarHeight, pillarConductivity, pillarArea, pillarRadius};

        m_Gap = Tarcog::ISO15099::Layers::createPillar(pillar, gapPressure);

        // Keep IGU alive as member
        const auto windowWidth = 1.0;
        const auto windowHeight = 1.0;
        m_IGU = std::make_unique<Tarcog::ISO15099::CIGU>(windowWidth, windowHeight);
        m_IGU->addLayers({layer1, m_Gap, layer2});

        // Assign dummy temperatures for conduction flow
        auto frontSurface = m_Gap->getSurface(FenestrationCommon::Side::Front);
        frontSurface->setTemperature(290);

        auto backSurface = m_Gap->getSurface(FenestrationCommon::Side::Back);
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
    ASSERT_TRUE(aGap);
    EXPECT_NEAR(1.218959, aGap->getConvectionConductionFlow(), tolerance);
}
