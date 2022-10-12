#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"
#include "WCECommon.hpp"

class TripleClearDeflectionMeasured : public testing::Test
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
        auto solidLayerThickness1 = 0.003048;   // [m]
        auto solidLayerThickness2 = 0.005715;
        auto solidLayerConductance = 1.0;

        auto layer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness1, solidLayerConductance);

        auto layer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness2, solidLayerConductance);

        auto layer3 = Tarcog::ISO15099::Layers::solid(solidLayerThickness1, solidLayerConductance);

        auto gapThickness = 0.0127;
        auto gapPressure = 101325.0;
        auto gap1 = Tarcog::ISO15099::Layers::gap(gapThickness, gapPressure);
        ASSERT_TRUE(gap1 != nullptr);

        auto gap2 = Tarcog::ISO15099::Layers::gap(gapThickness, gapPressure);
        ASSERT_TRUE(gap2 != nullptr);

        auto windowWidth = 1.0;
        auto windowHeight = 1.0;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, gap1, layer2, gap2, layer3});

        // Deflection properties
        std::vector<double> measuredGapsWidths = {0.0135, 0.013};
        aIGU.setDeflectionProperties(measuredGapsWidths);

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
    }
};

TEST_F(TripleClearDeflectionMeasured, Test1)
{
    SCOPED_TRACE("Begin Test: Triple Clear - Measured Deflection.");

    constexpr auto Tolerance = 1e-6;
    constexpr auto DeflectionTolerance = 1e-7;

    auto aSystem = GetSystem();
    ASSERT_TRUE(aSystem != nullptr);

    const auto Temperature = aSystem->getTemperatures();
    std::vector correctTemperature{
      257.493976, 257.702652, 271.535517, 271.926785, 284.395405, 284.604082};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], Tolerance);
    }

    const auto Radiosity = aSystem->getRadiosities();
    std::vector correctRadiosity{
      247.813715, 258.078374, 300.200818, 318.403140, 362.495875, 380.380188};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], Tolerance);
    }

    const auto MaxDeflection = aSystem->getMaxDeflections();
    std::vector correctMaxDeflection{0.00074180, -5.820e-05, -0.0003582};
    ASSERT_EQ(correctMaxDeflection.size(), MaxDeflection.size());

    for(auto i = 0u; i < correctMaxDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctMaxDeflection[i], MaxDeflection[i], DeflectionTolerance);
    }

    const auto MeanDeflection = aSystem->getMeanDeflections();
    std::vector correctMeanDeflection{0.00031076, -2.437e-05, -0.0001501};
    ASSERT_EQ(correctMeanDeflection.size(), MeanDeflection.size());

    for(auto i = 0u; i < correctMaxDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctMeanDeflection[i], MeanDeflection[i], DeflectionTolerance);
    }

    auto numOfIter = aSystem->getNumberOfIterations();
    EXPECT_EQ(20u, numOfIter);
}
