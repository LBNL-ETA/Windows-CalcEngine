#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

#include "vectorTesting.hpp"

// Example of double clear window with inital guess for solution
class TestTripleClearDeflectionWithLoad : public testing::Test
{
private:
    std::shared_ptr<Tarcog::ISO15099::CSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        constexpr auto airTemperature{250};      // Kelvins
        constexpr auto airSpeed{5.5};            // meters per second
        constexpr auto tSky{255.15};             // Kelvins
        constexpr auto solarRadiation{0.0};

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        constexpr auto roomTemperature{293.0};

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        constexpr auto solidLayerThickness{0.003048}; // [m]
        constexpr auto solidLayerConductance{1.0};    // [W/m2K]

        auto aSolidLayer1 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer1->setSolarAbsorptance(0.099839858711, solarRadiation);

        auto aSolidLayer2 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer2->setSolarAbsorptance(0.076627746224, solarRadiation);

        auto aSolidLayer3 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer3->setSolarAbsorptance(0.058234799653, solarRadiation);

        constexpr auto gapThickness1{0.006};
        const auto gapLayer1 = Tarcog::ISO15099::Layers::gap(gapThickness1);
        ASSERT_TRUE(gapLayer1 != nullptr);

        constexpr auto gapThickness2{0.025};
        const auto gapLayer2 = Tarcog::ISO15099::Layers::gap(gapThickness2);
        ASSERT_TRUE(gapLayer2 != nullptr);

        constexpr auto windowWidth{1.0};
        constexpr auto windowHeight{1.0};
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({aSolidLayer1, gapLayer1, aSolidLayer2, gapLayer2, aSolidLayer3});

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_shared<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
        m_TarcogSystem->setAppliedLoad({0, 0, 100000});
        m_TarcogSystem->setDeflectionProperties(273, 101325);
        ASSERT_TRUE(m_TarcogSystem != nullptr);
    }

public:
    [[nodiscard]] std::shared_ptr<Tarcog::ISO15099::CSystem> GetSystem() const
    {
        return m_TarcogSystem;
    }
};

TEST_F(TestTripleClearDeflectionWithLoad, Test1)
{
    SCOPED_TRACE("Begin Test: Double Clear - Surface temperatures");

    constexpr auto Tolerance = 1e-6;
    constexpr auto DeflectionTolerance = 1e-8;

    const auto aSystem = GetSystem();
    ASSERT_TRUE(aSystem != nullptr);

    //////////////////////////////////////////////////////////////////////
    /// UValue run
    //////////////////////////////////////////////////////////////////////

    auto aRun = Tarcog::ISO15099::System::Uvalue;

    const auto Temperature = aSystem->getTemperatures(aRun);
    const std::vector correctTemperature{253.442695, 253.723313, 266.443064, 266.723682, 280.000269, 280.280887};
    testVectors("Surface temperatures", Temperature, correctTemperature, Tolerance);

    const std::vector correctDeflection{22.808033e-3, 24.468037e-3, 63.330606e-3};
    const auto deflection = aSystem->getMaxLayerDeflections(Tarcog::ISO15099::System::Uvalue);
    testVectors("Maximum deflection", deflection, correctDeflection, DeflectionTolerance);
}
