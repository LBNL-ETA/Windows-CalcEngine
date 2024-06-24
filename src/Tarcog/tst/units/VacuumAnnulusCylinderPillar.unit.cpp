#include <memory>
#include <gtest/gtest.h>

#include <WCETarcog.hpp>

class NFRC102_NFRC102_VacuumAnnulusCylinderPillar : public testing::Test
{
private:
    Tarcog::ISO15099::CSingleSystem m_TarcogSystem{createTarcogSystem()};

protected:
    void SetUp() override
    {
        m_TarcogSystem.solve();
    }

    Tarcog::ISO15099::CSingleSystem createTarcogSystem()
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
        if(Outdoor == nullptr)
            throw std::runtime_error("Failed to create Outdoor environment.");
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        auto roomTemperature = 294.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        if(Indoor == nullptr)
            throw std::runtime_error("Failed to create Indoor environment.");

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
        const auto innerRadius = 0.1e-3;
        const auto outerRadius = 0.2e-3;
        const auto pillarHeight = 0.1e-3;
        const auto gapPressure = 0.1333;
        const auto pillarConductivity = 999.0;
        const auto pillarArea = 0.03 * 0.03;

        Tarcog::ISO15099::AnnulusCylinderPillar pillar{
          pillarHeight, pillarConductivity, pillarArea, innerRadius, outerRadius};

        auto pillarGap = Tarcog::ISO15099::Layers::createPillar(pillar, gapPressure);

        if(pillarGap == nullptr)
            throw std::runtime_error("Failed to create pillar gap.");

        auto windowWidth = 1.0;   //[m]
        auto windowHeight = 1.0;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, pillarGap, layer2});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        Tarcog::ISO15099::CSingleSystem tarcogSystem(aIGU, Indoor, Outdoor);
        return tarcogSystem;
    }

public:
    [[nodiscard]] Tarcog::ISO15099::CSingleSystem & GetSystem()
    {
        return m_TarcogSystem;
    }
};

TEST_F(NFRC102_NFRC102_VacuumAnnulusCylinderPillar, Test1)
{
    constexpr auto tolerance = 1e-6;

    const auto aSystem = GetSystem();

    const auto Temperature = aSystem.getTemperatures();
    std::vector correctTemperature = {258.203956, 258.475964, 281.611256, 281.883264};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], tolerance);
    }

    const auto Radiosity = aSystem.getRadiosities();
    std::vector correctRadiosity = {250.132403, 267.347632, 342.304986, 368.601704};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], tolerance);
    }

    const auto numOfIter = aSystem.getNumberOfIterations();
    EXPECT_EQ(31u, numOfIter);

    const auto uValue{aSystem.getUValue()};
    EXPECT_NEAR(2.288245, uValue, tolerance);
}
