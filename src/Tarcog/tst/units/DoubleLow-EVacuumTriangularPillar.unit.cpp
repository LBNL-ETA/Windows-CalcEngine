#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

class DoubleLowEVacuumTriangularPillar : public testing::Test
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
        auto solidLayerThickness = 0.004;   // [m]
        auto solidLayerConductance = 1.0;
        auto TransmittanceIR = 0.0;
        auto emissivityFrontIR = 0.84;
        auto emissivityBackIR = 0.036749500781;

        auto layer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness,
                                                      solidLayerConductance,
                                                      emissivityFrontIR,
                                                      TransmittanceIR,
                                                      emissivityBackIR,
                                                      TransmittanceIR);

        solidLayerThickness = 0.0039624;
        emissivityBackIR = 0.84;

        auto layer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness,
                                                      solidLayerConductance,
                                                      emissivityFrontIR,
                                                      TransmittanceIR,
                                                      emissivityBackIR,
                                                      TransmittanceIR);

        // Add support pillars
        auto pillarLength = 0.1e-3;
        auto pillarHeight = 0.1e-3;
        auto gapPressure = 0.1333;
        auto pillarConductivity = 999.0;
        auto pillarSpacing = 0.03;
        auto pillarGap = Tarcog::ISO15099::Layers::triangularPillar(
          pillarLength, pillarHeight, pillarConductivity, pillarSpacing, gapPressure);

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

TEST_F(DoubleLowEVacuumTriangularPillar, Test1)
{
    constexpr auto tolerance = 1e-6;

    const auto aSystem = GetSystem();

    ASSERT_TRUE(aSystem != nullptr);

    const auto Temperature = aSystem->getTemperatures();
    std::vector correctTemperature = {255.557273, 255.604797, 292.272457, 292.319534};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], tolerance);
    }

    const auto Radiosity = aSystem->getRadiosities();
    std::vector correctRadiosity = {241.585541, 406.264744, 412.531259, 415.666263};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], tolerance);
    }

    const auto numOfIter = aSystem->getNumberOfIterations();
    EXPECT_EQ(35u, numOfIter);

    const auto uValue{aSystem->getUValue()};
    EXPECT_NEAR(0.304640, uValue, tolerance);
}
