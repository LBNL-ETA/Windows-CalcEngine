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
        auto solidLayerConductance = 1.0;

        auto layer1 = Tarcog::ISO15099::Layers::solid(solidLayerThickness1, solidLayerConductance);

        auto layer2 = Tarcog::ISO15099::Layers::solid(solidLayerThickness1, solidLayerConductance);

        auto layer3 = Tarcog::ISO15099::Layers::solid(solidLayerThickness1, solidLayerConductance);

        const auto gapThickness1 = 0.006;
        const auto gapThickness2 = 0.025;
        auto gapPressure = Gases::DefaultPressure;
        auto gap1 = Tarcog::ISO15099::Layers::gap(gapThickness1, gapPressure);
        ASSERT_TRUE(gap1 != nullptr);

        auto gap2 = Tarcog::ISO15099::Layers::gap(gapThickness2, gapPressure);
        ASSERT_TRUE(gap2 != nullptr);

        auto windowWidth = 1.0;
        auto windowHeight = 1.0;
        Tarcog::ISO15099::CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayers({layer1, gap1, layer2, gap2, layer3});

        /////////////////////////////////////////////////////////
        // System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_shared<Tarcog::ISO15099::CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        // Deflection properties
        std::vector<double> measuredGapsWidths = {0.0135, 0.013};
        m_TarcogSystem->setDeflectionProperties(measuredGapsWidths);

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
    std::vector correctTemperature{257.56333804292137,
                                   257.778199167472,
                                   270.38163708188478,
                                   270.5964982064354,
                                   283.89299684982035,
                                   284.10785797437092};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], Tolerance);
    }

    const auto Radiosity = aSystem->getRadiosities();
    std::vector correctRadiosity{248.03939792628864,
                                 257.61321393304598,
                                 295.7534834937947,
                                 312.85131025670762,
                                 359.41188170064646,
                                 378.20666480049886};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], Tolerance);
    }

    const auto MaxDeflection = aSystem->getMaxLayerDeflections();
    std::vector correctMaxDeflection{0.001, -0.0065, 0.0055};
    ASSERT_EQ(correctMaxDeflection.size(), MaxDeflection.size());

    for(auto i = 0u; i < correctMaxDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctMaxDeflection[i], MaxDeflection[i], DeflectionTolerance);
    }

    const auto MeanDeflection = aSystem->getMeanLayerDeflections();
    std::vector correctMeanDeflection{
      0.00041892601712781834, -0.0027230191113308219, 0.0023040930942030032};
    ASSERT_EQ(correctMeanDeflection.size(), MeanDeflection.size());

    for(auto i = 0u; i < correctMaxDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctMeanDeflection[i], MeanDeflection[i], DeflectionTolerance);
    }

    auto numOfIter = aSystem->getNumberOfIterations();
    EXPECT_EQ(24u, numOfIter);
}
