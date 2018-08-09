#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"
#include "WCECommon.hpp"

using namespace Tarcog;
using namespace FenestrationCommon;

class DoubleClearDeflectionTPDefault : public testing::Test
{
private:
    std::shared_ptr<CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        // Outdoor
        /////////////////////////////////////////////////////////
        auto airTemperature = 255.15;   // Kelvins
        auto pressure = 101325.0;       // Pascals
        auto airSpeed = 5.5;            // meters per second
        auto tSky = 255.15;   // Kelvins
        auto solarRadiation = 0.0;

        auto Outdoor = Environments::outdoor(airTemperature,
                                             pressure,
                                             airSpeed,
                                             solarRadiation,
                                             tSky,
                                             SkyModel::AllSpecified);
        ASSERT_TRUE(Outdoor != nullptr);
        Outdoor->setHCoeffModel(BoundaryConditionsCoeffModel::CalculateH);

        /////////////////////////////////////////////////////////
        // Indoor
        /////////////////////////////////////////////////////////

        auto roomTemperature = 294.15;

        auto Indoor = Environments::indoor(roomTemperature, pressure);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        auto solidLayerThickness1 = 0.003048;   // [m]
        auto solidLayerThickness2 = 0.005715;
        auto solidLayerConductance = 1.0;

        auto aSolidLayer1 = Layers::solid(solidLayerThickness1, solidLayerConductance);

        auto aSolidLayer2 = Layers::solid(solidLayerThickness2, solidLayerConductance);

        auto gapThickness = 0.0127;
        auto gapPressure = 101325.0;
        auto gapLayer = Layers::gap(gapThickness, gapPressure);
        ASSERT_TRUE(gapLayer != nullptr);

        double windowWidth = 1;
        double windowHeight = 1;
        CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayer(aSolidLayer1);
        aIGU.addLayer(gapLayer);
        aIGU.addLayer(aSolidLayer2);

        // Deflection properties
        auto Tini = 303.15;
        auto Pini = 101325.0;
        aIGU.setDeflectionProperties(Tini, Pini);

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_shared<CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    std::shared_ptr<CSingleSystem> getSystem() const
    {
        return m_TarcogSystem;
    }
};

TEST_F(DoubleClearDeflectionTPDefault, Test1)
{
    SCOPED_TRACE("Begin Test: Double Clear - Calculated Deflection");

    auto aSystem = getSystem();
    ASSERT_TRUE(aSystem != nullptr);

    auto Temperature = *aSystem->getTemperatures();
    std::vector<double> correctTemperature = {258.811500, 259.137749, 278.961419, 279.573136};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], 1e-5);
    }

    auto Radiosity = *aSystem->getRadiosities();
    std::vector<double> correctRadiosity = {252.131797, 267.765290, 331.256183, 358.865247};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], 1e-5);
    }

    auto MaxDeflection = *aSystem->getMaxDeflections();
    std::vector<double> correctMaxDeflection = {-0.0031162, 0.00029386};
    ASSERT_EQ(correctMaxDeflection.size(), MaxDeflection.size());

    for(auto i = 0u; i < correctMaxDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctMaxDeflection[i], MaxDeflection[i], 1e-5);
    }

    auto MeanDeflection = *aSystem->getMeanDeflections();
    std::vector<double> correctMeanDeflection = {-0.0013055, 0.00012311};
    ASSERT_EQ(correctMeanDeflection.size(), MeanDeflection.size());

    for(auto i = 0u; i < correctMaxDeflection.size(); ++i)
    {
        EXPECT_NEAR(correctMeanDeflection[i], MeanDeflection[i], 1e-5);
    }

    auto numOfIter = aSystem->getNumberOfIterations();
    EXPECT_EQ(25u, numOfIter);
}
