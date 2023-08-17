#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"
#include "WCECommon.hpp"

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
        const auto airTemperature{250};      // Kelvins
        const auto airSpeed{5.5};            // meters per second
        const auto tSky{255.15};             // Kelvins
        const auto solarRadiation{0.0};

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        const auto roomTemperature{293.0};

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        const auto solidLayerThickness{0.003048}; // [m]
        const auto solidLayerConductance{1.0};    // [W/m2K]

        auto aSolidLayer1 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer1->setSolarAbsorptance(0.099839858711, solarRadiation);

        auto aSolidLayer2 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer2->setSolarAbsorptance(0.076627746224, solarRadiation);

        auto aSolidLayer3 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer3->setSolarAbsorptance(0.058234799653, solarRadiation);

        const auto gapThickness1{0.006};
        const auto gapLayer1 = Tarcog::ISO15099::Layers::gap(gapThickness1);
        ASSERT_TRUE(gapLayer1 != nullptr);

        const auto gapThickness2{0.025};
        const auto gapLayer2 = Tarcog::ISO15099::Layers::gap(gapThickness2);
        ASSERT_TRUE(gapLayer2 != nullptr);

        const auto windowWidth{1.0};
        const auto windowHeight{1.0};
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({aSolidLayer1, gapLayer1, aSolidLayer2, gapLayer2, aSolidLayer3});

        //aIGU.setDeflectionProperties(273, 101325);

        // Alternative way of adding layers.
        // aIGU.addLayer(aSolidLayer1);
        // aIGU.addLayer(gapLayer);
        // aIGU.addLayer(aSolidLayer2);

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
    const std::vector correctTemperature{253.314583, 253.583839, 265.810776, 266.080031, 280.499960, 280.769216};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], Tolerance);
    }

    const std::vector correctDeflection{22.784211e-3, 24.460877e-3, 63.338034e-3};

    const auto deflection = aSystem->getMaxLayerDeflections(Tarcog::ISO15099::System::Uvalue);

    for(auto i = 0u; i < correctDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctDeflection[i], deflection[i], DeflectionTolerance);
    }
}
