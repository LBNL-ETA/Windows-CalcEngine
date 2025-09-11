#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"
#include "WCECommon.hpp"

class DoubleLowEVacuumNoPillar : public testing::Test
{
private:
    Tarcog::ISO15099::CSingleSystem m_TarcogSystem{createTarcogSystem()};

protected:
    void SetUp() override
    {
        m_TarcogSystem.solve();
    }

    static Tarcog::ISO15099::CSingleSystem createTarcogSystem()
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

        solidLayerThickness = 0.003962399904;
        emissivityBackIR = 0.84;

        auto layer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness,
                                                      solidLayerConductance,
                                                      emissivityFrontIR,
                                                      TransmittanceIR,
                                                      emissivityBackIR,
                                                      TransmittanceIR);

        auto gapThickness = 0.0001;
        auto gapPressure = 0.1333;
        auto m_GapLayer = Tarcog::ISO15099::Layers::gap(gapThickness, gapPressure);
        if(m_GapLayer == nullptr)
            throw std::runtime_error("Failed to create gap layer.");

        auto windowWidth = 1.0;   //[m]
        auto windowHeight = 1.0;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, m_GapLayer, layer2});

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

TEST_F(DoubleLowEVacuumNoPillar, Test1)
{
    SCOPED_TRACE("Begin Test: Double Low-E - vacuum with no pillar support");

    constexpr auto Tolerance = 1e-6;

    auto aSystem = GetSystem();

    const auto Temperature = aSystem.getTemperatures();
    const std::vector correctTemperature{255.530983, 255.575438, 292.387411, 292.431448};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], Tolerance);
    }

    const auto Radiosity = aSystem.getRadiosities();
    std::vector correctRadiosity{241.501961, 406.882895, 413.177234, 416.199118};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], Tolerance);
    }

    const auto numOfIter = aSystem.getNumberOfIterations();
    EXPECT_EQ(36u, numOfIter);
}
