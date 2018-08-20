#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCETarcog.hpp"
#include "WCECommon.hpp"

using namespace Tarcog;
using namespace FenestrationCommon;

class DoubleLowEVacuumCircularPillar : public testing::Test
{
private:
    std::shared_ptr<CSingleSystem> m_TarcogSystem;

protected:
    void SetUp() override
    {
        /////////////////////////////////////////////////////////
        /// Outdoor
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
        /// Indoor
        /////////////////////////////////////////////////////////

        auto roomTemperature = 294.15;

        auto Indoor = Environments::indoor(roomTemperature, pressure);
        ASSERT_TRUE(Indoor != nullptr);

        /////////////////////////////////////////////////////////
        /// IGU
        /////////////////////////////////////////////////////////
        auto solidLayerThickness = 0.004;   // [m]
        auto solidLayerConductance = 1.0;
        auto TransmittanceIR = 0.0;
        auto emissivityFrontIR = 0.84;
        auto emissivityBackIR = 0.036749500781;

        auto aSolidLayer1 = Layers::solid(solidLayerThickness,
                                          solidLayerConductance,
                                          emissivityFrontIR,
                                          TransmittanceIR,
                                          emissivityBackIR,
                                          TransmittanceIR);

        solidLayerThickness = 0.003962399904;
        emissivityBackIR = 0.84;

        auto aSolidLayer2 = Layers::solid(solidLayerThickness,
                                          solidLayerConductance,
                                          emissivityFrontIR,
                                          TransmittanceIR,
                                          emissivityBackIR,
                                          TransmittanceIR);

        auto gapThickness = 0.0001;
        auto gapPressure = 0.1333;
        auto aGapLayer = Layers::gap(gapThickness, gapPressure);

        // Add support pillars
        auto pillarConductivity = 999.0;
        auto pillarSpacing = 0.03;
        auto pillarRadius = 0.0002;
        auto aGapWithPillars =
          Layers::addCircularPillar(aGapLayer, pillarConductivity, pillarSpacing, pillarRadius);

        ASSERT_TRUE(aGapWithPillars != nullptr);

        auto windowWidth = 1.0;   //[m]
        auto windowHeight = 1.0;
        CIGU aIGU(windowWidth, windowHeight);
        aIGU.addLayer(aSolidLayer1);
        aIGU.addLayer(aGapWithPillars);
        aIGU.addLayer(aSolidLayer2);

        /////////////////////////////////////////////////////////
        /// System
        /////////////////////////////////////////////////////////
        m_TarcogSystem = std::make_shared<CSingleSystem>(aIGU, Indoor, Outdoor);
        ASSERT_TRUE(m_TarcogSystem != nullptr);

        m_TarcogSystem->solve();
    }

public:
    std::shared_ptr<CSingleSystem> GetSystem() const
    {
        return m_TarcogSystem;
    };
};

TEST_F(DoubleLowEVacuumCircularPillar, Test1)
{
    SCOPED_TRACE("Begin Test: Double Low-E - vacuum with circular pillar support");

    auto aSystem = GetSystem();

    ASSERT_TRUE(aSystem != nullptr);

    auto Temperature = *aSystem->getTemperatures();
    std::vector<double> correctTemperature = {255.997063, 256.095933, 290.398479, 290.496419};
    ASSERT_EQ(correctTemperature.size(), Temperature.size());

    for(auto i = 0u; i < correctTemperature.size(); ++i)
    {
        EXPECT_NEAR(correctTemperature[i], Temperature[i], 1e-5);
    }

    auto Radiosity = *aSystem->getRadiosities();
    std::vector<double> correctRadiosity = {242.987484, 396.293176, 402.108090, 407.071738};
    ASSERT_EQ(correctRadiosity.size(), Radiosity.size());

    for(auto i = 0u; i < correctRadiosity.size(); ++i)
    {
        EXPECT_NEAR(correctRadiosity[i], Radiosity[i], 1e-5);
    }

    auto numOfIter = aSystem->getNumberOfIterations();
    EXPECT_EQ(21u, numOfIter);
}
