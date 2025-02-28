#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

#include "vectorTesting.hpp"

// Example of double clear window with inital guess for solution
class TestDoubleLoweEnvironmentWithDeflection : public testing::Test
{
private:
    std::shared_ptr<Tarcog::ISO15099::CSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        const auto airTemperature{255.15};   // Kelvins
        const auto airSpeed{5.5};            // meters per second
        const auto tSky{255.15};             // Kelvins
        const auto solarRadiation{783.0};

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        /// Indoor
        /////////////////////////////////////////////////////////

        const auto roomTemperature{294.15};
        const auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        // IGU
        /////////////////////////////////////////////////////////
        const auto solidLayerThickness1{0.00318};   // [m]
        const auto solidLayerConductance1{1.0};
        const auto tIR1{0.0};
        const auto frontEmissivity1{0.84};
        const auto backEmissivity1{0.046578168869};

        const auto layer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness1,
                                                            solidLayerConductance1,
                                                            frontEmissivity1,
                                                            tIR1,
                                                            backEmissivity1,
                                                            tIR1);

        layer1->setSolarHeatGain(0.194422408938, solarRadiation);
        ASSERT_TRUE(layer1 != nullptr);

        const auto gapThickness{0.0127};
        auto gap{Tarcog::ISO15099::Layers::gap(gapThickness)};

        const auto solidLayerThickness2{0.005715};   // [m]
        const auto solidLayerConductance2{1.0};

        const auto layer2 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness2, solidLayerConductance2);

        layer2->setSolarHeatGain(0.054760526866, solarRadiation);

        const auto iguWidth{1.0};
        const auto iguHeight{1.0};
        Tarcog::ISO15099::CIGU aIGU(iguWidth, iguHeight);
        aIGU.addLayers({layer1, gap, layer2});

        const double initialTemperature{293.15};
        const double initialPressure{101325};
        aIGU.setDeflectionProperties(initialTemperature, initialPressure);

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_shared<Tarcog::ISO15099::CSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);
    }

public:
    [[nodiscard]] std::shared_ptr<Tarcog::ISO15099::CSystem> GetSystem() const
    {
        return m_TarcogSystem;
    }
};

TEST_F(TestDoubleLoweEnvironmentWithDeflection, Test1)
{
    SCOPED_TRACE("Begin Test: Double Low-e - Deflection Results");

    constexpr auto Tolerance = 1e-6;
    constexpr auto DeflectionTolerance = 1e-8;

    const auto aSystem = GetSystem();
    ASSERT_TRUE(aSystem != nullptr);

    ///////////////////////////////////////////////////////////////////////////////
    /// Deflection results
    ///////////////////////////////////////////////////////////////////////////////
    const std::vector correctMaxDeflectionU{-1.849876e-3, 0.343977e-3};
    Helper::testVectors("Max deflection U-value run",
                        correctMaxDeflectionU,
                        aSystem->getMaxLayerDeflections(Tarcog::ISO15099::System::Uvalue),
                        DeflectionTolerance);

    const std::vector correctMaxDeflectionSHGC{-1.385198e-3, 0.252978e-3};
    Helper::testVectors("Max deflection SHGC run",
                        correctMaxDeflectionSHGC,
                        aSystem->getMaxLayerDeflections(Tarcog::ISO15099::System::SHGC),
                        DeflectionTolerance);

    //////////////////////////////////////////////////////////////////////
    /// General results
    //////////////////////////////////////////////////////////////////////

    const auto numOfIterU = aSystem->getNumberOfIterations(Tarcog::ISO15099::System::Uvalue);
    EXPECT_EQ(26u, numOfIterU);

    const auto numOfIterS = aSystem->getNumberOfIterations(Tarcog::ISO15099::System::SHGC);
    EXPECT_EQ(26u, numOfIterS);

    const auto Uvalue = aSystem->getUValue();
    EXPECT_NEAR(Uvalue, 1.693616, Tolerance);

    const auto SHGC = aSystem->getSHGC(0.3716);
    EXPECT_NEAR(SHGC, 0.425337, Tolerance);
}
