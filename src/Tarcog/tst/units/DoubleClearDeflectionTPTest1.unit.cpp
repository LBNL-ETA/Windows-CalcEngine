#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"
#include "WCECommon.hpp"

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
        auto airTemperature = 255.15;   // Kelvins
        auto airSpeed = 5.5;            // meters per second
        auto tSky = 255.15;             // Kelvins
        auto solarRadiation = 0.0;

        auto Outdoor = Tarcog::ISO15099::Environments::outdoor(
          airTemperature, airSpeed, solarRadiation, tSky, Tarcog::ISO15099::SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(Tarcog::ISO15099::BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        // Indoor
        /////////////////////////////////////////////////////////

        auto roomTemperature = 294.15;

        auto Indoor = Tarcog::ISO15099::Environments::indoor(roomTemperature);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        // IGU
        /////////////////////////////////////////////////////////
        auto solidLayerThickness1 = 0.003048;   // [m]
        auto solidLayerThickness2 = 0.005715;
        auto solidLayerConductance = 1.0;

        auto aSolidLayer1 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness1, solidLayerConductance);

        // Introducing non default deflection properties
        auto youngsModulus = 8.1e10;
        aSolidLayer1 = Tarcog::ISO15099::Layers::updateDeflectionCoefficients(aSolidLayer1, youngsModulus);

        // Layer will be using default deflection values
        auto aSolidLayer2 =
          Tarcog::ISO15099::Layers::solid(solidLayerThickness2, solidLayerConductance);

        auto gapThickness = 0.0127;
        auto m_GapLayer = Tarcog::ISO15099::Layers::gap(gapThickness);
        ASSERT_TRUE(m_GapLayer != nullptr);

        double windowWidth = 1;
        double windowHeight = 1;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({aSolidLayer1, m_GapLayer, aSolidLayer2});

        // Alternative way of putting layers in
        // aIGU.addLayer(aSolidLayer1);
        // aIGU.addLayer(m_GapLayer);
        // aIGU.addLayer(aSolidLayer2);

        // Deflection properties
        auto Tini = 303.15;
        auto Pini = 101325.0;
        aIGU.setDeflectionProperties(Tini, Pini);

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_shared<Tarcog::ISO15099::CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    std::shared_ptr<Tarcog::ISO15099::CSingleSystem> GetSystem() const
    {
        return m_TarcogSystem;
    };
};

TEST_F(DoubleClearDeflectionTPTest1, Test1)
{
    SCOPED_TRACE("Begin Test: Double Clear - Calculated Deflection");

    auto aSystem = GetSystem();
    ASSERT_TRUE(aSystem != nullptr);

    ///////////////////////////////////////////////////////////////////////////////
    /// Temperatures test
    ///////////////////////////////////////////////////////////////////////////////
    auto Temperature = aSystem->getTemperatures();
    std::vector<double> correctTemperature = {258.799794, 259.124997, 279.007776, 279.617532};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], 1e-5);
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// Radiosity test
    ///////////////////////////////////////////////////////////////////////////////
    auto Radiosity = aSystem->getRadiosities();
    std::vector<double> correctRadiosity = {252.093139, 267.753397, 331.446036, 359.050103};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], 1e-5);
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// Max deflection test
    ///////////////////////////////////////////////////////////////////////////////
    const auto MaxDeflection = aSystem->getMaxDeflections();
    std::vector<double> correctMaxDeflection = {-2.256997e-3, 0.531714e-3};
    ASSERT_EQ(correctMaxDeflection.size(), MaxDeflection.size());

    for(auto i = 0u; i < correctMaxDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctMaxDeflection[i], MaxDeflection[i], 1e-8);
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// Mean deflection test
    ///////////////////////////////////////////////////////////////////////////////
    const auto MeanDeflection = aSystem->getMeanDeflections();
    std::vector<double> correctMeanDeflection = {-0.945515e-3, 0.222749e-3};
    ASSERT_EQ(correctMeanDeflection.size(), MeanDeflection.size());

    for(auto i = 0u; i < correctMaxDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctMeanDeflection[i], MeanDeflection[i], 1e-8);
    }

    const auto numOfIter = aSystem->getNumberOfIterations();
    EXPECT_EQ(20u, numOfIter);
}
