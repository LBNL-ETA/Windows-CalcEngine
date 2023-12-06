#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

class NFRC102_NFRC102_VacuumHexagonPillar : public testing::Test
{
private:
    std::shared_ptr<Tarcog::ISO15099::CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        auto airTemperature = 255.15;   // Kelvins
        auto airSpeed = 5.5;            // meters per second
        auto tSky = 255.15;             // Kelvins
        auto solarRadiation = 0.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        auto roomTemperature = 294.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        auto solidLayerThickness = 3.048e-3;   // [m]
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

        // Add support pillars
        const auto pillarLength = 0.1e-3;
        const auto pillarHeight = 0.1e-3;
        const auto gapPressure = 0.1333;
        const auto pillarConductivity = 999.0;
        const auto pillarArea = 0.03 * 0.03;

        Tarcog::ISO15099::PolygonalPillar pillar{pillarHeight,
                                                 pillarConductivity,
                                                 pillarArea,
                                                 pillarLength,
                                                 Tarcog::ISO15099::PolygonType::Hexagon};

        auto pillarGap = Tarcog::ISO15099::Layers::createPillar(pillar, gapPressure);

        ASSERT_TRUE(pillarGap != nullptr);

        auto windowWidth = 1.0;   //[m]
        auto windowHeight = 1.0;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, pillarGap, layer2});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_shared<Tarcog::ISO15099::CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    [[nodiscard]] std::shared_ptr<Tarcog::ISO15099::CSingleSystem> GetSystem() const
    {
        return m_TarcogSystem;
    }
};

TEST_F(NFRC102_NFRC102_VacuumHexagonPillar, Test1)
{
    constexpr auto tolerance = 1e-6;

    const auto aSystem = GetSystem();

    ASSERT_TRUE(aSystem != nullptr);

    const auto Temperature = aSystem->getTemperatures();
    std::vector correctTemperature = {258.079100, 258.339966, 282.098943, 282.359810};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], tolerance);
    }

    const auto Radiosity = aSystem->getRadiosities();
    std::vector correctRadiosity = {249.723254, 267.230431, 344.366497, 370.640212};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], tolerance);
    }

    const auto numOfIter = aSystem->getNumberOfIterations();
    EXPECT_EQ(35u, numOfIter);

    const auto uValue{aSystem->getUValue()};
    EXPECT_NEAR(2.194517, uValue, tolerance);
}
