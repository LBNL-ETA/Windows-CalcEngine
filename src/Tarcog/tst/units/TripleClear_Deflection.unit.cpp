#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

#include "vectorTesting.hpp"

// Example of double clear window with inital guess for solution
class TestTripleClearDeflection : public testing::Test
{
private:
    std::shared_ptr<Tarcog::ISO15099::CSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        constexpr auto airTemperature{250};   // Kelvins
        constexpr auto airSpeed{5.5};         // meters per second
        constexpr auto tSky{255.15};          // Kelvins
        constexpr auto solarRadiation{783.0};

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
        constexpr auto solidLayerThickness{0.003048};   // [m]
        constexpr auto solidLayerConductance{1.0};      // [W/m2K]

        auto aSolidLayer1 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer1->setSolarHeatGain(0.099839858711, solarRadiation);

        auto aSolidLayer2 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer2->setSolarHeatGain(0.076627746224, solarRadiation);

        auto aSolidLayer3 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness, solidLayerConductance);
        aSolidLayer3->setSolarHeatGain(0.058234799653, solarRadiation);

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

        m_TarcogSystem->setDeflectionProperties(273, 101325);
        ASSERT_TRUE(m_TarcogSystem != nullptr);
    }

public:
    [[nodiscard]] std::shared_ptr<Tarcog::ISO15099::CSystem> GetSystem() const
    {
        return m_TarcogSystem;
    }
};

TEST_F(TestTripleClearDeflection, Test1)
{
    SCOPED_TRACE("Begin Test: Double Clear - Surface temperatures");

    constexpr auto Tolerance = 1e-6;
    constexpr auto DeflectionTolerance = 1e-8;

    auto aSystem = GetSystem();
    ASSERT_TRUE(aSystem != nullptr);

    //////////////////////////////////////////////////////////////////////
    /// UValue run
    //////////////////////////////////////////////////////////////////////

    auto aRun = Tarcog::ISO15099::System::Uvalue;

    auto Temperature = aSystem->getTemperatures(aRun);
    std::vector correctTemperature{
      253.145128, 253.399356, 265.491273, 265.745502, 281.162057, 281.416285};
    Helper::testVectors("U-value run temperatures", Temperature, correctTemperature, Tolerance);

    std::vector correctDeflection{-0.421986e-3, 0.265021e-3, 0.167762e-3};
    Helper::testVectors("U-value run maximum deflection",
                        aSystem->getMaxLayerDeflections(Tarcog::ISO15099::System::Uvalue),
                        correctDeflection,
                        DeflectionTolerance);

    auto numOfIter = aSystem->getNumberOfIterations(aRun);
    EXPECT_EQ(24u, numOfIter);

    //////////////////////////////////////////////////////////////////////
    /// SHGC run
    //////////////////////////////////////////////////////////////////////

    aRun = Tarcog::ISO15099::System::SHGC;

    Temperature = aSystem->getTemperatures(aRun);
    correctTemperature = {257.436181, 257.952788, 276.188735, 276.494764, 289.161417, 289.306516};
    Helper::testVectors(
      "SHGC run temperatures", aSystem->getTemperatures(aRun), correctTemperature, Tolerance);

    correctDeflection = {-0.421986e-3, 0.265021e-3, 0.167762e-3};
    Helper::testVectors("SHGC run maximum deflection",
                        aSystem->getMaxLayerDeflections(Tarcog::ISO15099::System::Uvalue),
                        correctDeflection,
                        DeflectionTolerance);

    numOfIter = aSystem->getNumberOfIterations(aRun);
    EXPECT_EQ(27u, numOfIter);

    //////////////////////////////////////////////////////////////////////
    /// General results
    //////////////////////////////////////////////////////////////////////
    const auto Uvalue = aSystem->getUValue();
    EXPECT_NEAR(Uvalue, 1.952304, Tolerance);

    const auto SHGC = aSystem->getSHGC(0.598424255848);
    EXPECT_NEAR(SHGC, 0.673268, Tolerance);

    const auto relativeHeatGain = aSystem->relativeHeatGain(0.703296);
    EXPECT_NEAR(relativeHeatGain, 579.474468, Tolerance);
}
