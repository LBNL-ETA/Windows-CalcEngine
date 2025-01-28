#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"

#include "vectorTesting.hpp"

class DoubleClearDeflectionTPTest1 : public testing::Test
{
private:
    std::shared_ptr<Tarcog::ISO15099::CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
        /////////////////////////////////////////////////////////
        constexpr auto airTemperature = 255.15;   // Kelvins
        constexpr auto airSpeed = 5.5;            // meters per second
        constexpr auto tSky = 255.15;             // Kelvins
        constexpr auto solarRadiation = 0.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        // Indoor
        /////////////////////////////////////////////////////////

        constexpr auto roomTemperature = 294.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        // IGU
        /////////////////////////////////////////////////////////
        constexpr auto solidLayerThickness1 = 0.003048;   // [m]
        constexpr auto solidLayerThickness2 = 0.005715;
        constexpr auto solidLayerConductance = 1.0;

        auto aSolidLayer1 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness1, solidLayerConductance);

        // Introducing non default deflection properties
        constexpr auto youngsModulus = 8.1e10;
        aSolidLayer1 = Tarcog::ISO15099::Layers::updateMaterialData(aSolidLayer1, youngsModulus);

        // Layer will be using default deflection values
        auto aSolidLayer2 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness2, solidLayerConductance);

        constexpr auto gapThickness = 0.0127;
        auto m_GapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(m_GapLayer != nullptr);

        constexpr double windowWidth = 1;
        constexpr double windowHeight = 1;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({aSolidLayer1, m_GapLayer, aSolidLayer2});

        // Deflection properties
        constexpr auto Tini = 303.15;
        constexpr auto Pini = 101325.0;
        aIGU.setDeflectionProperties(Tini, Pini);

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_shared<Tarcog::ISO15099::CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    [[nodiscard]] std::shared_ptr<Tarcog::ISO15099::CSingleSystem> GetSystem() const
    {
        return m_TarcogSystem;
    };
};

TEST_F(DoubleClearDeflectionTPTest1, Test1)
{
    SCOPED_TRACE("Begin Test: Double Clear - Calculated Deflection");

    constexpr double Tolerance = 1e-6;

    auto aSystem = GetSystem();
    ASSERT_TRUE(aSystem != nullptr);

    ///////////////////////////////////////////////////////////////////////////////
    /// Temperatures test
    ///////////////////////////////////////////////////////////////////////////////
    std::vector correctTemperature = {258.795861, 259.120713, 279.023351, 279.632449};
    Helper::testVectors("Temperature", correctTemperature, aSystem->getTemperatures(), Tolerance);

    ///////////////////////////////////////////////////////////////////////////////
    /// Radiosity test
    ///////////////////////////////////////////////////////////////////////////////
    std::vector correctRadiosity = {252.080153, 267.749407, 331.509846, 359.112232};
    Helper::testVectors("Radiosity", correctRadiosity, aSystem->getRadiosities(), Tolerance);

    ///////////////////////////////////////////////////////////////////////////////
    /// Heat flow
    ///////////////////////////////////////////////////////////////////////////////
    constexpr auto correctHeatFlow = 106.578839;
    EXPECT_NEAR(
      correctHeatFlow, aSystem->getHeatFlow(Tarcog::ISO15099::Environment::Indoor), Tolerance);

    ///////////////////////////////////////////////////////////////////////////////
    /// Effective thermal conductivity
    ///////////////////////////////////////////////////////////////////////////////
    std::vector correctEffectiveTC = {0.061796};
    Helper::testVectors("Effective Thermal Conductivity",
                        correctEffectiveTC,
                        aSystem->getGapEffectiveLayerConductivities(),
                        Tolerance);

    ///////////////////////////////////////////////////////////////////////////////
    /// Max deflection test
    ///////////////////////////////////////////////////////////////////////////////
    std::vector<double> correctMaxDeflection = {-2.28568e-3, 0.483674e-3};
    Helper::testVectors(
      "Max deflection", correctMaxDeflection, aSystem->getMaxLayerDeflections(), 1e-8);

    ///////////////////////////////////////////////////////////////////////////////
    /// Mean deflection test
    ///////////////////////////////////////////////////////////////////////////////
    std::vector correctMeanDeflection = {-0.957531e-3, 0.202624e-3};
    Helper::testVectors(
      "Mean deflection", correctMeanDeflection, aSystem->getMeanLayerDeflections(), 1e-8);

    const auto numOfIter = aSystem->getNumberOfIterations();
    EXPECT_EQ(25u, numOfIter);
}
